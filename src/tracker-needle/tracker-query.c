/* tracker-query.c generated by valac 0.11.5, the Vala compiler
 * generated from tracker-query.vala, do not modify */

/**/
/* Copyright 2010, Martyn Russell <martyn@lanedo.com>*/
/**/
/* This program is free software; you can redistribute it and/or*/
/* modify it under the terms of the GNU General Public License*/
/* as published by the Free Software Foundation; either version 2*/
/* of the License, or (at your option) any later version.*/
/**/
/* This program is distributed in the hope that it will be useful,*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the*/
/* GNU General Public License for more details.*/
/**/
/* You should have received a copy of the GNU General Public License*/
/* along with this program; if not, write to the Free Software*/
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA*/
/* 02110-1301, USA.*/
/**/

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <libtracker-sparql/tracker-sparql.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <gobject/gvaluecollector.h>


#define TRACKER_TYPE_QUERY (tracker_query_get_type ())
#define TRACKER_QUERY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_QUERY, TrackerQuery))
#define TRACKER_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_QUERY, TrackerQueryClass))
#define TRACKER_IS_QUERY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_QUERY))
#define TRACKER_IS_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_QUERY))
#define TRACKER_QUERY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_QUERY, TrackerQueryClass))

typedef struct _TrackerQuery TrackerQuery;
typedef struct _TrackerQueryClass TrackerQueryClass;
typedef struct _TrackerQueryPrivate TrackerQueryPrivate;

#define TRACKER_QUERY_TYPE_TYPE (tracker_query_type_get_type ())
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _tracker_query_unref0(var) ((var == NULL) ? NULL : (var = (tracker_query_unref (var), NULL)))
typedef struct _TrackerQueryPerformAsyncData TrackerQueryPerformAsyncData;
typedef struct _TrackerParamSpecQuery TrackerParamSpecQuery;

struct _TrackerQuery {
	GTypeInstance parent_instance;
	volatile int ref_count;
	TrackerQueryPrivate * priv;
};

struct _TrackerQueryClass {
	GTypeClass parent_class;
	void (*finalize) (TrackerQuery *self);
};

struct _TrackerQueryPrivate {
	gchar* _criteria;
	guint _offset;
	guint _limit;
	gchar* _query;
};

typedef enum  {
	TRACKER_QUERY_TYPE_ALL,
	TRACKER_QUERY_TYPE_ALL_ONLY_IN_TITLES,
	TRACKER_QUERY_TYPE_CONTACTS,
	TRACKER_QUERY_TYPE_APPLICATIONS,
	TRACKER_QUERY_TYPE_MUSIC,
	TRACKER_QUERY_TYPE_IMAGES,
	TRACKER_QUERY_TYPE_VIDEOS,
	TRACKER_QUERY_TYPE_DOCUMENTS,
	TRACKER_QUERY_TYPE_MAIL,
	TRACKER_QUERY_TYPE_CALENDAR,
	TRACKER_QUERY_TYPE_FOLDERS
} TrackerQueryType;

struct _TrackerQueryPerformAsyncData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	TrackerQuery* self;
	TrackerQueryType query_type;
	GCancellable* cancellable;
	TrackerSparqlCursor* result;
	TrackerSparqlCursor* cursor;
	gboolean _tmp0_;
	gint _tmp1_;
	gchar* _tmp2_;
	gchar* criteria_escaped;
	const gchar* _tmp3_;
	gchar* _tmp4_;
	gchar* unknown;
	const gchar* _tmp5_;
	const gchar* _tmp6_;
	gchar* _tmp7_;
	gchar* _tmp8_;
	gchar* _tmp9_;
	gchar* _tmp10_;
	gchar* _tmp11_;
	gchar* _tmp12_;
	gchar* _tmp13_;
	gchar* criteria_escaped_down;
	const gchar* _tmp14_;
	const gchar* _tmp15_;
	gchar* _tmp16_;
	gchar* _tmp17_;
	gchar* _tmp18_;
	gchar* _tmp19_;
	gchar* _tmp20_;
	gchar* _tmp21_;
	const gchar* _tmp22_;
	const gchar* _tmp23_;
	gchar* _tmp24_;
	gchar* _tmp25_;
	gchar* _tmp26_;
	gchar* _tmp27_;
	gchar* _tmp28_;
	gchar* _tmp29_;
	const gchar* _tmp30_;
	const gchar* _tmp31_;
	gchar* _tmp32_;
	gchar* _tmp33_;
	gchar* _tmp34_;
	gchar* _tmp35_;
	gchar* _tmp36_;
	gchar* _tmp37_;
	const gchar* _tmp38_;
	const gchar* _tmp39_;
	gchar* _tmp40_;
	gchar* _tmp41_;
	gchar* _tmp42_;
	gchar* _tmp43_;
	gchar* _tmp44_;
	gchar* _tmp45_;
	const gchar* _tmp46_;
	const gchar* _tmp47_;
	gchar* _tmp48_;
	gchar* _tmp49_;
	gchar* _tmp50_;
	gchar* _tmp51_;
	gchar* _tmp52_;
	gchar* _tmp53_;
	const gchar* _tmp54_;
	gchar* _tmp55_;
	gchar* pages;
	const gchar* _tmp56_;
	const gchar* _tmp57_;
	const gchar* _tmp58_;
	gchar* _tmp59_;
	gchar* _tmp60_;
	gchar* _tmp61_;
	gchar* _tmp62_;
	gchar* _tmp63_;
	gchar* _tmp64_;
	const gchar* _tmp65_;
	gchar* _tmp66_;
	gchar* no_subject;
	const gchar* _tmp67_;
	gchar* _tmp68_;
	gchar* to;
	const gchar* _tmp69_;
	const gchar* _tmp70_;
	const gchar* _tmp71_;
	const gchar* _tmp72_;
	const gchar* _tmp73_;
	gchar* _tmp74_;
	gchar* _tmp75_;
	gchar* _tmp76_;
	gchar* _tmp77_;
	gchar* _tmp78_;
	gchar* _tmp79_;
	const gchar* _tmp80_;
	const gchar* _tmp81_;
	gchar* _tmp82_;
	gchar* _tmp83_;
	gchar* _tmp84_;
	gchar* _tmp85_;
	gchar* _tmp86_;
	gchar* _tmp87_;
	TrackerSparqlCursor* _tmp88_;
	TrackerSparqlCursor* _tmp89_;
	TrackerSparqlCursor* _tmp90_;
	GError * ea;
	GError * eb;
	GError * ec;
	GError * _inner_error_;
};

