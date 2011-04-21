/* tracker-resdump.c generated by valac 0.12.0, the Vala compiler
 * generated from tracker-resdump.vala, do not modify */

/*
 * Copyright (C) 2010, Nokia
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

#include <glib.h>
#include <glib-object.h>
#include <libtracker-sparql/tracker-sparql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gee.h>

#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))


extern TrackerSparqlConnection* conn;
TrackerSparqlConnection* conn = NULL;
extern GeeSet* looked_up_iris;
GeeSet* looked_up_iris = NULL;

void usage (gchar** args, int args_length1);
gboolean dump_resource (const gchar* urn);
gint _vala_main (gchar** args, int args_length1);


void usage (gchar** args, int args_length1) {
	fprintf (stderr, "Usage: %s urn\n", args[0]);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


gboolean dump_resource (const gchar* urn) {
	gboolean result = FALSE;
	GeeArrayList* _tmp0_ = NULL;
	GeeList* iris_to_lookup;
	gchar* _tmp1_ = NULL;
	gchar* _tmp2_;
	TrackerSparqlCursor* _tmp3_ = NULL;
	TrackerSparqlCursor* _tmp4_;
	TrackerSparqlCursor* cursor;
	GeeArrayList* _tmp5_ = NULL;
	GeeList* type_statements;
	GeeArrayList* _tmp6_ = NULL;
	GeeList* statements;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (urn != NULL, FALSE);
	_tmp0_ = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL);
	iris_to_lookup = (GeeList*) _tmp0_;
	gee_collection_add ((GeeCollection*) looked_up_iris, urn);
	_tmp1_ = g_strdup_printf ("SELECT ?p rdfs:range(?p) ?o {<%s> ?p ?o}", urn);
	_tmp2_ = _tmp1_;
	_tmp3_ = tracker_sparql_connection_query (conn, _tmp2_, NULL, &_inner_error_);
	_tmp4_ = _tmp3_;
	_g_free0 (_tmp2_);
	cursor = _tmp4_;
	if (_inner_error_ != NULL) {
		goto __catch0_g_error;
	}
	_tmp5_ = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL);
	type_statements = (GeeList*) _tmp5_;
	_tmp6_ = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL);
	statements = (GeeList*) _tmp6_;
	while (TRUE) {
		gboolean _tmp7_;
		gboolean _tmp8_;
		const gchar* _tmp9_ = NULL;
		gboolean _tmp10_;
		const gchar* _tmp11_ = NULL;
		gchar* _tmp12_ = NULL;
		gchar* statement;
		const gchar* _tmp13_ = NULL;
		const gchar* _tmp14_;
		GQuark _tmp15_;
		static GQuark _tmp15__label0 = 0;
		static GQuark _tmp15__label1 = 0;
		static GQuark _tmp15__label2 = 0;
		static GQuark _tmp15__label3 = 0;
		static GQuark _tmp15__label4 = 0;
		const gchar* _tmp31_ = NULL;
		_tmp7_ = tracker_sparql_cursor_next (cursor, NULL, &_inner_error_);
		_tmp8_ = _tmp7_;
		if (_inner_error_ != NULL) {
			_g_object_unref0 (statements);
			_g_object_unref0 (type_statements);
			_g_object_unref0 (cursor);
			goto __catch0_g_error;
		}
		if (!_tmp8_) {
			break;
		}
		_tmp9_ = tracker_sparql_cursor_get_string (cursor, 0, NULL);
		_tmp10_ = g_str_has_prefix (_tmp9_, "http://www.tracker-project.org/ontologies/tracker#");
		if (_tmp10_) {
			continue;
		}
		_tmp11_ = tracker_sparql_cursor_get_string (cursor, 0, NULL);
		_tmp12_ = g_strdup_printf ("<%s> <%s> ", urn, _tmp11_);
		statement = _tmp12_;
		_tmp13_ = tracker_sparql_cursor_get_string (cursor, 1, NULL);
		_tmp14_ = _tmp13_;
		_tmp15_ = (NULL == _tmp14_) ? 0 : g_quark_from_string (_tmp14_);
		if ((_tmp15_ == ((0 != _tmp15__label0) ? _tmp15__label0 : (_tmp15__label0 = g_quark_from_static_string ("http://www.w3.org/2001/XMLSchema#string")))) || (_tmp15_ == ((0 != _tmp15__label1) ? _tmp15__label1 : (_tmp15__label1 = g_quark_from_static_string ("http://www.w3.org/2001/XMLSchema#dateTime"))))) {
			switch (0) {
				default:
				{
					const gchar* _tmp16_ = NULL;
					gchar* _tmp17_ = NULL;
					gchar* _tmp18_;
					gchar* _tmp19_ = NULL;
					gchar* _tmp20_;
					gchar* _tmp21_;
					_tmp16_ = tracker_sparql_cursor_get_string (cursor, 2, NULL);
					_tmp17_ = tracker_sparql_escape_string (_tmp16_);
					_tmp18_ = _tmp17_;
					_tmp19_ = g_strdup_printf ("\"%s\"", _tmp18_);
					_tmp20_ = _tmp19_;
					_tmp21_ = g_strconcat (statement, _tmp20_, NULL);
					_g_free0 (statement);
					statement = _tmp21_;
					_g_free0 (_tmp20_);
					_g_free0 (_tmp18_);
					break;
				}
			}
		} else if (((_tmp15_ == ((0 != _tmp15__label2) ? _tmp15__label2 : (_tmp15__label2 = g_quark_from_static_string ("http://www.w3.org/2001/XMLSchema#integer")))) || (_tmp15_ == ((0 != _tmp15__label3) ? _tmp15__label3 : (_tmp15__label3 = g_quark_from_static_string ("http://www.w3.org/2001/XMLSchema#double"))))) || (_tmp15_ == ((0 != _tmp15__label4) ? _tmp15__label4 : (_tmp15__label4 = g_quark_from_static_string ("http://www.w3.org/2001/XMLSchema#boolean"))))) {
			switch (0) {
				default:
				{
					const gchar* _tmp22_ = NULL;
					gchar* _tmp23_ = NULL;
					gchar* _tmp24_;
					gchar* _tmp25_;
					_tmp22_ = tracker_sparql_cursor_get_string (cursor, 2, NULL);
					_tmp23_ = g_strdup_printf ("%s", _tmp22_);
					_tmp24_ = _tmp23_;
					_tmp25_ = g_strconcat (statement, _tmp24_, NULL);
					_g_free0 (statement);
					statement = _tmp25_;
					_g_free0 (_tmp24_);
					break;
				}
			}
		} else {
			switch (0) {
				default:
				{
					const gchar* _tmp26_ = NULL;
					const gchar* obj;
					gboolean _tmp27_;
					gchar* _tmp28_ = NULL;
					gchar* _tmp29_;
					gchar* _tmp30_;
					_tmp26_ = tracker_sparql_cursor_get_string (cursor, 2, NULL);
					obj = _tmp26_;
					_tmp27_ = gee_collection_contains ((GeeCollection*) looked_up_iris, obj);
					if (!_tmp27_) {
						gee_collection_add ((GeeCollection*) iris_to_lookup, obj);
					}
					_tmp28_ = g_strdup_printf ("<%s>", obj);
					_tmp29_ = _tmp28_;
					_tmp30_ = g_strconcat (statement, _tmp29_, NULL);
					_g_free0 (statement);
					statement = _tmp30_;
					_g_free0 (_tmp29_);
					break;
				}
			}
		}
		_tmp31_ = tracker_sparql_cursor_get_string (cursor, 0, NULL);
		if (g_strcmp0 (_tmp31_, "http://www.w3.org/1999/02/22-rdf-syntax-ns#type") == 0) {
			gee_collection_add ((GeeCollection*) type_statements, statement);
		} else {
			gee_collection_add ((GeeCollection*) statements, statement);
		}
		_g_free0 (statement);
	}
	{
		GeeList* _tmp32_;
		GeeList* _s_list;
		gint _tmp33_;
		gint _s_size;
		gint _s_index;
		_tmp32_ = _g_object_ref0 (type_statements);
		_s_list = _tmp32_;
		_tmp33_ = gee_collection_get_size ((GeeCollection*) _s_list);
		_s_size = _tmp33_;
		_s_index = -1;
		while (TRUE) {
			gpointer _tmp34_ = NULL;
			gchar* s;
			_s_index = _s_index + 1;
			if (!(_s_index < _s_size)) {
				break;
			}
			_tmp34_ = gee_list_get (_s_list, _s_index);
			s = (gchar*) _tmp34_;
			fprintf (stdout, "%s .\n", s);
			_g_free0 (s);
		}
		_g_object_unref0 (_s_list);
	}
	{
		GeeList* _tmp35_;
		GeeList* _s_list;
		gint _tmp36_;
		gint _s_size;
		gint _s_index;
		_tmp35_ = _g_object_ref0 (statements);
		_s_list = _tmp35_;
		_tmp36_ = gee_collection_get_size ((GeeCollection*) _s_list);
		_s_size = _tmp36_;
		_s_index = -1;
		while (TRUE) {
			gpointer _tmp37_ = NULL;
			gchar* s;
			_s_index = _s_index + 1;
			if (!(_s_index < _s_size)) {
				break;
			}
			_tmp37_ = gee_list_get (_s_list, _s_index);
			s = (gchar*) _tmp37_;
			fprintf (stdout, "%s .\n", s);
			_g_free0 (s);
		}
		_g_object_unref0 (_s_list);
	}
	{
		GeeList* _tmp38_;
		GeeList* _s_list;
		gint _tmp39_;
		gint _s_size;
		gint _s_index;
		_tmp38_ = _g_object_ref0 (iris_to_lookup);
		_s_list = _tmp38_;
		_tmp39_ = gee_collection_get_size ((GeeCollection*) _s_list);
		_s_size = _tmp39_;
		_s_index = -1;
		while (TRUE) {
			gpointer _tmp40_ = NULL;
			gchar* s;
			gboolean _tmp41_;
			_s_index = _s_index + 1;
			if (!(_s_index < _s_size)) {
				break;
			}
			_tmp40_ = gee_list_get (_s_list, _s_index);
			s = (gchar*) _tmp40_;
			_tmp41_ = dump_resource (s);
			if (!_tmp41_) {
				result = FALSE;
				_g_free0 (s);
				_g_object_unref0 (_s_list);
				_g_object_unref0 (statements);
				_g_object_unref0 (type_statements);
				_g_object_unref0 (cursor);
				_g_object_unref0 (iris_to_lookup);
				return result;
			}
			_g_free0 (s);
		}
		_g_object_unref0 (_s_list);
	}
	_g_object_unref0 (statements);
	_g_object_unref0 (type_statements);
	_g_object_unref0 (cursor);
	goto __finally0;
	__catch0_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		g_critical ("tracker-resdump.vala:90: Couldn't query info for resource %s: %s", urn, e->message);
		result = FALSE;
		_g_error_free0 (e);
		_g_object_unref0 (iris_to_lookup);
		return result;
	}
	__finally0:
	if (_inner_error_ != NULL) {
		_g_object_unref0 (iris_to_lookup);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return FALSE;
	}
	result = TRUE;
	_g_object_unref0 (iris_to_lookup);
	return result;
}


gint _vala_main (gchar** args, int args_length1) {
	gint result = 0;
	TrackerSparqlConnection* _tmp0_ = NULL;
	TrackerSparqlConnection* _tmp1_;
	GeeHashSet* _tmp2_ = NULL;
	gboolean _tmp3_;
	GError * _inner_error_ = NULL;
	if (args_length1 != 2) {
		usage (args, args_length1);
		result = 1;
		return result;
	}
	_tmp0_ = tracker_sparql_connection_get (NULL, &_inner_error_);
	_tmp1_ = _tmp0_;
	if (_inner_error_ != NULL) {
		goto __catch1_g_error;
	}
	_g_object_unref0 (conn);
	conn = _tmp1_;
	goto __finally1;
	__catch1_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		g_critical ("tracker-resdump.vala:107: Couldn't connect to Tracker: %s", e->message);
		result = 1;
		_g_error_free0 (e);
		return result;
	}
	__finally1:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return 0;
	}
	_tmp2_ = gee_hash_set_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL, NULL);
	_g_object_unref0 (looked_up_iris);
	looked_up_iris = (GeeSet*) _tmp2_;
	_tmp3_ = dump_resource (args[1]);
	if (_tmp3_) {
		result = 0;
		return result;
	}
	result = 1;
	return result;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return _vala_main (argv, argc);
}



