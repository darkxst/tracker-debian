/*
 * Copyright (C) 2010, Nokia <ivan.frade@nokia.com>
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

#ifndef __TRACKER_GSF_H__
#define __TRACKER_GSF_H__

#include <glib.h>
#include <gsf/gsf.h>

G_BEGIN_DECLS

void tracker_gsf_parse_xml_in_zip (const gchar         *zip_file_uri,
                                   const gchar         *xml_filename,
                                   GMarkupParseContext *context);

G_END_DECLS

#endif /* __TRACKER_GSF_H__ */
