/* tracker-bus-fd-cursor.c generated by valac 0.11.5, the Vala compiler
 * generated from tracker-bus-fd-cursor.vala, do not modify */

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
#include <libtracker-sparql/tracker-sparql.h>
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>


#define TRACKER_BUS_TYPE_FD_CURSOR (tracker_bus_fd_cursor_get_type ())
#define TRACKER_BUS_FD_CURSOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_BUS_TYPE_FD_CURSOR, TrackerBusFDCursor))
#define TRACKER_BUS_FD_CURSOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_BUS_TYPE_FD_CURSOR, TrackerBusFDCursorClass))
#define TRACKER_BUS_IS_FD_CURSOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_BUS_TYPE_FD_CURSOR))
#define TRACKER_BUS_IS_FD_CURSOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_BUS_TYPE_FD_CURSOR))
#define TRACKER_BUS_FD_CURSOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_BUS_TYPE_FD_CURSOR, TrackerBusFDCursorClass))

typedef struct _TrackerBusFDCursor TrackerBusFDCursor;
typedef struct _TrackerBusFDCursorClass TrackerBusFDCursorClass;
typedef struct _TrackerBusFDCursorPrivate TrackerBusFDCursorPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
typedef struct _TrackerBusFdCursorNextAsyncData TrackerBusFdCursorNextAsyncData;

struct _TrackerBusFDCursor {
	TrackerSparqlCursor parent_instance;
	TrackerBusFDCursorPrivate * priv;
	gchar* buffer;
	gulong buffer_index;
	gulong buffer_size;
	gint _n_columns;
	gint* offsets;
	gint* types;
	gchar* data;
	gchar** variable_names;
	gint variable_names_length1;
	gint _variable_names_size_;
};

struct _TrackerBusFDCursorClass {
	TrackerSparqlCursorClass parent_class;
};

struct _TrackerBusFdCursorNextAsyncData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	TrackerBusFDCursor* self;
	GCancellable* cancellable;
	gboolean result;
	gboolean _tmp0_;
	gboolean _tmp1_;
	GError * _inner_error_;
};


static gpointer tracker_bus_fd_cursor_parent_class = NULL;

GType tracker_bus_fd_cursor_get_type (void) G_GNUC_CONST;
enum  {
	TRACKER_BUS_FD_CURSOR_DUMMY_PROPERTY,
	TRACKER_BUS_FD_CURSOR_N_COLUMNS
};
TrackerBusFDCursor* tracker_bus_fd_cursor_new (gchar* buffer, gulong buffer_size, gchar** variable_names, int variable_names_length1);
TrackerBusFDCursor* tracker_bus_fd_cursor_construct (GType object_type, gchar* buffer, gulong buffer_size, gchar** variable_names, int variable_names_length1);
static gchar** _vala_array_dup4 (gchar** self, int length);
static inline gint tracker_bus_fd_cursor_buffer_read_int (TrackerBusFDCursor* self);
static TrackerSparqlValueType tracker_bus_fd_cursor_real_get_value_type (TrackerSparqlCursor* base, gint column);
static const gchar* tracker_bus_fd_cursor_real_get_variable_name (TrackerSparqlCursor* base, gint column);
static const gchar* tracker_bus_fd_cursor_real_get_string (TrackerSparqlCursor* base, gint column, glong* length);
static gboolean tracker_bus_fd_cursor_real_next (TrackerSparqlCursor* base, GCancellable* cancellable, GError** error);
static void tracker_bus_fd_cursor_real_next_async_data_free (gpointer _data);
static void tracker_bus_fd_cursor_real_next_async (TrackerSparqlCursor* base, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
static gboolean tracker_bus_fd_cursor_real_next_async_co (TrackerBusFdCursorNextAsyncData* data);
static void tracker_bus_fd_cursor_real_rewind (TrackerSparqlCursor* base);
static void tracker_bus_fd_cursor_finalize (GObject* obj);
static void _vala_tracker_bus_fd_cursor_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);