struct _TrackerParamSpecQuery {
	GParamSpec parent_instance;
};


static gpointer tracker_query_parent_class = NULL;
static TrackerSparqlConnection* tracker_query_connection;
static TrackerSparqlConnection* tracker_query_connection = NULL;

gpointer tracker_query_ref (gpointer instance);
void tracker_query_unref (gpointer instance);
GParamSpec* tracker_param_spec_query (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void tracker_value_set_query (GValue* value, gpointer v_object);
void tracker_value_take_query (GValue* value, gpointer v_object);
gpointer tracker_value_get_query (const GValue* value);
GType tracker_query_get_type (void) G_GNUC_CONST;
#define TRACKER_QUERY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TRACKER_TYPE_QUERY, TrackerQueryPrivate))
enum  {
	TRACKER_QUERY_DUMMY_PROPERTY
};
GType tracker_query_type_get_type (void) G_GNUC_CONST;
TrackerQuery* tracker_query_new (void);
TrackerQuery* tracker_query_construct (GType object_type);
static void tracker_query_perform_async_data_free (gpointer _data);
void tracker_query_perform_async (TrackerQuery* self, TrackerQueryType query_type, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
TrackerSparqlCursor* tracker_query_perform_finish (TrackerQuery* self, GAsyncResult* _res_, GError** error);
static gboolean tracker_query_perform_async_co (TrackerQueryPerformAsyncData* data);
const gchar* tracker_query_get_criteria (TrackerQuery* self);
guint tracker_query_get_limit (TrackerQuery* self);
guint tracker_query_get_offset (TrackerQuery* self);
static void tracker_query_set_query (TrackerQuery* self, const gchar* value);
const gchar* tracker_query_get_query (TrackerQuery* self);
static void tracker_query_perform_async_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_);
void tracker_query_set_criteria (TrackerQuery* self, const gchar* value);
void tracker_query_set_offset (TrackerQuery* self, guint value);
void tracker_query_set_limit (TrackerQuery* self, guint value);
static void tracker_query_finalize (TrackerQuery* obj);


GType tracker_query_type_get_type (void) {
	static volatile gsize tracker_query_type_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_query_type_type_id__volatile)) {
		static const GEnumValue values[] = {{TRACKER_QUERY_TYPE_ALL, "TRACKER_QUERY_TYPE_ALL", "all"}, {TRACKER_QUERY_TYPE_ALL_ONLY_IN_TITLES, "TRACKER_QUERY_TYPE_ALL_ONLY_IN_TITLES", "all-only-in-titles"}, {TRACKER_QUERY_TYPE_CONTACTS, "TRACKER_QUERY_TYPE_CONTACTS", "contacts"}, {TRACKER_QUERY_TYPE_APPLICATIONS, "TRACKER_QUERY_TYPE_APPLICATIONS", "applications"}, {TRACKER_QUERY_TYPE_MUSIC, "TRACKER_QUERY_TYPE_MUSIC", "music"}, {TRACKER_QUERY_TYPE_IMAGES, "TRACKER_QUERY_TYPE_IMAGES", "images"}, {TRACKER_QUERY_TYPE_VIDEOS, "TRACKER_QUERY_TYPE_VIDEOS", "videos"}, {TRACKER_QUERY_TYPE_DOCUMENTS, "TRACKER_QUERY_TYPE_DOCUMENTS", "documents"}, {TRACKER_QUERY_TYPE_MAIL, "TRACKER_QUERY_TYPE_MAIL", "mail"}, {TRACKER_QUERY_TYPE_CALENDAR, "TRACKER_QUERY_TYPE_CALENDAR", "calendar"}, {TRACKER_QUERY_TYPE_FOLDERS, "TRACKER_QUERY_TYPE_FOLDERS", "folders"}, {0, NULL, NULL}};
		GType tracker_query_type_type_id;
		tracker_query_type_type_id = g_enum_register_static ("TrackerQueryType", values);
		g_once_init_leave (&tracker_query_type_type_id__volatile, tracker_query_type_type_id);
	}
	return tracker_query_type_type_id__volatile;
}


