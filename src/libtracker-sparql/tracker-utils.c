/* tracker-utils.c generated by valac 0.24.0.131-42e78, the Vala compiler
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
	GString* str = NULL;
	GString* _tmp0_ = NULL;
	gchar* p = NULL;
	const gchar* _tmp1_ = NULL;
	GString* _tmp19_ = NULL;
	const gchar* _tmp20_ = NULL;
	gchar* _tmp21_ = NULL;
#line 48 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	g_return_val_if_fail (literal != NULL, NULL);
#line 49 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp0_ = g_string_new ("");
#line 49 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	str = _tmp0_;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp1_ = literal;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	p = (gchar*) _tmp1_;
#line 57 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	while (TRUE) {
#line 86 "tracker-utils.c"
		gchar* _tmp2_ = NULL;
		gsize len = 0UL;
		gchar* _tmp3_ = NULL;
		gsize _tmp4_ = 0UL;
		GString* _tmp5_ = NULL;
		gchar* _tmp6_ = NULL;
		gsize _tmp7_ = 0UL;
		gchar* _tmp8_ = NULL;
		gsize _tmp9_ = 0UL;
		gchar* _tmp10_ = NULL;
		gchar* _tmp18_ = NULL;
#line 57 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp2_ = p;
#line 57 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		if (!((*_tmp2_) != '\0')) {
#line 57 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			break;
#line 104 "tracker-utils.c"
		}
#line 58 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp3_ = p;
#line 58 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp4_ = strcspn ((const gchar*) _tmp3_, "\t\n\r\b\f\"\\");
#line 58 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		len = _tmp4_;
#line 59 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp5_ = str;
#line 59 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp6_ = p;
#line 59 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp7_ = len;
#line 59 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		g_string_append_len (_tmp5_, (const gchar*) _tmp6_, (gssize) ((glong) _tmp7_));
#line 60 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp8_ = p;
#line 60 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp9_ = len;
#line 60 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		p = _tmp8_ + _tmp9_;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp10_ = p;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		switch (*_tmp10_) {
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\t':
#line 132 "tracker-utils.c"
			{
				GString* _tmp11_ = NULL;
#line 64 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp11_ = str;
#line 64 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp11_, "\\t");
#line 65 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 141 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\n':
#line 145 "tracker-utils.c"
			{
				GString* _tmp12_ = NULL;
#line 67 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp12_ = str;
#line 67 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp12_, "\\n");
#line 68 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 154 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\r':
#line 158 "tracker-utils.c"
			{
				GString* _tmp13_ = NULL;
#line 70 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp13_ = str;
#line 70 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp13_, "\\r");
#line 71 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 167 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\b':
#line 171 "tracker-utils.c"
			{
				GString* _tmp14_ = NULL;
#line 73 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp14_ = str;
#line 73 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp14_, "\\b");
#line 74 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 180 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\f':
#line 184 "tracker-utils.c"
			{
				GString* _tmp15_ = NULL;
#line 76 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp15_ = str;
#line 76 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp15_, "\\f");
#line 77 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 193 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '"':
#line 197 "tracker-utils.c"
			{
				GString* _tmp16_ = NULL;
#line 79 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp16_ = str;
#line 79 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp16_, "\\\"");
#line 80 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 206 "tracker-utils.c"
			}
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
			case '\\':
#line 210 "tracker-utils.c"
			{
				GString* _tmp17_ = NULL;
#line 82 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				_tmp17_ = str;
#line 82 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				g_string_append (_tmp17_, "\\\\");
#line 83 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				break;
#line 219 "tracker-utils.c"
			}
			default:
			{
#line 85 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
				continue;
#line 225 "tracker-utils.c"
			}
		}
#line 88 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		_tmp18_ = p;
#line 88 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
		p = _tmp18_ + 1;
#line 232 "tracker-utils.c"
	}
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp19_ = str;
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp20_ = _tmp19_->str;
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp21_ = g_strdup (_tmp20_);
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	result = _tmp21_;
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_g_string_free0 (str);
#line 91 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	return result;
#line 246 "tracker-utils.c"
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
	guchar* base_uuid = NULL;
	guchar* _tmp0_ = NULL;
	gint base_uuid_length1 = 0;
	gint _base_uuid_size_ = 0;
	gchar* lower = NULL;
	gchar* _tmp1_ = NULL;
	gchar* _tmp2_ = NULL;
#line 114 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp0_ = g_new0 (guchar, 16);
#line 114 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	base_uuid = _tmp0_;
#line 114 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	base_uuid_length1 = 16;
#line 114 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_base_uuid_size_ = base_uuid_length1;
#line 115 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp1_ = g_new0 (gchar, 37);
#line 115 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	lower = (gchar*) _tmp1_;
#line 117 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	uuid_generate (base_uuid);
#line 118 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	uuid_unparse_lower (base_uuid, (gchar*) lower);
#line 121 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_tmp2_ = g_strdup_printf ("urn:uuid:%s", lower);
#line 121 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	result = _tmp2_;
#line 121 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	_g_free0 (lower);
#line 121 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	base_uuid = (g_free (base_uuid), NULL);
#line 121 "/home/martyn/Source/checkout/gnome/tracker/src/libtracker-sparql/tracker-utils.vala"
	return result;
#line 298 "tracker-utils.c"
}



