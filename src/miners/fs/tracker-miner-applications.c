/*
 * Copyright (C) 2008, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "config.h"

#include <libtracker-common/tracker-utils.h>
#include <libtracker-common/tracker-ontologies.h>

#include "tracker-miner-applications.h"

#define RDF_TYPE                     TRACKER_RDF_PREFIX "type"
#define NFO_PREFIX                   TRACKER_NFO_PREFIX
#define NIE_PREFIX                   TRACKER_NIE_PREFIX
#define MAEMO_PREFIX                 TRACKER_MAEMO_PREFIX

#define GROUP_DESKTOP_ENTRY          "Desktop Entry"

#define APPLICATION_DATASOURCE_URN   "urn:nepomuk:datasource:84f20000-1241-11de-8c30-0800200c9a66"
#define APPLET_DATASOURCE_URN        "urn:nepomuk:datasource:192bd060-1f9a-11de-8c30-0800200c9a66"
#define SOFTWARE_CATEGORY_URN_PREFIX "urn:software-category:"
#define THEME_ICON_URN_PREFIX        "urn:theme-icon:"

static void     miner_applications_finalize          (GObject              *object);
static void     miner_applications_constructed       (GObject              *object);

static gboolean miner_applications_check_file        (TrackerMinerFS       *fs,
                                                      GFile                *file);
static gboolean miner_applications_check_directory   (TrackerMinerFS       *fs,
                                                      GFile                *file);
static gboolean miner_applications_process_file      (TrackerMinerFS       *fs,
                                                      GFile                *file,
                                                      TrackerSparqlBuilder *sparql,
                                                      GCancellable         *cancellable);
static gboolean miner_applications_monitor_directory (TrackerMinerFS       *fs,
                                                      GFile                *file);

static GQuark miner_applications_error_quark = 0;

typedef struct ProcessApplicationData ProcessApplicationData;

struct ProcessApplicationData {
	TrackerMinerFS *miner;
	GFile *file;
	TrackerSparqlBuilder *sparql;
	GCancellable *cancellable;
	GKeyFile *key_file;
	gchar *type;
};

G_DEFINE_TYPE (TrackerMinerApplications, tracker_miner_applications, TRACKER_TYPE_MINER_FS)

static void
tracker_miner_applications_class_init (TrackerMinerApplicationsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	TrackerMinerFSClass *miner_fs_class = TRACKER_MINER_FS_CLASS (klass);

	object_class->finalize = miner_applications_finalize;
	object_class->constructed = miner_applications_constructed;

	miner_fs_class->check_file = miner_applications_check_file;
	miner_fs_class->check_directory = miner_applications_check_directory;
	miner_fs_class->monitor_directory = miner_applications_monitor_directory;
	miner_fs_class->process_file = miner_applications_process_file;

	miner_applications_error_quark = g_quark_from_static_string ("TrackerMinerApplications");
}

static void
tracker_miner_applications_init (TrackerMinerApplications *ma)
{
}

static void
miner_applications_finalize (GObject *object)
{
	G_OBJECT_CLASS (tracker_miner_applications_parent_class)->finalize (object);
}

static void
miner_applications_constructed (GObject *object)
{
	GFile *file;

	G_OBJECT_CLASS (tracker_miner_applications_parent_class)->constructed (object);

	file = g_file_new_for_path ("/usr/share/applications/");
	tracker_miner_fs_directory_add (TRACKER_MINER_FS (object), file, TRUE);
	g_object_unref (file);

	file = g_file_new_for_path ("/usr/share/desktop-directories/");
	tracker_miner_fs_directory_add (TRACKER_MINER_FS (object), file, TRUE);
	g_object_unref (file);

	/* FIXME: Check XDG_DATA_DIRS and also process applications in there */
}

static void
insert_data_from_desktop_file (TrackerSparqlBuilder *sparql,
                               const gchar          *subject,
                               const gchar          *metadata_key,
                               GKeyFile             *desktop_file,
                               const gchar          *key,
                               gboolean                      use_locale)
{
	gchar *str;

	if (use_locale) {
		str = g_key_file_get_locale_string (desktop_file, GROUP_DESKTOP_ENTRY, key, NULL, NULL);
	} else {
		str = g_key_file_get_string (desktop_file, GROUP_DESKTOP_ENTRY, key, NULL);
	}

	if (str) {
		tracker_sparql_builder_subject_iri (sparql, subject);
		tracker_sparql_builder_predicate_iri (sparql, metadata_key);
		tracker_sparql_builder_object_string (sparql, str);
		g_free (str);
	}
}