TrackerQuery* tracker_query_construct (GType object_type) {
	TrackerQuery* self = NULL;
	TrackerSparqlConnection* _tmp0_ = NULL;
	TrackerSparqlConnection* _tmp1_;
	TrackerSparqlConnection* _tmp2_;
	GError * _inner_error_ = NULL;
	self = (TrackerQuery*) g_type_create_instance (object_type);
	_tmp0_ = tracker_sparql_connection_get (NULL, &_inner_error_);
	_tmp1_ = _tmp0_;
	if (_inner_error_ != NULL) {
		if (_inner_error_->domain == TRACKER_SPARQL_ERROR) {
			goto __catch3_tracker_sparql_error;
		}
		if (_inner_error_->domain == G_IO_ERROR) {
			goto __catch3_g_io_error;
		}
		if (_inner_error_->domain == G_DBUS_ERROR) {
			goto __catch3_g_dbus_error;
		}
		g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	_tmp2_ = _tmp1_;
	_g_object_unref0 (tracker_query_connection);
	tracker_query_connection = _tmp2_;
	goto __finally3;
	__catch3_tracker_sparql_error:
	{
		GError * ea;
		ea = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-query.vala:48: Could not get Sparql connection: %s", ea->message);
		_g_error_free0 (ea);
	}
	goto __finally3;
	__catch3_g_io_error:
	{
		GError * eb;
		eb = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-query.vala:50: Could not get Sparql connection: %s", eb->message);
		_g_error_free0 (eb);
	}
	goto __finally3;
	__catch3_g_dbus_error:
	{
		GError * ec;
		ec = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-query.vala:52: Could not get Sparql connection: %s", ec->message);
		_g_error_free0 (ec);
	}
	__finally3:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	return self;
}


TrackerQuery* tracker_query_new (void) {
	return tracker_query_construct (TRACKER_TYPE_QUERY);
}


static void tracker_query_perform_async_data_free (gpointer _data) {
	TrackerQueryPerformAsyncData* data;
	data = _data;
	_g_object_unref0 (data->cancellable);
	_g_object_unref0 (data->result);
	_tracker_query_unref0 (data->self);
	g_slice_free (TrackerQueryPerformAsyncData, data);
}


static gpointer _tracker_query_ref0 (gpointer self) {
	return self ? tracker_query_ref (self) : NULL;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


void tracker_query_perform_async (TrackerQuery* self, TrackerQueryType query_type, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	TrackerQueryPerformAsyncData* _data_;
	_data_ = g_slice_new0 (TrackerQueryPerformAsyncData);
	_data_->_async_result = g_simple_async_result_new (NULL, _callback_, _user_data_, tracker_query_perform_async);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, tracker_query_perform_async_data_free);
	_data_->self = _tracker_query_ref0 (self);
	_data_->query_type = query_type;
	_data_->cancellable = _g_object_ref0 (cancellable);
	tracker_query_perform_async_co (_data_);
}


TrackerSparqlCursor* tracker_query_perform_finish (TrackerQuery* self, GAsyncResult* _res_, GError** error) {
	TrackerSparqlCursor* result;
	TrackerQueryPerformAsyncData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return NULL;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	_data_->result = NULL;
	return result;
}


static const gchar* string_to_string (const gchar* self) {
	const gchar* result = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	result = self;
	return result;
}


static void tracker_query_perform_async_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_) {
	TrackerQueryPerformAsyncData* data;
	data = _user_data_;
	data->_source_object_ = source_object;
	data->_res_ = _res_;
	tracker_query_perform_async_co (data);
}


