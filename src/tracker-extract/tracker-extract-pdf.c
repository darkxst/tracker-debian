/*
 * Copyright (C) 2006, Jamie McCracken <jamiemcc@gnome.org>
 * Copyright (C) 2008-2009, Nokia <ivan.frade@nokia.com>
 * Copyright (C) 2010, Amit Aggarwal <amitcs06@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "config.h"

#include <string.h>

#include <glib.h>
#include <poppler.h>

#include <libtracker-common/tracker-date-time.h>
#include <libtracker-common/tracker-utils.h>

#include <libtracker-extract/tracker-extract.h>

#include "tracker-main.h"

typedef struct {
	gchar *title;
	gchar *subject;
	gchar *creation_date;
	gchar *author;
	gchar *date;
	gchar *keywords;
} PDFData;

static void extract_pdf (const gchar          *uri,
                         TrackerSparqlBuilder *preupdate,
                         TrackerSparqlBuilder *metadata);

static TrackerExtractData data[] = {
	{ "application/pdf", extract_pdf },
	{ NULL, NULL }
};

static void
read_toc (PopplerIndexIter  *index,
          GString          **toc)
{
        if (!index) {
	        return;
        }

        if (!*toc) {
	        *toc = g_string_new ("");
        }

        do {
                PopplerAction *action;
                PopplerIndexIter *iter;

                action = poppler_index_iter_get_action (index);

                if (!action) {
                        continue;
                }

                switch (action->type) {
                case POPPLER_ACTION_GOTO_DEST: {
                        PopplerActionGotoDest *ag = (PopplerActionGotoDest*) action;
                        PopplerDest *agd = ag->dest;

                        if (!tracker_is_empty_string (ag->title)) {
	                        g_string_append_printf (*toc, "%s ", ag->title);
                        }

                        if (!tracker_is_empty_string (agd->named_dest)) {
	                        g_string_append_printf (*toc, "%s ", agd->named_dest);
                        }

                        break;
                }

                case POPPLER_ACTION_LAUNCH: {
                        PopplerActionLaunch *al = (PopplerActionLaunch*) action;

                        if (!tracker_is_empty_string (al->title)) {
	                        g_string_append_printf (*toc, "%s ", al->title);
                        }

                        if (!tracker_is_empty_string (al->file_name)) {
	                        g_string_append_printf (*toc, "%s ", al->file_name);
                        }

                        if (!tracker_is_empty_string (al->params)) {
	                        g_string_append_printf (*toc, "%s ", al->params);
                        }

                        break;
                }

                case POPPLER_ACTION_URI: {
                        PopplerActionUri *au = (PopplerActionUri*) action;

                        if (!tracker_is_empty_string (au->uri)) {
	                        g_string_append_printf (*toc, "%s ", au->uri);
                        }

                        break;
                }

                case POPPLER_ACTION_NAMED: {
                        PopplerActionNamed *an = (PopplerActionNamed*) action;

                        if (!tracker_is_empty_string (an->title)) {
	                        g_string_append_printf (*toc, "%s, ", an->title);
                        }

                        if (!tracker_is_empty_string (an->named_dest)) {
                                g_string_append_printf (*toc, "%s ", an->named_dest);
                        }

                        break;
                }

                case POPPLER_ACTION_MOVIE: {
                        PopplerActionNamed *am = (PopplerActionNamed*) action;

                        if (!tracker_is_empty_string (am->title)) {
	                        g_string_append_printf (*toc, "%s ", am->title);
                        }

                        break;
                }

                case POPPLER_ACTION_NONE:
                case POPPLER_ACTION_UNKNOWN:
                case POPPLER_ACTION_GOTO_REMOTE:
                        /* Do nothing */
                        break;
                }

                iter = poppler_index_iter_get_child (index);
                read_toc (iter, toc);
        } while (poppler_index_iter_next (index));

        poppler_index_iter_free (index);
}

static void
read_outline (PopplerDocument      *document,
              TrackerSparqlBuilder *metadata)
{
        PopplerIndexIter *index;
        GString *toc = NULL;

        index = poppler_index_iter_new (document);

        if (!index) {
                return;
        }

        read_toc (index, &toc);

        if (toc) {
	        if (toc->len > 0) {
		        tracker_sparql_builder_predicate (metadata, "nfo:tableOfContents");
		        tracker_sparql_builder_object_unvalidated (metadata, toc->str);
	        }

	        g_string_free (toc, TRUE);
        }
}

