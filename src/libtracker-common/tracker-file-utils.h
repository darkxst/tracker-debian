/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2006, Mr Jamie McCracken (jamiemcc@gnome.org)
 * Copyright (C) 2008, Nokia
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

#ifndef __LIBTRACKER_COMMON_FILE_UTILS_H__
#define __LIBTRACKER_COMMON_FILE_UTILS_H__

#if !defined (__LIBTRACKER_COMMON_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "only <libtracker-common/tracker-common.h> must be included directly."
#endif

#include <stdio.h>
#include <glib.h>

FILE*    tracker_file_open                         (const gchar  *uri,
						    const gchar  *how,
						    gboolean      sequential);
void     tracker_file_close                        (FILE         *file,
						    gboolean      need_again_soon);
gboolean tracker_file_unlink                       (const gchar  *uri);
goffset  tracker_file_get_size                     (const gchar  *uri);
guint64  tracker_file_get_mtime                    (const gchar  *uri);
gchar *  tracker_file_get_mime_type                (const gchar  *uri);
void     tracker_file_get_path_and_name            (const gchar  *uri,
						    gchar       **path,
						    gchar       **name);
gboolean tracker_file_system_has_enough_space      (const gchar  *path,
						    gulong        required_bytes);
void     tracker_path_remove                       (const gchar  *uri);
gboolean tracker_path_is_in_path                   (const gchar  *path,
						    const gchar  *in_path);
void     tracker_path_hash_table_filter_duplicates (GHashTable   *roots);
GSList * tracker_path_list_filter_duplicates       (GSList       *roots,
						    const gchar  *basename_exception_prefix);
gchar *  tracker_path_evaluate_name                (const gchar  *uri);

gboolean tracker_env_check_xdg_dirs                (void);

#endif /* __LIBTRACKER_COMMON_FILE_UTILS_H__ */
