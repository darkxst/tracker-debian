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
 *
 * Author: Philip Van Hoof <philip@codeminded.be>
 */

#ifndef __TRACKER_STORE_STORE_H__
#define __TRACKER_STORE_STORE_H__

#include <stdio.h>

#include <libtracker-common/tracker-common.h>
#include <libtracker-db/tracker-db-interface.h>

G_BEGIN_DECLS

typedef void (* TrackerStoreSparqlUpdateCallback) (GError          *error,
                                                   gpointer         user_data);
typedef void (* TrackerStoreCommitCallback)       (gpointer         user_data);
typedef void (* TrackerStoreTurtleCallback)       (GError          *error,
                                                   gpointer         user_data);

void         tracker_store_init                   (void);
void         tracker_store_shutdown               (void);
void         tracker_store_queue_commit           (TrackerStoreCommitCallback callback,
                                                   const gchar   *client_id,
                                                   gpointer       user_data,
                                                   GDestroyNotify destroy);
void         tracker_store_queue_sparql_update    (const gchar   *sparql,
                                                   TrackerStoreSparqlUpdateCallback callback,
                                                   const gchar   *client_id,
                                                   gpointer       user_data,
                                                   GDestroyNotify destroy);
void         tracker_store_queue_turtle_import    (GFile         *file,
                                                   TrackerStoreTurtleCallback callback,
                                                   gpointer       user_data,
                                                   GDestroyNotify destroy);
void         tracker_store_sparql_update          (const gchar   *sparql,
                                                   GError       **error);
GPtrArray *  tracker_store_sparql_update_blank    (const gchar   *sparql,
                                                   GError       **error);
TrackerDBResultSet*
tracker_store_sparql_query           (const gchar   *sparql,
                                      GError       **error);

guint        tracker_store_get_queue_size         (void);

void         tracker_store_unreg_batches          (const gchar   *client_id);

G_END_DECLS

#endif /* __TRACKER_STORE_STORE_H__ */
