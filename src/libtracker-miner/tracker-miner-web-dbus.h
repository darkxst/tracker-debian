/*
 * Copyright (C) 2010, Adrien Bustany <abustany@gnome.org>
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
 */

#ifndef __LIBTRACKER_MINER_WEB_DBUS_H__
#define __LIBTRACKER_MINER_WEB_DBUS_H__

#if !defined (__LIBTRACKER_MINER_H_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "Only <libtracker-miner/tracker-miner.h> can be included directly."
#endif

#include <glib-object.h>

#include "tracker-miner-dbus.h"
#include "tracker-miner-web.h"

G_BEGIN_DECLS

void _tracker_miner_web_dbus_authenticate         (TrackerMinerWeb        *miner,
                                                   DBusGMethodInvocation  *context,
                                                   GError                **error);
void _tracker_miner_web_dbus_get_association_data (TrackerMinerWeb        *miner,
                                                   DBusGMethodInvocation  *context,
                                                   GError                **error);
void _tracker_miner_web_dbus_associate            (TrackerMinerWeb        *miner,
                                                   GHashTable             *association_data,
                                                   DBusGMethodInvocation  *context,
                                                   GError                **error);
void _tracker_miner_web_dbus_dissociate           (TrackerMinerWeb        *miner,
                                                   DBusGMethodInvocation  *context,
                                                   GError                **error);

G_END_DECLS

#endif /* __LIBTRACKER_MINER_WEB_DBUS_H__ */
