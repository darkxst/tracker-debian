/* tracker-taglist.c generated by valac 0.12.0, the Vala compiler
 * generated from tracker-taglist.vala, do not modify */

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
#include <gtk/gtk.h>
#include <libtracker-sparql/tracker-sparql.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n-lib.h>
#include <gio/gio.h>


#define TRACKER_TYPE_TAG_LIST (tracker_tag_list_get_type ())
#define TRACKER_TAG_LIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_TAG_LIST, TrackerTagList))
#define TRACKER_TAG_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_TAG_LIST, TrackerTagListClass))
#define TRACKER_IS_TAG_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_TAG_LIST))
#define TRACKER_IS_TAG_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_TAG_LIST))
#define TRACKER_TAG_LIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_TAG_LIST, TrackerTagListClass))

typedef struct _TrackerTagList TrackerTagList;
typedef struct _TrackerTagListClass TrackerTagListClass;
typedef struct _TrackerTagListPrivate TrackerTagListPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define TRACKER_TYPE_CELL_RENDERER_TEXT (tracker_cell_renderer_text_get_type ())
#define TRACKER_CELL_RENDERER_TEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_CELL_RENDERER_TEXT, TrackerCellRendererText))
#define TRACKER_CELL_RENDERER_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_CELL_RENDERER_TEXT, TrackerCellRendererTextClass))
#define TRACKER_IS_CELL_RENDERER_TEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_CELL_RENDERER_TEXT))
#define TRACKER_IS_CELL_RENDERER_TEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_CELL_RENDERER_TEXT))
#define TRACKER_CELL_RENDERER_TEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_CELL_RENDERER_TEXT, TrackerCellRendererTextClass))

typedef struct _TrackerCellRendererText TrackerCellRendererText;
typedef struct _TrackerCellRendererTextClass TrackerCellRendererTextClass;
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
typedef struct _TrackerTagListGetTagsData TrackerTagListGetTagsData;

struct _TrackerTagList {
	GtkScrolledWindow parent_instance;
	TrackerTagListPrivate * priv;
};

struct _TrackerTagListClass {
	GtkScrolledWindowClass parent_class;
};

struct _TrackerTagListPrivate {
	GtkTreeView* treeview;
	GtkListStore* store;
	gint offset;
	gint limit;
};

struct _TrackerTagListGetTagsData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	TrackerTagList* self;
	gchar* _tmp0_;
	gchar* _tmp1_;
	gchar* _tmp2_;
	gchar* _tmp3_;
	gchar* _tmp4_;
	gchar* _tmp5_;
	gchar* query;
	TrackerSparqlCursor* cursor;
	TrackerSparqlCursor* _tmp6_;
	TrackerSparqlCursor* _tmp7_;
	gboolean _tmp8_;
	gboolean _tmp9_;
	gint i;
	gboolean _tmp10_;
	gint _tmp11_;
	const gchar* _tmp12_;
	const gchar* _tmp13_;
	GtkTreeIter iter;
	GtkTreeIter _tmp14_;
	const gchar* _tmp15_;
	const gchar* _tmp16_;
	const gchar* _tmp17_;
	GError * e;
	GError * _inner_error_;
};


static gpointer tracker_tag_list_parent_class = NULL;
static TrackerSparqlConnection* tracker_tag_list_connection;
static TrackerSparqlConnection* tracker_tag_list_connection = NULL;

GType tracker_tag_list_get_type (void) G_GNUC_CONST;
#define TRACKER_TAG_LIST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TRACKER_TYPE_TAG_LIST, TrackerTagListPrivate))
enum  {
	TRACKER_TAG_LIST_DUMMY_PROPERTY
};
TrackerTagList* tracker_tag_list_new (void);
TrackerTagList* tracker_tag_list_construct (GType object_type);
TrackerCellRendererText* tracker_cell_renderer_text_new (void);
TrackerCellRendererText* tracker_cell_renderer_text_construct (GType object_type);
GType tracker_cell_renderer_text_get_type (void) G_GNUC_CONST;
static void tracker_tag_list_get_tags (TrackerTagList* self, GAsyncReadyCallback _callback_, gpointer _user_data_);
static void tracker_tag_list_get_tags_finish (TrackerTagList* self, GAsyncResult* _res_);
static void tracker_tag_list_get_tags_data_free (gpointer _data);
static gboolean tracker_tag_list_get_tags_co (TrackerTagListGetTagsData* data);
static void tracker_tag_list_get_tags_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_);
static void tracker_tag_list_finalize (GObject* obj);