static gboolean tracker_query_perform_async_co (TrackerQueryPerformAsyncData* data) {
	switch (data->_state_) {
		case 0:
		goto _state_0;
		case 1:
		goto _state_1;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	g_return_val_if_fail (tracker_query_connection != NULL, FALSE);
	data->cursor = NULL;
	if (data->self->priv->_criteria == NULL) {
		data->_tmp0_ = TRUE;
	} else {
		data->_tmp1_ = strlen (data->self->priv->_criteria);
		data->_tmp0_ = data->_tmp1_ < 1;
	}
	if (data->_tmp0_) {
		g_warning ("tracker-query.vala:61: Criteria was NULL or an empty string, no query " \
"performed");
		data->result = NULL;
		_g_object_unref0 (data->cursor);
		if (data->_state_ == 0) {
			g_simple_async_result_complete_in_idle (data->_async_result);
		} else {
			g_simple_async_result_complete (data->_async_result);
		}
		g_object_unref (data->_async_result);
		return FALSE;
	}
	if (data->self->priv->_limit < 1) {
		g_warning ("tracker-query.vala:66: Limit was < 1, no query performed");
		data->result = NULL;
		_g_object_unref0 (data->cursor);
		if (data->_state_ == 0) {
			g_simple_async_result_complete_in_idle (data->_async_result);
		} else {
			g_simple_async_result_complete (data->_async_result);
		}
		g_object_unref (data->_async_result);
		return FALSE;
	}
	data->_tmp2_ = NULL;
	data->_tmp2_ = tracker_sparql_escape_string (data->self->priv->_criteria);
	data->criteria_escaped = data->_tmp2_;
	data->_tmp3_ = NULL;
	data->_tmp3_ = _ ("Unknown");
	data->_tmp4_ = g_strdup (data->_tmp3_);
	data->unknown = data->_tmp4_;
	switch (data->query_type) {
		case TRACKER_QUERY_TYPE_ALL:
		{
			data->_tmp5_ = NULL;
			data->_tmp5_ = string_to_string (data->unknown);
			data->_tmp6_ = NULL;
			data->_tmp6_ = string_to_string (data->criteria_escaped);
			data->_tmp7_ = NULL;
			data->_tmp7_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp8_ = data->_tmp7_;
			data->_tmp9_ = NULL;
			data->_tmp9_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp10_ = data->_tmp9_;
			data->_tmp11_ = NULL;
			data->_tmp11_ = g_strconcat ("SELECT ?u nie:url(?u) tracker:coalesce(nie:title(?u), nfo:fileName(?u)" \
", \"", data->_tmp5_, "\") nfo:fileLastModified(?u) nfo:fileSize(?u) nie:url(?c) WHERE { ?u f" \
"ts:match \"", data->_tmp6_, "\" . ?u nfo:belongsToContainer ?c ; tracker:available true . } ORDER B" \
"Y DESC(fts:rank(?u)) OFFSET ", data->_tmp8_, " LIMIT ", data->_tmp10_, NULL);
			data->_tmp12_ = data->_tmp11_;
			tracker_query_set_query (data->self, data->_tmp12_);
			_g_free0 (data->_tmp12_);
			_g_free0 (data->_tmp10_);
			_g_free0 (data->_tmp8_);
			break;
		}
		case TRACKER_QUERY_TYPE_ALL_ONLY_IN_TITLES:
		{
			data->_tmp13_ = NULL;
			data->_tmp13_ = g_utf8_strdown (data->criteria_escaped, (gssize) (-1));
			data->criteria_escaped_down = data->_tmp13_;
			data->_tmp14_ = NULL;
			data->_tmp14_ = string_to_string (data->unknown);
			data->_tmp15_ = NULL;
			data->_tmp15_ = string_to_string (data->criteria_escaped_down);
			data->_tmp16_ = NULL;
			data->_tmp16_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp17_ = data->_tmp16_;
			data->_tmp18_ = NULL;
			data->_tmp18_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp19_ = data->_tmp18_;
			data->_tmp20_ = NULL;
			data->_tmp20_ = g_strconcat ("SELECT ?u nie:url(?u) tracker:coalesce(nfo:fileName(?u), \"", data->_tmp14_, "\") nfo:fileLastModified(?u) nfo:fileSize(?u) nie:url(?c) WHERE { ?u a" \
" nfo:FileDataObject ; nfo:belongsToContainer ?c ; tracker:available tr" \
"ue . FILTER(fn:contains(fn:lower-case(nfo:fileName(?u)), \"", data->_tmp15_, "\")) } ORDER BY DESC(nfo:fileName(?u)) OFFSET ", data->_tmp17_, " LIMIT ", data->_tmp19_, NULL);
			data->_tmp21_ = data->_tmp20_;
			tracker_query_set_query (data->self, data->_tmp21_);
			_g_free0 (data->_tmp21_);
			_g_free0 (data->_tmp19_);
			_g_free0 (data->_tmp17_);
			_g_free0 (data->criteria_escaped_down);
			break;
		}
		case TRACKER_QUERY_TYPE_APPLICATIONS:
		{
			data->_tmp22_ = NULL;
			data->_tmp22_ = string_to_string (data->unknown);
			data->_tmp23_ = NULL;
			data->_tmp23_ = string_to_string (data->criteria_escaped);
			data->_tmp24_ = NULL;
			data->_tmp24_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp25_ = data->_tmp24_;
			data->_tmp26_ = NULL;
			data->_tmp26_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp27_ = data->_tmp26_;
			data->_tmp28_ = NULL;
			data->_tmp28_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn\n" \
"			          tracker:coalesce(nfo:softwareCmdLine(?urn), ?urn)\n" \
"			          tracker:coalesce(nie:title(?urn), nfo:fileName(?urn), \"", data->_tmp22_, "\")\n" \
"			          nie:comment(?urn)\n" \
"			        WHERE {\n" \
"			          ?urn a nfo:Software ;\n" \
"			               fts:match \"", data->_tmp23_, "\"\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nie:title(?urn))\n" \
"			        OFFSET ", data->_tmp25_, " LIMIT ", data->_tmp27_, "\n			        ", NULL);
			data->_tmp29_ = data->_tmp28_;
			tracker_query_set_query (data->self, data->_tmp29_);
			_g_free0 (data->_tmp29_);
			_g_free0 (data->_tmp27_);
			_g_free0 (data->_tmp25_);
			break;
		}
		case TRACKER_QUERY_TYPE_MUSIC:
		{
			data->_tmp30_ = NULL;
			data->_tmp30_ = string_to_string (data->unknown);
			data->_tmp31_ = NULL;
			data->_tmp31_ = string_to_string (data->criteria_escaped);
			data->_tmp32_ = NULL;
			data->_tmp32_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp33_ = data->_tmp32_;
			data->_tmp34_ = NULL;
			data->_tmp34_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp35_ = data->_tmp34_;
			data->_tmp36_ = NULL;
			data->_tmp36_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?song\n" \
"			          nie:url(?song)\n" \
"			          tracker:coalesce(nie:title(?song), nfo:fileName(?song), \"", data->_tmp30_, "\")\n" \
"			          fn:string-join((?performer, ?album), \" - \")\n" \
"			          nfo:duration(?song)\n" \
"			          ?tooltip\n" \
"			        WHERE {\n" \
"			          ?match fts:match \"", data->_tmp31_, "\"\n" \
"			          {\n" \
"			            ?song nmm:musicAlbum ?match\n" \
"			          } UNION {\n" \
"			            ?song nmm:performer ?match\n" \
"			          } UNION {\n" \
"			            ?song a nfo:Audio .\n" \
"			            ?match a nfo:Audio\n" \
"			            FILTER (?song = ?match)\n" \
"			          }\n" \
"			          ?song nmm:performer [ nmm:artistName ?performer ] ;\n" \
"			                nmm:musicAlbum [ nie:title ?album ] ;\n" \
"			                nie:url ?tooltip\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?song)) DESC(nie:title(?song))\n" \
"			        OFFSET ", data->_tmp33_, " LIMIT ", data->_tmp35_, "\n			        ", NULL);
			data->_tmp37_ = data->_tmp36_;
			tracker_query_set_query (data->self, data->_tmp37_);
			_g_free0 (data->_tmp37_);
			_g_free0 (data->_tmp35_);
			_g_free0 (data->_tmp33_);
			break;
		}
		case TRACKER_QUERY_TYPE_IMAGES:
		{
			data->_tmp38_ = NULL;
			data->_tmp38_ = string_to_string (data->unknown);
			data->_tmp39_ = NULL;
			data->_tmp39_ = string_to_string (data->criteria_escaped);
			data->_tmp40_ = NULL;
			data->_tmp40_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp41_ = data->_tmp40_;
			data->_tmp42_ = NULL;
			data->_tmp42_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp43_ = data->_tmp42_;
			data->_tmp44_ = NULL;
			data->_tmp44_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn \n" \
"			          nie:url(?urn) \n" \
"			          tracker:coalesce(nie:title(?urn), nfo:fileName(?urn), \"", data->_tmp38_, "\") \n" \
"			          fn:string-join((nfo:height(?urn), nfo:width(?urn)), \" x " \
"\") \n" \
"			          nfo:fileSize(?urn)\n" \
"			          ?tooltip\n" \
"			        WHERE {\n" \
"			          ?urn a nfo:Image ;\n" \
"			          nie:url ?tooltip ;\n" \
"			          fts:match \"", data->_tmp39_, "\" \n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nie:title(?urn)) \n" \
"			        OFFSET ", data->_tmp41_, " LIMIT ", data->_tmp43_, "\n			        ", NULL);
			data->_tmp45_ = data->_tmp44_;
			tracker_query_set_query (data->self, data->_tmp45_);
			_g_free0 (data->_tmp45_);
			_g_free0 (data->_tmp43_);
			_g_free0 (data->_tmp41_);
			break;
		}
		case TRACKER_QUERY_TYPE_VIDEOS:
		{
			data->_tmp46_ = NULL;
			data->_tmp46_ = string_to_string (data->unknown);
			data->_tmp47_ = NULL;
			data->_tmp47_ = string_to_string (data->criteria_escaped);
			data->_tmp48_ = NULL;
			data->_tmp48_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp49_ = data->_tmp48_;
			data->_tmp50_ = NULL;
			data->_tmp50_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp51_ = data->_tmp50_;
			data->_tmp52_ = NULL;
			data->_tmp52_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn \n" \
"			          nie:url(?urn) \n" \
"			          tracker:coalesce(nie:title(?urn), nfo:fileName(?urn), \"", data->_tmp46_, "\") \n" \
"			          \"\"\n" \
"			          nfo:duration(?urn)\n" \
"			          ?tooltip\n" \
"			        WHERE {\n" \
"			          ?urn a nfo:Video ;\n" \
"			          nie:url ?tooltip ;\n" \
"			          fts:match \"", data->_tmp47_, "\" .\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nie:title(?urn)) \n" \
"			        OFFSET ", data->_tmp49_, " LIMIT ", data->_tmp51_, "\n			        ", NULL);
			data->_tmp53_ = data->_tmp52_;
			tracker_query_set_query (data->self, data->_tmp53_);
			_g_free0 (data->_tmp53_);
			_g_free0 (data->_tmp51_);
			_g_free0 (data->_tmp49_);
			break;
		}
		case TRACKER_QUERY_TYPE_DOCUMENTS:
		{
			data->_tmp54_ = NULL;
			data->_tmp54_ = _ ("Pages");
			data->_tmp55_ = g_strdup (data->_tmp54_);
			data->pages = data->_tmp55_;
			data->_tmp56_ = NULL;
			data->_tmp56_ = string_to_string (data->unknown);
			data->_tmp57_ = NULL;
			data->_tmp57_ = string_to_string (data->pages);
			data->_tmp58_ = NULL;
			data->_tmp58_ = string_to_string (data->criteria_escaped);
			data->_tmp59_ = NULL;
			data->_tmp59_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp60_ = data->_tmp59_;
			data->_tmp61_ = NULL;
			data->_tmp61_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp62_ = data->_tmp61_;
			data->_tmp63_ = NULL;
			data->_tmp63_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn \n" \
"			          nie:url(?urn) \n" \
"			          tracker:coalesce(nie:title(?urn), nfo:fileName(?urn), \"", data->_tmp56_, "\") \n" \
"			          tracker:coalesce(nco:fullname(?creator), nco:fullname(?pu" \
"blisher), \"\")\n" \
"			          fn:concat(nfo:pageCount(?urn), \" ", data->_tmp57_, "\")\n" \
"			          ?tooltip\n" \
"			        WHERE {\n" \
"			          ?urn a nfo:Document ;\n" \
"			          nie:url ?tooltip ;\n" \
"			          fts:match \"", data->_tmp58_, "\" .\n" \
"			          OPTIONAL {\n" \
"			            ?urn nco:creator ?creator .\n" \
"			          }\n" \
"			          OPTIONAL {\n" \
"			            ?urn nco:publisher ?publisher .\n" \
"			          }\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nie:title(?urn)) \n" \
"			        OFFSET ", data->_tmp60_, " LIMIT ", data->_tmp62_, "\n			        ", NULL);
			data->_tmp64_ = data->_tmp63_;
			tracker_query_set_query (data->self, data->_tmp64_);
			_g_free0 (data->_tmp64_);
			_g_free0 (data->_tmp62_);
			_g_free0 (data->_tmp60_);
			_g_free0 (data->pages);
			break;
		}
		case TRACKER_QUERY_TYPE_MAIL:
		{
			data->_tmp65_ = NULL;
			data->_tmp65_ = _ ("No Subject");
			data->_tmp66_ = g_strdup (data->_tmp65_);
			data->no_subject = data->_tmp66_;
			data->_tmp67_ = NULL;
			data->_tmp67_ = _ ("To");
			data->_tmp68_ = g_strdup (data->_tmp67_);
			data->to = data->_tmp68_;
			data->_tmp69_ = NULL;
			data->_tmp69_ = string_to_string (data->unknown);
			data->_tmp70_ = NULL;
			data->_tmp70_ = string_to_string (data->no_subject);
			data->_tmp71_ = NULL;
			data->_tmp71_ = string_to_string (data->to);
			data->_tmp72_ = NULL;
			data->_tmp72_ = string_to_string (data->unknown);
			data->_tmp73_ = NULL;
			data->_tmp73_ = string_to_string (data->criteria_escaped);
			data->_tmp74_ = NULL;
			data->_tmp74_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp75_ = data->_tmp74_;
			data->_tmp76_ = NULL;
			data->_tmp76_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp77_ = data->_tmp76_;
			data->_tmp78_ = NULL;
			data->_tmp78_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn\n" \
"			          nie:url(?urn)\n" \
"			          tracker:coalesce(nco:fullname(?sender), nco:nickname(?sen" \
"der), nco:emailAddress(?sender), \"", data->_tmp69_, "\")\n			          tracker:coalesce(nmo:messageSubject(?urn), \"", data->_tmp70_, "\")\n			          nmo:receivedDate(?urn)\n			          fn:concat(\"", data->_tmp71_, ": \", tracker:coalesce(nco:fullname(?to), nco:nickname(?to), nco:email" \
"Address(?to), \"", data->_tmp72_, "\"))\n" \
"			        WHERE {\n" \
"			          ?urn a nmo:Email ;\n" \
"			          nmo:from ?sender ;\n" \
"			          nmo:to ?to ;\n" \
"			          fts:match \"", data->_tmp73_, "\" .\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nmo:messageSubject(?urn)" \
") DESC(nmo:receivedDate(?urn))\n" \
"			        OFFSET ", data->_tmp75_, " LIMIT ", data->_tmp77_, "\n			        ", NULL);
			data->_tmp79_ = data->_tmp78_;
			tracker_query_set_query (data->self, data->_tmp79_);
			_g_free0 (data->_tmp79_);
			_g_free0 (data->_tmp77_);
			_g_free0 (data->_tmp75_);
			_g_free0 (data->to);
			_g_free0 (data->no_subject);
			break;
		}
		case TRACKER_QUERY_TYPE_FOLDERS:
		{
			data->_tmp80_ = NULL;
			data->_tmp80_ = string_to_string (data->unknown);
			data->_tmp81_ = NULL;
			data->_tmp81_ = string_to_string (data->criteria_escaped);
			data->_tmp82_ = NULL;
			data->_tmp82_ = g_strdup_printf ("%u", data->self->priv->_offset);
			data->_tmp83_ = data->_tmp82_;
			data->_tmp84_ = NULL;
			data->_tmp84_ = g_strdup_printf ("%u", data->self->priv->_limit);
			data->_tmp85_ = data->_tmp84_;
			data->_tmp86_ = NULL;
			data->_tmp86_ = g_strconcat ("\n" \
"			        SELECT\n" \
"			          ?urn\n" \
"			          nie:url(?urn)\n" \
"			          tracker:coalesce(nie:title(?urn), nfo:fileName(?urn), \"", data->_tmp80_, "\")\n" \
"			          tracker:coalesce(nie:url(?parent), \"\")\n" \
"			          nfo:fileLastModified(?urn)\n" \
"			          ?tooltip\n" \
"			        WHERE {\n" \
"			          ?urn a nfo:Folder ;\n" \
"			          nie:url ?tooltip ;\n" \
"			          fts:match \"", data->_tmp81_, "\" .\n" \
"			          OPTIONAL {\n" \
"			            ?urn nfo:belongsToContainer ?parent .\n" \
"			          }\n" \
"			        }\n" \
"			        ORDER BY DESC(fts:rank(?urn)) DESC(nie:title(?urn))\n" \
"			        OFFSET ", data->_tmp83_, " LIMIT ", data->_tmp85_, "\n			        ", NULL);
			data->_tmp87_ = data->_tmp86_;
			tracker_query_set_query (data->self, data->_tmp87_);
			_g_free0 (data->_tmp87_);
			_g_free0 (data->_tmp85_);
			_g_free0 (data->_tmp83_);
			break;
		}
		default:
		{
			g_assert_not_reached ();
		}
	}
	g_debug ("tracker-query.vala:244: Running query: '%s'", data->self->priv->_query);
	data->_state_ = 1;
	tracker_sparql_connection_query_async (tracker_query_connection, data->self->priv->_query, NULL, tracker_query_perform_async_ready, data);
	return FALSE;
	_state_1:
	data->_tmp88_ = NULL;
	data->_tmp88_ = tracker_sparql_connection_query_finish (tracker_query_connection, data->_res_, &data->_inner_error_);
	data->_tmp89_ = data->_tmp88_;
	if (data->_inner_error_ != NULL) {
		if (data->_inner_error_->domain == TRACKER_SPARQL_ERROR) {
			goto __catch4_tracker_sparql_error;
		}
		if (data->_inner_error_->domain == G_IO_ERROR) {
			goto __catch4_g_io_error;
		}
		if (data->_inner_error_->domain == G_DBUS_ERROR) {
			goto __catch4_g_dbus_error;
		}
		_g_free0 (data->unknown);
		_g_free0 (data->criteria_escaped);
		_g_object_unref0 (data->cursor);
		g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, data->_inner_error_->message, g_quark_to_string (data->_inner_error_->domain), data->_inner_error_->code);
		g_clear_error (&data->_inner_error_);
		return FALSE;
	}
	data->_tmp90_ = data->_tmp89_;
	_g_object_unref0 (data->cursor);
	data->cursor = data->_tmp90_;
	goto __finally4;
	__catch4_tracker_sparql_error:
	{
		data->ea = data->_inner_error_;
		data->_inner_error_ = NULL;
		g_warning ("tracker-query.vala:249: Could not run Sparql query: %s", data->ea->message);
		_g_error_free0 (data->ea);
	}
	goto __finally4;
	__catch4_g_io_error:
	{
		data->eb = data->_inner_error_;
		data->_inner_error_ = NULL;
		g_warning ("tracker-query.vala:251: Could not run Sparql query: %s", data->eb->message);
		_g_error_free0 (data->eb);
	}
	goto __finally4;
	__catch4_g_dbus_error:
	{
		data->ec = data->_inner_error_;
		data->_inner_error_ = NULL;
		g_warning ("tracker-query.vala:253: Could not run Sparql query: %s", data->ec->message);
		_g_error_free0 (data->ec);
	}
	__finally4:
	if (data->_inner_error_ != NULL) {
		if (data->_inner_error_->domain == G_IO_ERROR) {
			g_simple_async_result_set_from_error (data->_async_result, data->_inner_error_);
			g_error_free (data->_inner_error_);
			_g_free0 (data->unknown);
			_g_free0 (data->criteria_escaped);
			_g_object_unref0 (data->cursor);
			if (data->_state_ == 0) {
				g_simple_async_result_complete_in_idle (data->_async_result);
			} else {
				g_simple_async_result_complete (data->_async_result);
			}
			g_object_unref (data->_async_result);
			return FALSE;
		} else {
			_g_free0 (data->unknown);
			_g_free0 (data->criteria_escaped);
			_g_object_unref0 (data->cursor);
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, data->_inner_error_->message, g_quark_to_string (data->_inner_error_->domain), data->_inner_error_->code);
			g_clear_error (&data->_inner_error_);
			return FALSE;
		}
	}
	g_debug ("tracker-query.vala:256: Done");
	data->result = data->cursor;
	_g_free0 (data->unknown);
	_g_free0 (data->criteria_escaped);
	if (data->_state_ == 0) {
		g_simple_async_result_complete_in_idle (data->_async_result);
	} else {
		g_simple_async_result_complete (data->_async_result);
	}
	g_object_unref (data->_async_result);
	return FALSE;
	_g_free0 (data->unknown);
	_g_free0 (data->criteria_escaped);
	_g_object_unref0 (data->cursor);
	if (data->_state_ == 0) {
		g_simple_async_result_complete_in_idle (data->_async_result);
	} else {
		g_simple_async_result_complete (data->_async_result);
	}
	g_object_unref (data->_async_result);
	return FALSE;
}