static void
insert_keywords (TrackerSparqlBuilder *metadata,
                 gchar                *keywords)
{
	char *saveptr, *p;
	size_t len;

	p = keywords;
	keywords = strchr (keywords, '"');

	if (keywords) {
		keywords++;
	} else {
		keywords = p;
	}

	len = strlen (keywords);
	if (keywords[len - 1] == '"') {
		keywords[len - 1] = '\0';
	}

	for (p = strtok_r (keywords, ",; ", &saveptr);
	     p;
	     p = strtok_r (NULL, ",; ", &saveptr)) {
		tracker_sparql_builder_predicate (metadata, "nao:hasTag");

		tracker_sparql_builder_object_blank_open (metadata);
		tracker_sparql_builder_predicate (metadata, "a");
		tracker_sparql_builder_object (metadata, "nao:Tag");

		tracker_sparql_builder_predicate (metadata, "nao:prefLabel");
		tracker_sparql_builder_object_unvalidated (metadata, p);

		tracker_sparql_builder_object_blank_close (metadata);
	}
}

static gchar *
extract_content (PopplerDocument *document,
                 guint            n_words)
{
	PopplerPage *page;
	PopplerRectangle rect;
	GString *string;
	gint n_pages, i, words;
	gchar *text, *t;

	n_pages = poppler_document_get_n_pages (document);
	string = g_string_new ("");
	words = i = 0;

	while (i < n_pages && words < n_words) {
		gint normalized_words;

		page = poppler_document_get_page (document, i);
		i++;

		rect.x1 = rect.y1 = 0;
		poppler_page_get_size (page, &rect.x2, &rect.y2);

		text = poppler_page_get_text (page, POPPLER_SELECTION_WORD, &rect);
		t = tracker_text_normalize (text, n_words - words, &normalized_words);

		words += normalized_words;
		g_string_append (string, t);

		g_free (text);
		g_free (t);
		g_object_unref (page);
	}

	return g_string_free (string, FALSE);
}

static void
write_pdf_data (PDFData               data,
                TrackerSparqlBuilder *metadata)
{
	if (!tracker_is_empty_string (data.title)) {
		tracker_sparql_builder_predicate (metadata, "nie:title");
		tracker_sparql_builder_object_unvalidated (metadata, data.title);
		g_free (data.title);
	}

	if (!tracker_is_empty_string (data.subject)) {
		tracker_sparql_builder_predicate (metadata, "nie:subject");
		tracker_sparql_builder_object_unvalidated (metadata, data.subject);
		g_free (data.subject);
	}

	if (!tracker_is_empty_string (data.author)) {
		tracker_sparql_builder_predicate (metadata, "nco:creator");
		tracker_sparql_builder_object_blank_open (metadata);
		tracker_sparql_builder_predicate (metadata, "a");
		tracker_sparql_builder_object (metadata, "nco:Contact");
		tracker_sparql_builder_predicate (metadata, "nco:fullname");
		tracker_sparql_builder_object_unvalidated (metadata, data.author);
		tracker_sparql_builder_object_blank_close (metadata);
		g_free (data.author);
	}

	if (!tracker_is_empty_string (data.date)) {
		tracker_sparql_builder_predicate (metadata, "nie:contentCreated");
		tracker_sparql_builder_object_unvalidated (metadata, data.date);
		g_free (data.date);
	}

	if (!tracker_is_empty_string (data.keywords)) {
		insert_keywords (metadata, data.keywords);
		g_free (data.keywords);
	}
}