TrackerTagList* tracker_tag_list_construct (GType object_type) {
	TrackerTagList * self = NULL;
	GtkListStore* _tmp0_ = NULL;
	GtkTreeView* _tmp1_ = NULL;
	GtkTreeViewColumn* col = NULL;
	GtkCellRenderer* renderer = NULL;
	GtkTreeViewColumn* _tmp2_ = NULL;
	const gchar* _tmp3_ = NULL;
	TrackerCellRendererText* _tmp4_ = NULL;
	TrackerCellRendererText* _tmp5_ = NULL;
	TrackerSparqlConnection* _tmp6_ = NULL;
	TrackerSparqlConnection* _tmp7_;
	GError * _inner_error_ = NULL;
	self = (TrackerTagList*) g_object_new (object_type, NULL);
	self->priv->limit = 100;
	gtk_scrolled_window_set_policy ((GtkScrolledWindow*) self, GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request ((GtkWidget*) self, 175, -1);
	_tmp0_ = gtk_list_store_new (4, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	_g_object_unref0 (self->priv->store);
	self->priv->store = _tmp0_;
	_tmp1_ = (GtkTreeView*) gtk_tree_view_new_with_model ((GtkTreeModel*) self->priv->store);
	_g_object_unref0 (self->priv->treeview);
	self->priv->treeview = g_object_ref_sink (_tmp1_);
	gtk_tree_view_set_headers_visible (self->priv->treeview, TRUE);
	_tmp2_ = gtk_tree_view_column_new ();
	_g_object_unref0 (col);
	col = g_object_ref_sink (_tmp2_);
	_tmp3_ = _ ("Tags");
	gtk_tree_view_column_set_title (col, _tmp3_);
	gtk_tree_view_column_set_resizable (col, TRUE);
	gtk_tree_view_column_set_expand (col, TRUE);
	gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	_tmp4_ = tracker_cell_renderer_text_new ();
	_g_object_unref0 (renderer);
	renderer = (GtkCellRenderer*) g_object_ref_sink (_tmp4_);
	gtk_cell_layout_pack_start ((GtkCellLayout*) col, renderer, TRUE);
	gtk_cell_layout_add_attribute ((GtkCellLayout*) col, renderer, "text", 1);
	gtk_cell_layout_add_attribute ((GtkCellLayout*) col, renderer, "subtext", 2);
	_tmp5_ = tracker_cell_renderer_text_new ();
	_g_object_unref0 (renderer);
	renderer = (GtkCellRenderer*) g_object_ref_sink (_tmp5_);
	g_object_set (renderer, "xpad", (guint) 5, NULL);
	g_object_set (renderer, "ypad", (guint) 5, NULL);
	gtk_cell_layout_pack_end ((GtkCellLayout*) col, renderer, FALSE);
	gtk_cell_layout_add_attribute ((GtkCellLayout*) col, renderer, "text", 3);
	gtk_tree_view_append_column (self->priv->treeview, col);
	gtk_container_add ((GtkContainer*) self, (GtkWidget*) self->priv->treeview);
	GTK_WIDGET_CLASS (tracker_tag_list_parent_class)->show_all ((GtkWidget*) GTK_SCROLLED_WINDOW (self));
	_tmp6_ = tracker_sparql_connection_get (NULL, &_inner_error_);
	_tmp7_ = _tmp6_;
	if (_inner_error_ != NULL) {
		goto __catch13_g_error;
	}
	_g_object_unref0 (tracker_tag_list_connection);
	tracker_tag_list_connection = _tmp7_;
	goto __finally13;
	__catch13_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-taglist.vala:81: Could not get Sparql connection: %s", e->message);
		_g_error_free0 (e);
		_g_object_unref0 (renderer);
		_g_object_unref0 (col);
		return self;
	}
	__finally13:
	if (_inner_error_ != NULL) {
		_g_object_unref0 (renderer);
		_g_object_unref0 (col);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	tracker_tag_list_get_tags (self, NULL, NULL);
	_g_object_unref0 (renderer);
	_g_object_unref0 (col);
	return self;
}


TrackerTagList* tracker_tag_list_new (void) {
	return tracker_tag_list_construct (TRACKER_TYPE_TAG_LIST);
}


static void tracker_tag_list_get_tags_data_free (gpointer _data) {
	TrackerTagListGetTagsData* data;
	data = _data;
	_g_object_unref0 (data->self);
	g_slice_free (TrackerTagListGetTagsData, data);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void tracker_tag_list_get_tags (TrackerTagList* self, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	TrackerTagListGetTagsData* _data_;
	_data_ = g_slice_new0 (TrackerTagListGetTagsData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, tracker_tag_list_get_tags);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, tracker_tag_list_get_tags_data_free);
	_data_->self = _g_object_ref0 (self);
	tracker_tag_list_get_tags_co (_data_);
}


static void tracker_tag_list_get_tags_finish (TrackerTagList* self, GAsyncResult* _res_) {
	TrackerTagListGetTagsData* _data_;
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
}


static void tracker_tag_list_get_tags_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_) {
	TrackerTagListGetTagsData* data;
	data = _user_data_;
	data->_source_object_ = source_object;
	data->_res_ = _res_;
	tracker_tag_list_get_tags_co (data);
}


static gboolean tracker_tag_list_get_tags_co (TrackerTagListGetTagsData* data) {
	switch (data->_state_) {
		case 0:
		goto _state_0;
		case 1:
		goto _state_1;
		case 2:
		goto _state_2;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	data->_tmp0_ = NULL;
	data->_tmp0_ = g_strdup_printf ("%i", data->self->priv->offset);
	data->_tmp1_ = data->_tmp0_;
	data->_tmp2_ = NULL;
	data->_tmp2_ = g_strdup_printf ("%i", data->self->priv->limit);
	data->_tmp3_ = data->_tmp2_;
	data->_tmp4_ = NULL;
	data->_tmp4_ = g_strconcat ("\n" \
"		               SELECT \n" \
"		                 ?tag \n" \
"		                 ?label\n" \
"		                 nao:description(?tag)\n" \
"		                 COUNT(?urns) AS urns\n" \
"		               WHERE {\n" \
"		                 ?tag a nao:Tag ;\n" \
"		                 nao:prefLabel ?label .\n" \
"		                 OPTIONAL {\n" \
"		                   ?urns nao:hasTag ?tag\n" \
"		                 }\n" \
"		               } \n" \
"		               GROUP BY ?tag \n" \
"		               ORDER BY ASC(?label) \n" \
"		               OFFSET ", data->_tmp1_, " \n		               LIMIT ", data->_tmp3_, "\n		               ", NULL);
	data->_tmp5_ = data->_tmp4_;
	_g_free0 (data->_tmp3_);
	_g_free0 (data->_tmp1_);
	data->query = data->_tmp5_;
	g_debug ("tracker-taglist.vala:108: Getting tags");
	data->cursor = NULL;
	data->_state_ = 1;
	tracker_sparql_connection_query_async (tracker_tag_list_connection, data->query, NULL, tracker_tag_list_get_tags_ready, data);
	return FALSE;
	_state_1:
	data->_tmp6_ = NULL;
	data->_tmp6_ = tracker_sparql_connection_query_finish (tracker_tag_list_connection, data->_res_, &data->_inner_error_);
	data->_tmp7_ = data->_tmp6_;
	if (data->_inner_error_ != NULL) {
		goto __catch14_g_error;
	}
	_g_object_unref0 (data->cursor);
	data->cursor = data->_tmp7_;
	while (TRUE) {
		data->_state_ = 2;
		tracker_sparql_cursor_next_async (data->cursor, NULL, tracker_tag_list_get_tags_ready, data);
		return FALSE;
		_state_2:
		data->_tmp8_ = tracker_sparql_cursor_next_finish (data->cursor, data->_res_, &data->_inner_error_);
		data->_tmp9_ = data->_tmp8_;
		if (data->_inner_error_ != NULL) {
			goto __catch14_g_error;
		}
		if (!data->_tmp9_) {
			break;
		}
		{
			data->i = 0;
			{
				data->_tmp10_ = TRUE;
				while (TRUE) {
					if (!data->_tmp10_) {
						data->i++;
					}
					data->_tmp10_ = FALSE;
					data->_tmp11_ = tracker_sparql_cursor_get_n_columns (data->cursor);
					if (!(data->i < data->_tmp11_)) {
						break;
					}
					if (data->i == 0) {
						data->_tmp12_ = NULL;
						data->_tmp12_ = tracker_sparql_cursor_get_string (data->cursor, data->i, NULL);
						g_debug ("tracker-taglist.vala:118: --> %s", data->_tmp12_);
					} else {
						data->_tmp13_ = NULL;
						data->_tmp13_ = tracker_sparql_cursor_get_string (data->cursor, data->i, NULL);
						g_debug ("tracker-taglist.vala:120:   --> %s", data->_tmp13_);
					}
				}
			}
		}
		memset (&data->_tmp14_, 0, sizeof (GtkTreeIter));
		gtk_list_store_append (data->self->priv->store, &data->_tmp14_);
		data->iter = data->_tmp14_;
		data->_tmp15_ = NULL;
		data->_tmp15_ = tracker_sparql_cursor_get_string (data->cursor, 1, NULL);
		data->_tmp16_ = NULL;
		data->_tmp16_ = tracker_sparql_cursor_get_string (data->cursor, 2, NULL);
		data->_tmp17_ = NULL;
		data->_tmp17_ = tracker_sparql_cursor_get_string (data->cursor, 3, NULL);
		gtk_list_store_set (data->self->priv->store, &data->iter, 0, FALSE, 1, data->_tmp15_, 2, data->_tmp16_, 3, data->_tmp17_, -1, -1);
	}
	goto __finally14;
	__catch14_g_error:
	{
		data->e = data->_inner_error_;
		data->_inner_error_ = NULL;
		g_warning ("tracker-taglist.vala:136: Could not run Sparql query: %s", data->e->message);
		_g_error_free0 (data->e);
	}
	__finally14:
	if (data->_inner_error_ != NULL) {
		_g_object_unref0 (data->cursor);
		_g_free0 (data->query);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, data->_inner_error_->message, g_quark_to_string (data->_inner_error_->domain), data->_inner_error_->code);
		g_clear_error (&data->_inner_error_);
		return FALSE;
	}
	g_debug ("tracker-taglist.vala:139:   Done");
	_g_object_unref0 (data->cursor);
	_g_free0 (data->query);
	if (data->_state_ == 0) {
		g_simple_async_result_complete_in_idle (data->_async_result);
	} else {
		g_simple_async_result_complete (data->_async_result);
	}
	g_object_unref (data->_async_result);
	return FALSE;
}


static void tracker_tag_list_class_init (TrackerTagListClass * klass) {
	tracker_tag_list_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (TrackerTagListPrivate));
	G_OBJECT_CLASS (klass)->finalize = tracker_tag_list_finalize;
}


static void tracker_tag_list_instance_init (TrackerTagList * self) {
	self->priv = TRACKER_TAG_LIST_GET_PRIVATE (self);
}


static void tracker_tag_list_finalize (GObject* obj) {
	TrackerTagList * self;
	self = TRACKER_TAG_LIST (obj);
	_g_object_unref0 (self->priv->treeview);
	_g_object_unref0 (self->priv->store);
	G_OBJECT_CLASS (tracker_tag_list_parent_class)->finalize (obj);
}


GType tracker_tag_list_get_type (void) {
	static volatile gsize tracker_tag_list_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_tag_list_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TrackerTagListClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_tag_list_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerTagList), 0, (GInstanceInitFunc) tracker_tag_list_instance_init, NULL };
		GType tracker_tag_list_type_id;
		tracker_tag_list_type_id = g_type_register_static (GTK_TYPE_SCROLLED_WINDOW, "TrackerTagList", &g_define_type_info, 0);
		g_once_init_leave (&tracker_tag_list_type_id__volatile, tracker_tag_list_type_id);
	}
	return tracker_tag_list_type_id__volatile;
}



