/*
 * Copyright (C) 2009, Nokia <ivan.frade@nokia.com>
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

#include <libtracker-common/tracker-file-utils.h>

#include "tracker-writeback-file.h"

static gboolean tracker_writeback_file_update_metadata (TrackerWriteback        *writeback,
                                                        GPtrArray               *values,
                                                        TrackerSparqlConnection *connection);

G_DEFINE_ABSTRACT_TYPE (TrackerWritebackFile, tracker_writeback_file, TRACKER_TYPE_WRITEBACK)

static void
tracker_writeback_file_class_init (TrackerWritebackFileClass *klass)
{
	TrackerWritebackClass *writeback_class = TRACKER_WRITEBACK_CLASS (klass);

	writeback_class->update_metadata = tracker_writeback_file_update_metadata;
}

static void
tracker_writeback_file_init (TrackerWritebackFile *writeback_file)
{
}

static gboolean
file_unlock_cb (gpointer user_data)
{
	GFile *file;
	gchar *path;

	file = user_data;
	path = g_file_get_path (file);
	g_message ("Unlocking file '%s'", path);
	g_free (path);

	tracker_file_unlock (file);
	g_object_unref (file);

	return FALSE;
}

static GFile *
get_tmp_file (GFile *file)
{
	GFile *tmp_file, *parent;
	gchar *tmp_name, *name;

	/* Create a temporary, hidden file
	 * within the same directory */
	parent = g_file_get_parent (file);
	name = g_file_get_basename (file);

	tmp_name = g_strdup_printf ("._tracker_%s", name);
	tmp_file = g_file_get_child (parent, tmp_name);

	g_object_unref (parent);
	g_free (tmp_name);
	g_free (name);

	return tmp_file;
}

static gboolean
tracker_writeback_file_update_metadata (TrackerWriteback        *writeback,
                                        GPtrArray               *values,
                                        TrackerSparqlConnection *connection)
{
	TrackerWritebackFileClass *writeback_file_class;
	gboolean retval;
	GFile *file, *tmp_file;
	GFileInfo *file_info;
	const gchar *urls[2] = { NULL, NULL };
	GStrv row;
	const gchar * const *content_types;
	const gchar *mime_type;
	guint n;

	writeback_file_class = TRACKER_WRITEBACK_FILE_GET_CLASS (writeback);

	if (!writeback_file_class->update_file_metadata) {
		g_critical ("%s doesn't implement update_file_metadata()",
		            G_OBJECT_TYPE_NAME (writeback));
		return FALSE;
	}

	if (!writeback_file_class->content_types) {
		g_critical ("%s doesn't implement content_types()",
		            G_OBJECT_TYPE_NAME (writeback));
		return FALSE;
	}

	/* Get the file from the row */
	row = g_ptr_array_index (values, 0);
	file = g_file_new_for_uri (row[0]);

	file_info = g_file_query_info (file,
	                               G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
	                               G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
	                               NULL, NULL);

	if (!file_info) {
		g_object_unref (file);
		return FALSE;
	}

	/* Copy to a temporary file so we can perform an atomic write on move */
	tmp_file = get_tmp_file (file);
	if (!g_file_copy (file, tmp_file, 0,
			  NULL, NULL, NULL, NULL)) {
		g_object_unref (file);
		g_object_unref (tmp_file);
		return FALSE;
	}

	mime_type = g_file_info_get_content_type (file_info);
	content_types = (writeback_file_class->content_types) (TRACKER_WRITEBACK_FILE (writeback));

	retval = FALSE;

	for (n = 0; content_types[n] != NULL; n++) {
		if (g_strcmp0 (mime_type, content_types[n]) == 0) {
			retval = TRUE;
			break;
		}
	}

	g_object_unref (file_info);

	if (retval) {
		g_message ("Locking file '%s' in order to write metadata", row[0]);

		tracker_file_lock (file);

		urls[0] = row[0];

		tracker_miner_manager_ignore_next_update (tracker_writeback_get_miner_manager (),
		                                          "org.freedesktop.Tracker1.Miner.Files",
		                                          urls);

		/* A note on IgnoreNextUpdate + Writeback. Consider this situation
		 * I see with an application recording a video:
		 *  - Application creates a resource for a video in the store and
		 *    sets slo:location
		 *  - Application starts writting the new video file.
		 *  - Store tells writeback to write the new slo:location in the file
		 *  - Writeback reaches this exact function and sends IgnoreNextUpdate,
		 *    then tries to update metadata.
		 *  - Miner-fs gets the IgnoreNextUpdate (sent by the line above).
		 *  - Application is still recording the video, which gets translated
		 *    into an original CREATED event plus multiple UPDATE events which
		 *    are being merged at tracker-monitor level, still not notified to
		 *    TrackerMinerFS.
		 *  - TrackerWriteback tries to updte file metadata (line below) but cannot
		 *    do it yet as application is still updating the file, thus, the real
		 *    metadata update gets delayed until the application ends writing
		 *    the video.
		 *  - Application ends writing the video.
		 *  - Now TrackerWriteback really updates the file. This happened N seconds
		 *    after we sent the IgnoreNextUpdate, being N the length of the video...
		 *  - TrackerMonitor sends the merged CREATED event to TrackerMinerFS,
		 *    detects the IgnoreNextUpdate request and in this case we ignore the
		 *    IgnoreNextUpdate request as this is a CREATED event.
		 *
		 * Need to review the whole writeback+IgnoreNextUpdate mechanism to cope
		 * with situations like the one above.
		 */

		retval = (writeback_file_class->update_file_metadata) (TRACKER_WRITEBACK_FILE (writeback),
		                                                       tmp_file, values, connection);

		/*
		 * This timeout value was 3s before, which could have been in
		 * order to have miner-fs skip the updates we just made, something
		 * along the purpose of IgnoreNextUpdate.
		 *
		 * But this is a problem when the event being ignored is a CREATED
		 * event. This is, tracker-writeback modifies a file that was just
		 * created. If we ignore this in the miner-fs, it will never index
		 * it, and that is not good. As there is already the
		 * IgnoreNextUpdate mechanism in place, I'm moving this timeout
		 * value to 1s. So, once writeback has written the file, only 1s
		 * after will unlock it. This synchronizes well with the 2s timeout
		 * in the miner-fs between detecting the file update and the actual
		 * processing.
		 */
		g_timeout_add_seconds (1, file_unlock_cb, g_object_ref (file));
	}

	/* Move back the modified file to the original location */
	g_file_move (tmp_file, file,
		     G_FILE_COPY_OVERWRITE,
		     NULL, NULL, NULL, NULL);

	g_object_unref (tmp_file);
	g_object_unref (file);

	return retval;
}
