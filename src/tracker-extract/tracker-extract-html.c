/*
 * Copyright (C) 2007, Jason Kivlighn <jkivlighn@gmail.com>
 * Copyright (C) 2008-2009, Nokia <ivan.frade@nokia.com>
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

#include <libxml/HTMLparser.h>

#include <libtracker-extract/tracker-extract.h>

typedef enum {
	READ_TITLE,
} tag_type;

typedef struct {
	TrackerSparqlBuilder *metadata;
	tag_type current;
	const gchar *uri;
} parser_data;

static void extract_html (const gchar          *filename,
                          TrackerSparqlBuilder *preupdate,
                          TrackerSparqlBuilder *metadata);

static TrackerExtractData data[] = {
	{ "text/html",             extract_html },
	{ "application/xhtml+xml", extract_html },
	{ NULL, NULL }
};

static gboolean
has_attribute (const gchar **attrs,
               const gchar  *attr,
               const gchar  *val)
{
	gint i;

	if (!attrs || !attr || !val) {
		return FALSE;
	}

	for (i = 0; attrs[i] && attrs[i + 1]; i += 2) {
		if (g_ascii_strcasecmp (attrs[i], attr) == 0) {
			if (g_ascii_strcasecmp (attrs[i + 1], val) == 0) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

static const xmlChar *
lookup_attribute (const gchar **attrs,
                  const gchar  *attr)
{
	gint i;

	if (!attrs || !attr) {
		return NULL;
	}

	for (i = 0; attrs[i] && attrs[i + 1]; i += 2) {
		if (g_ascii_strcasecmp (attrs[i], attr) == 0) {
			return attrs[i + 1];
		}
	}

	return NULL;
}

static void
parser_start_element (void           *data,
                      const xmlChar  *name_,
                      const xmlChar **attrs_)
{
	parser_data *pd = data;
	const gchar *name = (const gchar*) name_;
	const gchar **attrs = (const gchar**) attrs_;

	if (!pd || !name) {
		return;
	}

	/* Look for RDFa triple describing the license */
	if (g_ascii_strcasecmp (name, "a") == 0) {
		/* This tag is a license.  Ignore, however, if it is
		 * referring to another document.
		 */
		if (has_attribute (attrs, "rel", "license") &&
		    has_attribute (attrs, "about", NULL) == FALSE) {
			const xmlChar *href;

			href = lookup_attribute (attrs, "href");

			if (href) {
				tracker_sparql_builder_predicate (pd->metadata, "nie:license");
				tracker_sparql_builder_object_unvalidated (pd->metadata, href);
			}
		}
	} else if (g_ascii_strcasecmp (name, "title") == 0) {
		pd->current = READ_TITLE;
	} else if (g_ascii_strcasecmp (name, "meta") == 0) {
		if (has_attribute (attrs, "name", "author")) {
			const xmlChar *author;

			author = lookup_attribute (attrs, "content");

			if (author) {
				tracker_sparql_builder_predicate (pd->metadata, "nco:creator");
				tracker_sparql_builder_object_blank_open (pd->metadata);
				tracker_sparql_builder_predicate (pd->metadata, "a");
				tracker_sparql_builder_object (pd->metadata, "nco:Contact");
				tracker_sparql_builder_predicate (pd->metadata, "nco:fullname");
				tracker_sparql_builder_object_unvalidated (pd->metadata, author);
				tracker_sparql_builder_object_blank_close (pd->metadata);
			}
		}

		if (has_attribute (attrs, "name", "description")) {
			const xmlChar *desc;

			desc = lookup_attribute (attrs,"content");

			if (desc) {
				tracker_sparql_builder_predicate (pd->metadata, "nie:description");
				tracker_sparql_builder_object_unvalidated (pd->metadata, desc);
			}
		}

		if (has_attribute (attrs, "name", "keywords")) {
			const xmlChar* content = lookup_attribute (attrs, "content");

			if (content) {
				gchar **keywords;
				gint i;

				keywords = g_strsplit (content, ",", -1);
				if (keywords) {
					for (i = 0; keywords[i] != NULL; i++) {
						if (!keywords[i] || keywords[i] == '\0') {
							continue;
						}

						tracker_sparql_builder_predicate (pd->metadata, "nie:keyword");
						tracker_sparql_builder_object_unvalidated (pd->metadata, g_strstrip (keywords[i]));
					}

					g_strfreev (keywords);
				}
			}
		}
	}
}

static void
parser_characters (void          *data,
                   const xmlChar *ch,
                   int            len)
{
	parser_data *pd = data;

	switch (pd->current) {
	case READ_TITLE:
		tracker_sparql_builder_predicate (pd->metadata, "nie:title");
		tracker_sparql_builder_object_unvalidated (pd->metadata, ch);
		break;
	default:
		break;
	}

	pd->current = -1;
}

static void
extract_html (const gchar          *uri,
              TrackerSparqlBuilder *preupdate,
              TrackerSparqlBuilder *metadata)
{
	htmlDocPtr doc;
	parser_data pd;
	gchar *filename;
	xmlSAXHandler handler = {
		NULL, /* internalSubset */
		NULL, /* isStandalone */
		NULL, /* hasInternalSubset */
		NULL, /* hasExternalSubset */
		NULL, /* resolveEntity */
		NULL, /* getEntity */
		NULL, /* entityDecl */
		NULL, /* notationDecl */
		NULL, /* attributeDecl */
		NULL, /* elementDecl */
		NULL, /* unparsedEntityDecl */
		NULL, /* setDocumentLocator */
		NULL, /* startDocument */
		NULL, /* endDocument */
		parser_start_element, /* startElement */
		NULL, /* endElement */
		NULL, /* reference */
		parser_characters, /* characters */
		NULL, /* ignorableWhitespace */
		NULL, /* processingInstruction */
		NULL, /* comment */
		NULL, /* xmlParserWarning */
		NULL, /* xmlParserError */
		NULL, /* xmlParserError */
		NULL, /* getParameterEntity */
		NULL, /* cdataBlock */
		NULL, /* externalSubset */
		1,    /* initialized */
		NULL, /* private */
		NULL, /* startElementNsSAX2Func */
		NULL, /* endElementNsSAX2Func */
		NULL  /* xmlStructuredErrorFunc */
	};

	tracker_sparql_builder_predicate (metadata, "a");
	tracker_sparql_builder_object (metadata, "nfo:Document");

	pd.metadata = metadata;
	pd.current = -1;
	pd.uri = uri;

	filename = g_filename_from_uri (uri, NULL, NULL);
	doc = htmlSAXParseFile (filename, NULL, &handler, &pd);
	g_free (filename);

	if (doc) {
		xmlFreeDoc (doc);
	}
}

TrackerExtractData *
tracker_extract_get_data (void)
{
	return data;
}