static gboolean
miner_applications_check_file (TrackerMinerFS *fs,
                               GFile          *file)
{
	gboolean retval = FALSE;
	gchar *basename;

	basename = g_file_get_basename (file);

	/* Check we're dealing with a desktop file */
	if (g_str_has_suffix (basename, ".desktop") ||
	    g_str_has_suffix (basename, ".directory")) {
		retval = TRUE;
	}

	g_free (basename);

	return retval;
}

static gboolean
miner_applications_check_directory (TrackerMinerFS *fs,
                                    GFile          *file)
{
	/* We want to inspect all the passed dirs and their children */
	return TRUE;
}

static gboolean
miner_applications_monitor_directory (TrackerMinerFS *fs,
                                      GFile          *file)
{
	/* We want to monitor all the passed dirs and their children */
	return TRUE;
}

static GKeyFile *
get_desktop_key_file (GFile   *file,
                      gchar  **type,
                      GError **error)
{
	GKeyFile *key_file;
	gchar *path;
	gchar *str;

	path = g_file_get_path (file);
	key_file = g_key_file_new ();

	if (!g_key_file_load_from_file (key_file, path, G_KEY_FILE_NONE, NULL)) {
		g_set_error (error, miner_applications_error_quark, 0, "Couldn't load desktop file:'%s'", path);
		g_key_file_free (key_file);
		g_free (path);
		return NULL;
	}

	if (g_key_file_get_boolean (key_file, GROUP_DESKTOP_ENTRY, "Hidden", NULL)) {
		g_set_error_literal (error, miner_applications_error_quark, 0, "Desktop file is 'hidden', not gathering metadata for it");
		g_key_file_free (key_file);
		g_free (path);
		return NULL;
	}

	str = g_key_file_get_string (key_file, GROUP_DESKTOP_ENTRY, "Type", NULL);

	if (G_UNLIKELY (!str)) {
		*type = NULL;

		g_set_error_literal (error, miner_applications_error_quark, 0, "Desktop file doesn't contain type");
		g_key_file_free (key_file);
		g_free (path);
		return NULL;
	} else {
		/* Sanitize type */
		*type = g_strstrip (str);
	}

	g_free (path);

	return key_file;
}