const gchar* tracker_query_get_criteria (TrackerQuery* self) {
	const gchar* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_criteria;
	return result;
}


void tracker_query_set_criteria (TrackerQuery* self, const gchar* value) {
	gchar* _tmp0_;
	gchar* _tmp1_;
	g_return_if_fail (self != NULL);
	_tmp0_ = g_strdup (value);
	_tmp1_ = _tmp0_;
	_g_free0 (self->priv->_criteria);
	self->priv->_criteria = _tmp1_;
}


guint tracker_query_get_offset (TrackerQuery* self) {
	guint result;
	g_return_val_if_fail (self != NULL, 0U);
	result = self->priv->_offset;
	return result;
}


void tracker_query_set_offset (TrackerQuery* self, guint value) {
	g_return_if_fail (self != NULL);
	self->priv->_offset = value;
}


guint tracker_query_get_limit (TrackerQuery* self) {
	guint result;
	g_return_val_if_fail (self != NULL, 0U);
	result = self->priv->_limit;
	return result;
}


void tracker_query_set_limit (TrackerQuery* self, guint value) {
	g_return_if_fail (self != NULL);
	self->priv->_limit = value;
}


const gchar* tracker_query_get_query (TrackerQuery* self) {
	const gchar* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_query;
	return result;
}