static gchar** _vala_array_dup4 (gchar** self, int length) {
	gchar** result;
	int i;
	result = g_new0 (gchar*, length + 1);
	for (i = 0; i < length; i++) {
		result[i] = g_strdup (self[i]);
	}
	return result;
}


TrackerBusFDCursor* tracker_bus_fd_cursor_construct (GType object_type, gchar* buffer, gulong buffer_size, gchar** variable_names, int variable_names_length1) {
	TrackerBusFDCursor * self = NULL;
	gchar** _tmp0_;
	gchar** _tmp1_;
	gchar** _tmp2_;
	self = (TrackerBusFDCursor*) tracker_sparql_cursor_construct (object_type);
	self->buffer = buffer;
	self->buffer_size = buffer_size;
	_tmp0_ = (_tmp1_ = variable_names, (_tmp1_ == NULL) ? ((gpointer) _tmp1_) : _vala_array_dup4 (_tmp1_, variable_names_length1));
	_tmp2_ = _tmp0_;
	self->variable_names = (_vala_array_free (self->variable_names, self->variable_names_length1, (GDestroyNotify) g_free), NULL);
	self->variable_names_length1 = variable_names_length1;
	self->_variable_names_size_ = self->variable_names_length1;
	self->variable_names = _tmp2_;
	self->_n_columns = variable_names_length1;
	return self;
}


TrackerBusFDCursor* tracker_bus_fd_cursor_new (gchar* buffer, gulong buffer_size, gchar** variable_names, int variable_names_length1) {
	return tracker_bus_fd_cursor_construct (TRACKER_BUS_TYPE_FD_CURSOR, buffer, buffer_size, variable_names, variable_names_length1);
}


static inline gint tracker_bus_fd_cursor_buffer_read_int (TrackerBusFDCursor* self) {
	gint result = 0;
	gint v;
	g_return_val_if_fail (self != NULL, 0);
	v = *((gint*) (self->buffer + self->buffer_index));
	self->buffer_index = self->buffer_index + ((gulong) 4);
	result = v;
	return result;
}


static TrackerSparqlValueType tracker_bus_fd_cursor_real_get_value_type (TrackerSparqlCursor* base, gint column) {
	TrackerBusFDCursor * self;
	TrackerSparqlValueType result = 0;
	self = (TrackerBusFDCursor*) base;
	g_return_val_if_fail (self->types != NULL, 0);
	result = (TrackerSparqlValueType) self->types[column];
	return result;
}


static const gchar* tracker_bus_fd_cursor_real_get_variable_name (TrackerSparqlCursor* base, gint column) {
	TrackerBusFDCursor * self;
	const gchar* result = NULL;
	self = (TrackerBusFDCursor*) base;
	g_return_val_if_fail (self->variable_names != NULL, NULL);
	result = self->variable_names[column];
	return result;
}


static const gchar* tracker_bus_fd_cursor_real_get_string (TrackerSparqlCursor* base, gint column, glong* length) {
	TrackerBusFDCursor * self;
	glong _length = 0L;
	const gchar* result = NULL;
	gint _tmp0_;
	const gchar* str;
	gint _tmp1_;
	self = (TrackerBusFDCursor*) base;
	_tmp0_ = tracker_sparql_cursor_get_n_columns ((TrackerSparqlCursor*) self);
	g_return_val_if_fail ((column < _tmp0_) && (self->data != NULL), NULL);
	str = NULL;
	if (self->types[column] == TRACKER_SPARQL_VALUE_TYPE_UNBOUND) {
		_length = (glong) 0;
		result = NULL;
		if (length) {
			*length = _length;
		}
		return result;
	}
	if (column == 0) {
		str = (const gchar*) self->data;
	} else {
		str = (const gchar*) ((self->data + self->offsets[column - 1]) + 1);
	}
	_tmp1_ = strlen (str);
	_length = (glong) _tmp1_;
	result = str;
	if (length) {
		*length = _length;
	}
	return result;
}