static gboolean
miner_applications_process_file_cb (gpointer user_data)
{
	ProcessApplicationData *data = user_data;
	TrackerSparqlBuilder *sparql;
	GKeyFile *key_file;
	gchar *path, *type, *filename, *name, *uri = NULL;
	GFileInfo *file_info;
	GStrv cats;
	gsize cats_len;
	gboolean is_software = TRUE;

	sparql = data->sparql;
	key_file = data->key_file;
	type = data->type;

	path = g_file_get_path (data->file);

	/* Try to get the categories with locale, then without if that fails */
	cats = g_key_file_get_locale_string_list (key_file, GROUP_DESKTOP_ENTRY, "Categories", NULL, &cats_len, NULL);

	if (!cats)
		cats = g_key_file_get_string_list (key_file, GROUP_DESKTOP_ENTRY, "Categories", &cats_len, NULL);

	/* NOTE: We sanitize categories later on when iterating them */

	/* Try to get the name with locale, then without if that fails */
	name = g_key_file_get_locale_string (key_file, GROUP_DESKTOP_ENTRY, "Name", NULL, NULL);

	if (!name) {
		name = g_key_file_get_string (key_file, GROUP_DESKTOP_ENTRY, "Name", NULL);
	}

	/* Sanitize name */
	if (name) {
		g_strstrip (name);
	}

	if (name && g_ascii_strcasecmp (type, "Directory") == 0) {
		gchar *canonical_uri = tracker_uri_printf_escaped (SOFTWARE_CATEGORY_URN_PREFIX "%s", path);
		gchar *icon = g_key_file_get_string (key_file, GROUP_DESKTOP_ENTRY, "Icon", NULL);

		uri = canonical_uri;
		tracker_sparql_builder_insert_open (sparql, uri);
		tracker_sparql_builder_subject_iri (sparql, uri);

		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "nfo:SoftwareCategory");

		if (icon) {
			gchar *icon_uri;

			/* Sanitize icon */
			g_strstrip (icon);

			icon_uri = g_strdup_printf (THEME_ICON_URN_PREFIX "%s", icon);

			tracker_sparql_builder_subject_iri (sparql, icon_uri);
			tracker_sparql_builder_predicate (sparql, "a");
			tracker_sparql_builder_object (sparql, "nfo:Image");

			tracker_sparql_builder_subject_iri (sparql, uri);
			tracker_sparql_builder_predicate (sparql, "nfo:softwareCategoryIcon");
			tracker_sparql_builder_object_iri (sparql, icon_uri);

			g_free (icon_uri);
			g_free (icon);
		}

		is_software = FALSE;

	} else if (name && g_ascii_strcasecmp (type, "Application") == 0) {
		uri = g_file_get_uri (data->file);
		tracker_sparql_builder_insert_open (sparql, uri);

		tracker_sparql_builder_subject_iri (sparql, APPLICATION_DATASOURCE_URN);
		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "nie:DataSource");

		tracker_sparql_builder_subject_iri (sparql, uri);

		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "nfo:SoftwareApplication");
		tracker_sparql_builder_object (sparql, "nie:DataObject");

		tracker_sparql_builder_predicate (sparql, "nie:dataSource");
		tracker_sparql_builder_object_iri (sparql, APPLICATION_DATASOURCE_URN);

		/* This matches SomeApplet as Type= */
	} else if (name && g_str_has_suffix (type, "Applet")) {
		/* The URI of the InformationElement should be a UUID URN */
		uri = g_file_get_uri (data->file);
		tracker_sparql_builder_insert_open (sparql, uri);

		tracker_sparql_builder_subject_iri (sparql, APPLET_DATASOURCE_URN);
		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "nie:DataSource");

		/* TODO This is atm specific for Maemo */
		tracker_sparql_builder_subject_iri (sparql, uri);

		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "maemo:SoftwareApplet");

		tracker_sparql_builder_predicate (sparql, "nie:dataSource");
		tracker_sparql_builder_object_iri (sparql, APPLET_DATASOURCE_URN);
	}

	if (sparql && uri) {
		gchar *desktop_file_uri;

		tracker_sparql_builder_predicate (sparql, "a");

		if (is_software) {
			tracker_sparql_builder_object (sparql, "nfo:Executable");
		}

		tracker_sparql_builder_object (sparql, "nfo:FileDataObject");
		tracker_sparql_builder_object (sparql, "nie:DataObject");

		/* Apparently this gets added by the file-module ATM
		   tracker_sparql_builder_predicate (sparql, "tracker:available");
		   tracker_sparql_builder_object_boolean (sparql, TRUE); */

		tracker_sparql_builder_predicate (sparql, "nie:title");
		tracker_sparql_builder_object_string (sparql, name);

		if (is_software) {
			gchar *icon;

			insert_data_from_desktop_file (sparql, uri, NIE_PREFIX "comment", key_file, "Comment", TRUE);
			insert_data_from_desktop_file (sparql, uri, NFO_PREFIX "softwareCmdLine", key_file, "Exec", TRUE);

			icon = g_key_file_get_string (key_file, GROUP_DESKTOP_ENTRY, "Icon", NULL);

			if (icon) {
				gchar *icon_uri;

				/* Sanitize icon */
				g_strstrip (icon);

				icon_uri = g_strdup_printf (THEME_ICON_URN_PREFIX "%s", icon);

				tracker_sparql_builder_subject_iri (sparql, icon_uri);
				tracker_sparql_builder_predicate (sparql, "a");
				tracker_sparql_builder_object (sparql, "nfo:Image");

				tracker_sparql_builder_subject_iri (sparql, uri);
				tracker_sparql_builder_predicate (sparql, "nfo:softwareIcon");
				tracker_sparql_builder_object_iri (sparql, icon_uri);

				g_free (icon_uri);
				g_free (icon);
			}
		}

		if (cats) {
			gsize i;

			for (i = 0 ; cats[i] && i < cats_len ; i++) {
				gchar *cat_uri;
				gchar *cat;

				cat = cats[i];

				if (!cat) {
					continue;
				}

				/* Sanitize category */
				g_strstrip (cat);

				cat_uri = tracker_uri_printf_escaped (SOFTWARE_CATEGORY_URN_PREFIX "%s", cat);

				/* There are also .desktop
				 * files that describe these categories, but we can handle
				 * preemptively creating them if we visit a app .desktop
				 * file that mentions one that we don't yet know about */

				tracker_sparql_builder_subject_iri (sparql, cat_uri);
				tracker_sparql_builder_predicate (sparql, "a");
				tracker_sparql_builder_object (sparql, "nfo:SoftwareCategory");

				tracker_sparql_builder_predicate (sparql, "nie:title");
				tracker_sparql_builder_object_string (sparql, cat);

				tracker_sparql_builder_subject_iri (sparql, uri);
				tracker_sparql_builder_predicate (sparql, "nie:isLogicalPartOf");
				tracker_sparql_builder_object_iri (sparql, cat_uri);

				g_free (cat_uri);
			}
		}

		tracker_sparql_builder_subject_iri (sparql, uri);
		tracker_sparql_builder_predicate (sparql, "nie:dataSource");
		tracker_sparql_builder_object_iri (sparql, APPLICATION_DATASOURCE_URN);

		filename = g_filename_display_basename (path);
		tracker_sparql_builder_predicate (sparql, "nfo:fileName");
		tracker_sparql_builder_object_string (sparql, filename);
		g_free (filename);

		desktop_file_uri = g_file_get_uri (data->file);
		tracker_sparql_builder_subject_iri (sparql, desktop_file_uri);
		tracker_sparql_builder_predicate (sparql, "a");
		tracker_sparql_builder_object (sparql, "nfo:FileDataObject");
		tracker_sparql_builder_object (sparql, "nie:DataObject");

		/* The URL of the DataObject */
		tracker_sparql_builder_predicate (sparql, "nie:url");
		tracker_sparql_builder_object_string (sparql, uri);

		/* Laying the link between the IE and the DO */
		tracker_sparql_builder_subject_iri (sparql, uri);
		tracker_sparql_builder_predicate (sparql, "nie:isStoredAs");
		tracker_sparql_builder_object_iri (sparql, desktop_file_uri);


		g_free (desktop_file_uri);
	}

	file_info = g_file_query_info (data->file,
	                               G_FILE_ATTRIBUTE_TIME_MODIFIED,
	                               G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
	                               NULL, NULL);

	if (file_info) {
		guint64 time;

		time = g_file_info_get_attribute_uint64 (file_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);
		tracker_sparql_builder_predicate (sparql, "nfo:fileLastModified");
		tracker_sparql_builder_object_date (sparql, (time_t *) &time);

		g_object_unref (file_info);
	}

	tracker_sparql_builder_insert_close (sparql);

	/* Notify about success */
	tracker_miner_fs_file_notify (data->miner, data->file, NULL);

	g_strfreev (cats);

	g_free (uri);
	g_free (path);
	g_free (name);

	return FALSE;
}

