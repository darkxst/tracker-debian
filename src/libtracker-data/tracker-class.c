/*
 * Copyright (C) 2006, Jamie McCracken <jamiemcc@gnome.org>
 * Copyright (C) 2008, Nokia <ivan.frade@nokia.com>
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>

#include <glib.h>

#include "tracker-class.h"
#include "tracker-namespace.h"
#include "tracker-ontologies.h"

#define GET_PRIV(obj) (((TrackerClass*) obj)->priv)
#define TRACKER_CLASS_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TRACKER_TYPE_CLASS, TrackerClassPrivate))

struct _TrackerClassPrivate {
	gchar *uri;
	gchar *name;
	gint count;
	gint id;
	gboolean is_new;
	gboolean db_schema_changed;
	gboolean notify;

	GArray *super_classes;
};

static void class_finalize     (GObject      *object);

G_DEFINE_TYPE (TrackerClass, tracker_class, G_TYPE_OBJECT);

static void
tracker_class_class_init (TrackerClassClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize     = class_finalize;

	g_type_class_add_private (object_class, sizeof (TrackerClassPrivate));
}

static void
tracker_class_init (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	priv = TRACKER_CLASS_GET_PRIVATE (service);

	priv->id = 0;
	priv->super_classes = g_array_new (TRUE, TRUE, sizeof (TrackerClass *));

	/* Make GET_PRIV working */
	service->priv = priv;
}

static void
class_finalize (GObject *object)
{
	TrackerClassPrivate *priv;

	priv = GET_PRIV (object);

	g_free (priv->uri);
	g_free (priv->name);

	g_array_free (priv->super_classes, TRUE);

	(G_OBJECT_CLASS (tracker_class_parent_class)->finalize) (object);
}

TrackerClass *
tracker_class_new (void)
{
	TrackerClass *service;

	service = g_object_new (TRACKER_TYPE_CLASS, NULL);

	return service;
}

const gchar *
tracker_class_get_uri (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), NULL);

	priv = GET_PRIV (service);

	return priv->uri;
}

const gchar *
tracker_class_get_name (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), NULL);

	priv = GET_PRIV (service);

	return priv->name;
}

gint
tracker_class_get_count (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), 0);

	priv = GET_PRIV (service);

	return priv->count;
}

gint
tracker_class_get_id (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), 0);

	priv = GET_PRIV (service);

	return priv->id;
}

TrackerClass **
tracker_class_get_super_classes (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), NULL);

	priv = GET_PRIV (service);

	return (TrackerClass **) priv->super_classes->data;
}

gboolean
tracker_class_get_is_new (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), FALSE);

	priv = GET_PRIV (service);

	return priv->is_new;
}

gboolean
tracker_class_get_notify (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), FALSE);

	priv = GET_PRIV (service);

	return priv->notify;
}

gboolean
tracker_class_get_db_schema_changed (TrackerClass *service)
{
	TrackerClassPrivate *priv;

	g_return_val_if_fail (TRACKER_IS_CLASS (service), FALSE);

	priv = GET_PRIV (service);

	return priv->db_schema_changed;
}

void
tracker_class_set_uri (TrackerClass *service,
                       const gchar  *value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	g_free (priv->uri);
	g_free (priv->name);
	priv->uri = NULL;
	priv->name = NULL;

	if (value) {
		gchar *namespace_uri, *hash;
		TrackerNamespace *namespace;

		priv->uri = g_strdup (value);

		hash = strrchr (priv->uri, '#');
		if (hash == NULL) {
			/* support ontologies whose namespace uri does not end in a hash, e.g. dc */
			hash = strrchr (priv->uri, '/');
		}
		if (hash == NULL) {
			g_critical ("Unknown namespace of class %s", priv->uri);
		} else {
			namespace_uri = g_strndup (priv->uri, hash - priv->uri + 1);
			namespace = tracker_ontologies_get_namespace_by_uri (namespace_uri);
			if (namespace == NULL) {
				g_critical ("Unknown namespace %s of class %s", namespace_uri, priv->uri);
			} else {
				priv->name = g_strdup_printf ("%s:%s", tracker_namespace_get_prefix (namespace), hash + 1);
			}
			g_free (namespace_uri);
		}
	}
}

void
tracker_class_set_count (TrackerClass *service,
                         gint          value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	priv->count = value;
}


void
tracker_class_set_id (TrackerClass *service,
                      gint          value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	priv->id = value;
}


void
tracker_class_add_super_class (TrackerClass *service,
                               TrackerClass *value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));
	g_return_if_fail (TRACKER_IS_CLASS (value));

	priv = GET_PRIV (service);

	g_array_append_val (priv->super_classes, value);
}

void
tracker_class_set_is_new (TrackerClass *service,
                          gboolean      value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	priv->is_new = value;
}

void
tracker_class_set_notify (TrackerClass *service,
                          gboolean      value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	priv->notify = value;
}

void
tracker_class_set_db_schema_changed (TrackerClass *service,
                                     gboolean      value)
{
	TrackerClassPrivate *priv;

	g_return_if_fail (TRACKER_IS_CLASS (service));

	priv = GET_PRIV (service);

	priv->db_schema_changed = value;
}