static gboolean tracker_bus_fd_cursor_real_next (TrackerSparqlCursor* base, GCancellable* cancellable, GError** error) {
	TrackerBusFDCursor * self;
	gboolean result = FALSE;
	gint last_offset = 0;
	gint _tmp0_;
	gint _tmp1_;
	gint _tmp2_;
	gint _tmp3_;
	self = (TrackerBusFDCursor*) base;
	if (self->buffer_index >= self->buffer_size) {
		result = FALSE;
		return result;
	}
	_tmp0_ = tracker_bus_fd_cursor_buffer_read_int (self);
	self->_n_columns = _tmp0_;
	self->types = (gint*) (self->buffer + self->buffer_index);
	_tmp1_ = tracker_sparql_cursor_get_n_columns ((TrackerSparqlCursor*) self);
	self->buffer_index = self->buffer_index + (sizeof (gint) * _tmp1_);
	self->offsets = (gint*) (self->buffer + self->buffer_index);
	_tmp2_ = tracker_sparql_cursor_get_n_columns ((TrackerSparqlCursor*) self);
	self->buffer_index = self->buffer_index + (sizeof (gint) * (_tmp2_ - 1));
	_tmp3_ = tracker_bus_fd_cursor_buffer_read_int (self);
	last_offset = _tmp3_;
	self->data = self->buffer + self->buffer_index;
	self->buffer_index = self->buffer_index + ((gulong) (last_offset + 1));
	result = TRUE;
	return result;
}


static void tracker_bus_fd_cursor_real_next_async_data_free (gpointer _data) {
	TrackerBusFdCursorNextAsyncData* data;
	data = _data;
	_g_object_unref0 (data->cancellable);
	_g_object_unref0 (data->self);
	g_slice_free (TrackerBusFdCursorNextAsyncData, data);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void tracker_bus_fd_cursor_real_next_async (TrackerSparqlCursor* base, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	TrackerBusFDCursor * self;
	TrackerBusFdCursorNextAsyncData* _data_;
	self = (TrackerBusFDCursor*) base;
	_data_ = g_slice_new0 (TrackerBusFdCursorNextAsyncData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, tracker_bus_fd_cursor_real_next_async);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, tracker_bus_fd_cursor_real_next_async_data_free);
	_data_->self = _g_object_ref0 (self);
	_data_->cancellable = _g_object_ref0 (cancellable);
	tracker_bus_fd_cursor_real_next_async_co (_data_);
}


static gboolean tracker_bus_fd_cursor_real_next_finish (TrackerSparqlCursor* base, GAsyncResult* _res_, GError** error) {
	gboolean result;
	TrackerBusFdCursorNextAsyncData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return FALSE;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	return result;
}