static void tracker_query_set_query (TrackerQuery* self, const gchar* value) {
	gchar* _tmp0_;
	gchar* _tmp1_;
	g_return_if_fail (self != NULL);
	_tmp0_ = g_strdup (value);
	_tmp1_ = _tmp0_;
	_g_free0 (self->priv->_query);
	self->priv->_query = _tmp1_;
}


static void tracker_value_query_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void tracker_value_query_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		tracker_query_unref (value->data[0].v_pointer);
	}
}


static void tracker_value_query_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = tracker_query_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer tracker_value_query_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* tracker_value_query_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		TrackerQuery* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = tracker_query_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* tracker_value_query_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	TrackerQuery** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = tracker_query_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* tracker_param_spec_query (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	TrackerParamSpecQuery* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TRACKER_TYPE_QUERY), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer tracker_value_get_query (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_QUERY), NULL);
	return value->data[0].v_pointer;
}


void tracker_value_set_query (GValue* value, gpointer v_object) {
	TrackerQuery* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_QUERY));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TRACKER_TYPE_QUERY));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		tracker_query_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		tracker_query_unref (old);
	}
}


void tracker_value_take_query (GValue* value, gpointer v_object) {
	TrackerQuery* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_QUERY));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TRACKER_TYPE_QUERY));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		tracker_query_unref (old);
	}
}


