/*
 * Copyright (C) 2011, Nokia <ivan.frade@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 * Author: Carlos Garnacho <carlos@lanedo.com>
 */

#include "tracker-extract-info.h"

struct _TrackerExtractInfo
{
	TrackerSparqlBuilder *preupdate;
	TrackerSparqlBuilder *metadata;
	gchar *where_clause;

	GFile *file;
	gchar *mimetype;
	gchar *graph;

	gint ref_count;
};

G_DEFINE_BOXED_TYPE (TrackerExtractInfo, tracker_extract_info,
                     tracker_extract_info_ref, tracker_extract_info_unref)

/**
 * tracker_extract_info_new:
 * @file: a #GFile
 * @mimetype: mimetype for @file
 * @graph: SPARQL graph used for inserting data
 *
 * Returns a newly created #TrackerExtractInfo
 *
 * Returns: (transfer full): (boxed): A newly allocated #TrackerExtractInfo
 *
 * Since: 0.12
 **/
TrackerExtractInfo *
tracker_extract_info_new (GFile       *file,
                          const gchar *mimetype,
                          const gchar *graph)
{
	TrackerExtractInfo *info;

	g_return_val_if_fail (G_IS_FILE (file), NULL);

	info = g_slice_new0 (TrackerExtractInfo);
	info->file = g_object_ref (file);
	info->mimetype = g_strdup (mimetype);
	info->graph = g_strdup (graph);

	info->preupdate = tracker_sparql_builder_new_update ();
	info->metadata = tracker_sparql_builder_new_embedded_insert ();

	info->ref_count = 1;

	return info;
}

/**
 * tracker_extract_info_ref:
 * @info: a #TrackerExtractInfo
 *
 * Increases the reference count of @info
 *
 * Returns: A new reference to @info
 *
 * Since: 0.12
 **/
TrackerExtractInfo *
tracker_extract_info_ref (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	g_atomic_int_inc (&info->ref_count);

	return info;
}

/**
 * tracker_extract_info_unref:
 * @info: a #TrackerExtractInfo
 *
 * Decreases the reference count of @info, freeing all its associated resources.
 * if it reaches 0.
 *
 * Since: 0.12
 **/
void
tracker_extract_info_unref (TrackerExtractInfo *info)
{
	g_return_if_fail (info != NULL);

	if (g_atomic_int_dec_and_test (&info->ref_count)) {
		g_object_unref (info->file);
		g_free (info->mimetype);
		g_free (info->graph);

		g_object_unref (info->preupdate);
		g_object_unref (info->metadata);
		g_free (info->where_clause);

		g_slice_free (TrackerExtractInfo, info);
	}
}

/**
 * tracker_extract_info_get_file:
 * @info: a #TrackerExtractInfo
 *
 * Returns a #GFile pointing to the file being affected
 * by the metadata extraction represented by @info
 *
 * Returns: (transfer none): The file being inspected
 *
 * Since: 0.12
 **/
GFile *
tracker_extract_info_get_file (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->file;
}

/**
 * tracker_extract_info_get_mimetype:
 * @info: a #TrackerExtractInfo
 *
 * Returns the mimetype being used for the file
 * metadata extraction.
 *
 * Returns: (transfer none): the mimetype being used
 *          for extraction.
 *
 * Since: 0.12
 **/
const gchar *
tracker_extract_info_get_mimetype (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->mimetype;
}

/**
 * tracker_extract_info_get_graph:
 * @info: a #TrackerExtractInfo
 *
 * Returns the SPARQL graph that will be used when
 * inserting metadata.
 *
 * Returns: (transfer none): The SPARQL graph the extract
 *          operation belongs to.
 *
 * Since: 0.12
 **/
const gchar *
tracker_extract_info_get_graph (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->graph;
}

/**
 * tracker_extract_info_get_preupdate_builder:
 * @info: a #TrackerSparqlBuilder
 *
 * Returns a #TrackerSparqlBuilder containing any
 * separate updates that could apply to the file,
 * such as author/band information in audio files,
 * and so on.
 *
 * Returns: (transfer none): miscellaneous metadata
 *
 * Since: 0.12
 **/
TrackerSparqlBuilder *
tracker_extract_info_get_preupdate_builder (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->preupdate;
}

/**
 * tracker_extract_info_get_metadata_builder:
 * @info: a #TrackerExtractInfo
 *
 * Returns a #TrackerSparqlBuilder containing the
 * file metadata.
 *
 * Returns: (transfer none): the file metadata
 *
 * Since: 0.12
 **/
TrackerSparqlBuilder *
tracker_extract_info_get_metadata_builder (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->metadata;
}

/**
 * tracker_extract_info_get_where_clause:
 * @info: a #TrackerExtractInfo
 *
 * Returns the where clause that will apply to the
 * other metadata contained in @info.
 *
 * Returns: (transfer none): The where clause
 *
 * Since: 0.12
 **/
const gchar *
tracker_extract_info_get_where_clause (TrackerExtractInfo *info)
{
	g_return_val_if_fail (info != NULL, NULL);

	return info->where_clause;
}

/**
 * tracker_extract_info_set_where_clause:
 * @info: a #TrackerExtractInfo
 * @where: Where clause for the file update.
 *
 * Sets the where clause for the returned metadata.
 *
 * Since: 0.12
 **/
void
tracker_extract_info_set_where_clause (TrackerExtractInfo *info,
                                       const gchar        *where)
{
	g_return_if_fail (info != NULL);

	g_free (info->where_clause);
	info->where_clause = g_strdup (where);
}