static gboolean tracker_bus_fd_cursor_real_next_async_co (TrackerBusFdCursorNextAsyncData* data) {
	switch (data->_state_) {
		case 0:
		goto _state_0;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	data->_tmp0_ = tracker_sparql_cursor_next ((TrackerSparqlCursor*) data->self, data->cancellable, &data->_inner_error_);
	data->_tmp1_ = data->_tmp0_;
	if (data->_inner_error_ != NULL) {
		g_simple_async_result_set_from_error (data->_async_result, data->_inner_error_);
		g_error_free (data->_inner_error_);
		if (data->_state_ == 0) {
			g_simple_async_result_complete_in_idle (data->_async_result);
		} else {
			g_simple_async_result_complete (data->_async_result);
		}
		g_object_unref (data->_async_result);
		return FALSE;
	}
	data->result = data->_tmp1_;
	if (data->_state_ == 0) {
		g_simple_async_result_complete_in_idle (data->_async_result);
	} else {
		g_simple_async_result_complete (data->_async_result);
	}
	g_object_unref (data->_async_result);
	return FALSE;
	if (data->_state_ == 0) {
		g_simple_async_result_complete_in_idle (data->_async_result);
	} else {
		g_simple_async_result_complete (data->_async_result);
	}
	g_object_unref (data->_async_result);
	return FALSE;
}


static void tracker_bus_fd_cursor_real_rewind (TrackerSparqlCursor* base) {
	TrackerBusFDCursor * self;
	self = (TrackerBusFDCursor*) base;
	self->buffer_index = (gulong) 0;
	self->data = self->buffer;
}


static gint tracker_bus_fd_cursor_real_get_n_columns (TrackerSparqlCursor* base) {
	gint result;
	TrackerBusFDCursor* self;
	self = (TrackerBusFDCursor*) base;
	result = self->_n_columns;
	return result;
}


static void tracker_bus_fd_cursor_class_init (TrackerBusFDCursorClass * klass) {
	tracker_bus_fd_cursor_parent_class = g_type_class_peek_parent (klass);
	TRACKER_SPARQL_CURSOR_CLASS (klass)->get_value_type = tracker_bus_fd_cursor_real_get_value_type;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->get_variable_name = tracker_bus_fd_cursor_real_get_variable_name;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->get_string = tracker_bus_fd_cursor_real_get_string;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->next = tracker_bus_fd_cursor_real_next;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->next_async = tracker_bus_fd_cursor_real_next_async;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->next_finish = tracker_bus_fd_cursor_real_next_finish;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->rewind = tracker_bus_fd_cursor_real_rewind;
	TRACKER_SPARQL_CURSOR_CLASS (klass)->get_n_columns = tracker_bus_fd_cursor_real_get_n_columns;
	G_OBJECT_CLASS (klass)->get_property = _vala_tracker_bus_fd_cursor_get_property;
	G_OBJECT_CLASS (klass)->finalize = tracker_bus_fd_cursor_finalize;
	g_object_class_override_property (G_OBJECT_CLASS (klass), TRACKER_BUS_FD_CURSOR_N_COLUMNS, "n-columns");
}


static void tracker_bus_fd_cursor_instance_init (TrackerBusFDCursor * self) {
}


static void tracker_bus_fd_cursor_finalize (GObject* obj) {
	TrackerBusFDCursor * self;
	self = TRACKER_BUS_FD_CURSOR (obj);
	g_free (self->buffer);
	self->variable_names = (_vala_array_free (self->variable_names, self->variable_names_length1, (GDestroyNotify) g_free), NULL);
	G_OBJECT_CLASS (tracker_bus_fd_cursor_parent_class)->finalize (obj);
}


GType tracker_bus_fd_cursor_get_type (void) {
	static volatile gsize tracker_bus_fd_cursor_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_bus_fd_cursor_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TrackerBusFDCursorClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_bus_fd_cursor_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerBusFDCursor), 0, (GInstanceInitFunc) tracker_bus_fd_cursor_instance_init, NULL };
		GType tracker_bus_fd_cursor_type_id;
		tracker_bus_fd_cursor_type_id = g_type_register_static (TRACKER_SPARQL_TYPE_CURSOR, "TrackerBusFDCursor", &g_define_type_info, 0);
		g_once_init_leave (&tracker_bus_fd_cursor_type_id__volatile, tracker_bus_fd_cursor_type_id);
	}
	return tracker_bus_fd_cursor_type_id__volatile;
}


static void _vala_tracker_bus_fd_cursor_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	TrackerBusFDCursor * self;
	self = TRACKER_BUS_FD_CURSOR (object);
	switch (property_id) {
		case TRACKER_BUS_FD_CURSOR_N_COLUMNS:
		g_value_set_int (value, tracker_sparql_cursor_get_n_columns ((TrackerSparqlCursor*) self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	if ((array != NULL) && (destroy_func != NULL)) {
		int i;
		for (i = 0; i < array_length; i = i + 1) {
			if (((gpointer*) array)[i] != NULL) {
				destroy_func (((gpointer*) array)[i]);
			}
		}
	}
}


static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func) {
	_vala_array_destroy (array, array_length, destroy_func);
	g_free (array);
}



