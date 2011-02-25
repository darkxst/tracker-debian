/* tracker-utils.c generated by valac 0.11.4, the Vala compiler
 * generated from tracker-utils.vala, do not modify */

/*
 * Copyright (C) 2010, Nokia <ivan.frade@nokia.com>
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

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define _g_string_free0(var) ((var == NULL) ? NULL : (var = (g_string_free (var, TRUE), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))



gchar* tracker_sparql_escape_uri_vprintf (const gchar* format, va_list args);
gchar* tracker_sparql_escape_uri_printf (const gchar* format, ...);
gchar* tracker_sparql_escape_string (const gchar* literal);
void uuid_generate (guchar* uuid);
void uuid_unparse_lower (guchar* uuid, gchar* out);
gchar* tracker_sparql_get_uuid_urn (void);


/**
 * SECTION: tracker-misc
 * @short_description: General purpose utilities provided by the library
 * @title: Utilities
 * @stability: Stable
 * @include: tracker-sparql.h
 *
 * <para>
 * The libtracker-sparql utilities help in the creation of proper SPARQL queries.
 * </para>
 */
/**
 * tracker_sparql_escape_string:
 * @literal: a string to escape
 *
 * Escapes a string so that it can be used in a SPARQL query.
 *
 * Returns: a newly-allocated string with the escaped version of @literal.
 * The returned string should be freed with g_free() when no longer needed.
 *
 * Since: 0.10
 */
gchar* tracker_sparql_escape_string (const gchar* literal) {
	gchar* result = NULL;
	GString* _tmp0_ = NULL;
	GString* str;
	gchar* p;
	gchar* _tmp2_;
	g_return_val_if_fail (literal != NULL, NULL);
	_tmp0_ = g_string_new ("");
	str = _tmp0_;
	p = literal;
	while (TRUE) {
		gsize _tmp1_;
		gsize len;
		if (!((*p) != '\0')) {
			break;
		}
		_tmp1_ = strcspn ((const gchar*) p, "\t\n\r\b\f\"\\");
		len = _tmp1_;
		g_string_append_len (str, (const gchar*) p, (gssize) ((glong) len));
		p = p + len;
		switch (*p) {
			case '\t':
			{
				g_string_append (str, "\\t");
				break;
			}
			case '\n':
			{
				g_string_append (str, "\\n");
				break;
			}
			case '\r':
			{
				g_string_append (str, "\\r");
				break;
			}
			case '\b':
			{
				g_string_append (str, "\\b");
				break;
			}
			case '\f':
			{
				g_string_append (str, "\\f");
				break;
			}
			case '"':
			{
				g_string_append (str, "\\\"");
				break;
			}
			case '\\':
			{
				g_string_append (str, "\\\\");
				break;
			}
			default:
			{
				continue;
			}
		}
		p++;
	}
	_tmp2_ = g_strdup (str->str);
	result = _tmp2_;
	_g_string_free0 (str);
	return result;
}


/**
 * tracker_sparql_get_uuid_urn:
 *
 * Generates a unique universal identifier to be used for urns
 * when inserting SPARQL into the database. The string returned is
 * in lower case and has the format "urn:uuid:&percnt;s" where
 * &percnt;s is the uuid generated.
 *
 * Returns: a newly-allocated string. The returned string should
 * be freed with g_free() when no longer needed.
 *
 * Since: 0.10
 */
gchar* tracker_sparql_get_uuid_urn (void) {
	gchar* result = NULL;
	guchar* _tmp0_ = NULL;
	gint base_uuid_length1;
	gint _base_uuid_size_;
	guchar* _tmp1_;
	guchar* base_uuid;
	gchar* _tmp2_ = NULL;
	gchar* lower;
	gchar* _tmp3_ = NULL;
	_tmp0_ = g_new0 (guchar, 16);
	_tmp1_ = _tmp0_;
	base_uuid_length1 = 16;
	_base_uuid_size_ = base_uuid_length1;
	base_uuid = _tmp1_;
	_tmp2_ = g_new0 (gchar, 37);
	lower = (gchar*) _tmp2_;
	uuid_generate (base_uuid);
	uuid_unparse_lower (base_uuid, (gchar*) lower);
	_tmp3_ = g_strdup_printf ("urn:uuid:%s", lower);
	result = _tmp3_;
	_g_free0 (lower);
	base_uuid = (g_free (base_uuid), NULL);
	return result;
}