static void
process_application_data_free (ProcessApplicationData *data)
{
	g_object_unref (data->miner);
	g_object_unref (data->file);
	g_object_unref (data->sparql);
	g_object_unref (data->cancellable);
	g_key_file_free (data->key_file);
	g_free (data->type);
	g_slice_free (ProcessApplicationData, data);
}

static gboolean
miner_applications_process_file (TrackerMinerFS       *fs,
                                 GFile                *file,
                                 TrackerSparqlBuilder *sparql,
                                 GCancellable         *cancellable)
{
	ProcessApplicationData *data;
	GKeyFile *key_file;
	gchar *type;

	key_file = get_desktop_key_file (file, &type, NULL);

	if (!key_file) {
		return FALSE;
	}

	data = g_slice_new0 (ProcessApplicationData);
	data->miner = g_object_ref (fs);
	data->sparql = g_object_ref (sparql);
	data->file = g_object_ref (file);
	data->cancellable = g_object_ref (cancellable);
	data->key_file = key_file;
	data->type = type;

	g_idle_add_full (G_PRIORITY_DEFAULT_IDLE,
	                 miner_applications_process_file_cb,
	                 data,
	                 (GDestroyNotify) process_application_data_free);

	return TRUE;
}

TrackerMiner *
tracker_miner_applications_new (void)
{
	return g_object_new (TRACKER_TYPE_MINER_APPLICATIONS,
	                     "name", "Applications",
	                     NULL);
}
