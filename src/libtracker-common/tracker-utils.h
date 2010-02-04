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

#ifndef __LIBTRACKER_COMMON_UTILS_H__
#define __LIBTRACKER_COMMON_UTILS_H__

#if !defined (__LIBTRACKER_COMMON_INSIDE__) && !defined (TRACKER_COMPILATION)
#error "only <libtracker-common/tracker-common.h> must be included directly."
#endif

#include <glib.h>
#include "tracker-config.h"

gboolean tracker_is_empty_string	    (const char    *str);
gchar *  tracker_string_replace		    (const gchar   *haystack,
					     const gchar   *needle,
					     const gchar   *replacement);
gchar *  tracker_string_remove		    (gchar	   *haystack,
					     const gchar   *needle);
gchar *  tracker_escape_db_string           (const gchar   *str,
					     gboolean       add_quotes,
					     gboolean       escape_percent);
gchar *  tracker_seconds_estimate_to_string (gdouble	    seconds_elapsed,
					     gboolean	    short_string,
					     guint	    items_done,
					     guint	    items_remaining);
gchar *  tracker_seconds_to_string	    (gdouble	    seconds_elapsed,
					     gboolean	    short_string);
void	 tracker_throttle		    (TrackerConfig *config,
					     gint	    multiplier);

/* Temporary: Just here until we upgrade to GLib 2.18. */
G_CONST_RETURN gchar *tracker_dngettext     (const gchar *domain,
					     const gchar *msgid,
					     const gchar *msgid_plural,
					     gulong       n);

#endif /* __LIBTRACKER_COMMON_UTILS_H__ */