static void tracker_query_class_init (TrackerQueryClass * klass) {
	tracker_query_parent_class = g_type_class_peek_parent (klass);
	TRACKER_QUERY_CLASS (klass)->finalize = tracker_query_finalize;
	g_type_class_add_private (klass, sizeof (TrackerQueryPrivate));
}


static void tracker_query_instance_init (TrackerQuery * self) {
	self->priv = TRACKER_QUERY_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void tracker_query_finalize (TrackerQuery* obj) {
	TrackerQuery * self;
	self = TRACKER_QUERY (obj);
	_g_free0 (self->priv->_criteria);
	_g_free0 (self->priv->_query);
}


GType tracker_query_get_type (void) {
	static volatile gsize tracker_query_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_query_type_id__volatile)) {
		static const GTypeValueTable g_define_type_value_table = { tracker_value_query_init, tracker_value_query_free_value, tracker_value_query_copy_value, tracker_value_query_peek_pointer, "p", tracker_value_query_collect_value, "p", tracker_value_query_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (TrackerQueryClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_query_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerQuery), 0, (GInstanceInitFunc) tracker_query_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		GType tracker_query_type_id;
		tracker_query_type_id = g_type_register_fundamental (g_type_fundamental_next (), "TrackerQuery", &g_define_type_info, &g_define_type_fundamental_info, 0);
		g_once_init_leave (&tracker_query_type_id__volatile, tracker_query_type_id);
	}
	return tracker_query_type_id__volatile;
}


gpointer tracker_query_ref (gpointer instance) {
	TrackerQuery* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void tracker_query_unref (gpointer instance) {
	TrackerQuery* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		TRACKER_QUERY_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}