static void
extract_pdf (const gchar          *uri,
             TrackerSparqlBuilder *preupdate,
             TrackerSparqlBuilder *metadata)
{
	TrackerFTSConfig *fts_config;
	GTime creation_date;
	GError *error = NULL;
	TrackerXmpData xd = { 0 };
	PDFData pd = { 0 }; /* actual data */
	PDFData md = { 0 }; /* for merging */
	PopplerDocument *document;
	gchar *xml = NULL;
	gchar *content;
	guint n_words;

	g_type_init ();

	document = poppler_document_new_from_file (uri, NULL, &error);

	if (error) {
		g_warning ("Couldn't create PopplerDocument from uri:'%s', %s",
		           uri,
		           error->message ? error->message : "no error given");
		g_error_free (error);

		return;
	}

	if (!document) {
		g_warning ("Could not create PopplerDocument from uri:'%s', "
		           "NULL returned without an error",
		           uri);
		return;
	}

	tracker_sparql_builder_predicate (metadata, "a");
	tracker_sparql_builder_object (metadata, "nfo:PaginatedTextDocument");

	g_object_get (document,
	              "title", &pd.title,
	              "author", &pd.author,
	              "subject", &pd.subject,
	              "keywords", &pd.keywords,
	              "creation-date", &creation_date,
	              NULL);

	/* metadata property not present in older poppler versions */
	if (g_object_class_find_property (G_OBJECT_GET_CLASS (document), "metadata")) {
		g_object_get (document, "metadata", &xml, NULL);
	}

	if (creation_date > 0) {
		pd.creation_date = tracker_date_to_string ((time_t) creation_date);
	}

	if (xml) {
		tracker_xmp_read (xml, strlen (xml), uri, &xd);
		g_free (xml);
		xml = NULL;

		md.title = tracker_coalesce (3, pd.title, xd.title, xd.title2, xd.pdf_title);
		md.subject = tracker_coalesce (2, pd.subject, xd.subject);
		md.date = tracker_coalesce (3, pd.creation_date, xd.date, xd.time_original);
		md.author = tracker_coalesce (2, pd.author, xd.creator);

		write_pdf_data (md, metadata);

		if (xd.keywords) {
			insert_keywords (metadata, xd.keywords);
			g_free (xd.keywords);
		}

		if (xd.pdf_keywords) {
			insert_keywords (metadata, xd.pdf_keywords);
			g_free (xd.pdf_keywords);
		}

		if (xd.publisher) {
			tracker_sparql_builder_predicate (metadata, "nco:publisher");
			tracker_sparql_builder_object_blank_open (metadata);
			tracker_sparql_builder_predicate (metadata, "a");
			tracker_sparql_builder_object (metadata, "nco:Contact");
			tracker_sparql_builder_predicate (metadata, "nco:fullname");
			tracker_sparql_builder_object_unvalidated (metadata, xd.publisher);
			tracker_sparql_builder_object_blank_close (metadata);
			g_free (xd.publisher);
		}

		if (xd.type) {
			tracker_sparql_builder_predicate (metadata, "dc:type");
			tracker_sparql_builder_object_unvalidated (metadata, xd.type);
			g_free (xd.type);
		}

		if (xd.format) {
			tracker_sparql_builder_predicate (metadata, "dc:format");
			tracker_sparql_builder_object_unvalidated (metadata, xd.format);
			g_free (xd.format);
		}

		if (xd.identifier) {
			tracker_sparql_builder_predicate (metadata, "dc:identifier");
			tracker_sparql_builder_object_unvalidated (metadata, xd.identifier);
			g_free (xd.identifier);
		}

		if (xd.source) {
			tracker_sparql_builder_predicate (metadata, "dc:source");
			tracker_sparql_builder_object_unvalidated (metadata, xd.source);
			g_free (xd.source);
		}

		if (xd.language) {
			tracker_sparql_builder_predicate (metadata, "dc:language");
			tracker_sparql_builder_object_unvalidated (metadata, xd.language);
			g_free (xd.language);
		}

		if (xd.relation) {
			tracker_sparql_builder_predicate (metadata, "dc:relation");
			tracker_sparql_builder_object_unvalidated (metadata, xd.relation);
			g_free (xd.relation);
		}

		if (xd.coverage) {
			tracker_sparql_builder_predicate (metadata, "dc:coverage");
			tracker_sparql_builder_object_unvalidated (metadata, xd.coverage);
			g_free (xd.coverage);
		}

		if (xd.license) {
			tracker_sparql_builder_predicate (metadata, "nie:license");
			tracker_sparql_builder_object_unvalidated (metadata, xd.license);
			g_free (xd.license);
		}

		if (xd.make || xd.model) {
			gchar *camera;

			if ((xd.make == NULL || xd.model == NULL) ||
			    (xd.make && xd.model && strstr (xd.model, xd.make) == NULL)) {
				camera = tracker_merge (" ", 2, xd.make, xd.model);
			} else {
				camera = g_strdup (xd.model);
				g_free (xd.model);
				g_free (xd.make);
			}

			tracker_sparql_builder_predicate (metadata, "nmm:camera");
			tracker_sparql_builder_object_unvalidated (metadata, camera);
			g_free (camera);
		}

		if (xd.orientation) {
			tracker_sparql_builder_predicate (metadata, "nfo:orientation");
			tracker_sparql_builder_object (metadata, xd.orientation);
			g_free (xd.orientation);
		}

		if (xd.rights) {
			tracker_sparql_builder_predicate (metadata, "nie:copyright");
			tracker_sparql_builder_object_unvalidated (metadata, xd.rights);
			g_free (xd.rights);
		}

		if (xd.white_balance) {
			tracker_sparql_builder_predicate (metadata, "nmm:whiteBalance");
			tracker_sparql_builder_object (metadata, xd.white_balance);
			g_free (xd.white_balance);
		}

		if (xd.fnumber) {
			gdouble value;

			value = g_strtod (xd.fnumber, NULL);
			tracker_sparql_builder_predicate (metadata, "nmm:fnumber");
			tracker_sparql_builder_object_double (metadata, value);
			g_free (xd.fnumber);
		}

		if (xd.flash) {
			tracker_sparql_builder_predicate (metadata, "nmm:flash");
			tracker_sparql_builder_object (metadata, xd.flash);
			g_free (xd.flash);
		}

		if (xd.focal_length) {
			gdouble value;

			value = g_strtod (xd.focal_length, NULL);
			tracker_sparql_builder_predicate (metadata, "nmm:focalLength");
			tracker_sparql_builder_object_double (metadata, value);
			g_free (xd.focal_length);
		}

		/* Question: Shouldn't xd.Artist be merged with md.author instead? */

		if (xd.artist || xd.contributor) {
			gchar *artist;

			artist = tracker_coalesce (2, xd.artist, xd.contributor);
			tracker_sparql_builder_predicate (metadata, "nco:contributor");
			tracker_sparql_builder_object_blank_open (metadata);
			tracker_sparql_builder_predicate (metadata, "a");
			tracker_sparql_builder_object (metadata, "nco:Contact");
			tracker_sparql_builder_predicate (metadata, "nco:fullname");
			tracker_sparql_builder_object_unvalidated (metadata, artist);
			tracker_sparql_builder_object_blank_close (metadata);
			g_free (artist);
		}

		if (xd.exposure_time) {
			gdouble value;

			value = g_strtod (xd.exposure_time, NULL);
			tracker_sparql_builder_predicate (metadata, "nmm:exposureTime");
			tracker_sparql_builder_object_double (metadata, value);
			g_free (xd.exposure_time);
		}

		if (xd.iso_speed_ratings) {
			gdouble value;

			value = g_strtod (xd.iso_speed_ratings, NULL);
			tracker_sparql_builder_predicate (metadata, "nmm:isoSpeed");
			tracker_sparql_builder_object_double (metadata, value);
			g_free (xd.iso_speed_ratings);
		}

		if (xd.description) {
			tracker_sparql_builder_predicate (metadata, "nie:description");
			tracker_sparql_builder_object_unvalidated (metadata, xd.description);
			g_free (xd.description);
		}

		if (xd.metering_mode) {
			tracker_sparql_builder_predicate (metadata, "nmm:meteringMode");
			tracker_sparql_builder_object (metadata, xd.metering_mode);
			g_free (xd.metering_mode);
		}

		if (xd.address || xd.country || xd.city) {
			tracker_sparql_builder_predicate (metadata, "mlo:location");
	
			tracker_sparql_builder_object_blank_open (metadata);
			tracker_sparql_builder_predicate (metadata, "a");
			tracker_sparql_builder_object (metadata, "mlo:GeoPoint");
	
			if (xd.address) {
				tracker_sparql_builder_predicate (metadata, "mlo:address");
				tracker_sparql_builder_object_unvalidated (metadata, xd.address);
				g_free (xd.address);
			}
	
			if (xd.state) {
				tracker_sparql_builder_predicate (metadata, "mlo:state");
				tracker_sparql_builder_object_unvalidated (metadata, xd.state);
				g_free (xd.state);
			}
	
			if (xd.city) {
				tracker_sparql_builder_predicate (metadata, "mlo:city");
				tracker_sparql_builder_object_unvalidated (metadata, xd.city);
				g_free (xd.city);
			}
	
			if (xd.country) {
				tracker_sparql_builder_predicate (metadata, "mlo:country");
				tracker_sparql_builder_object_unvalidated (metadata, xd.country);
				g_free (xd.country);
			}
		
			tracker_sparql_builder_object_blank_close (metadata);
		}

		/* PDF keywords aren't used ATM */
		g_free (pd.keywords);
	} else {
		/* So if we are here we have NO XMP data and we just
		 * write what we know from Poppler.
		 */
		write_pdf_data (pd, metadata);
	}

	tracker_sparql_builder_predicate (metadata, "nfo:pageCount");
	tracker_sparql_builder_object_int64 (metadata, poppler_document_get_n_pages (document));

	fts_config = tracker_main_get_fts_config ();
	n_words = tracker_fts_config_get_max_words_to_index (fts_config);
	content = extract_content (document, n_words);

	if (content) {
		tracker_sparql_builder_predicate (metadata, "nie:plainTextContent");
		tracker_sparql_builder_object_unvalidated (metadata, content);
		g_free (content);
	}

        read_outline (document, metadata);

	g_object_unref (document);
}

TrackerExtractData *
tracker_extract_get_data (void)
{
	return data;
}
