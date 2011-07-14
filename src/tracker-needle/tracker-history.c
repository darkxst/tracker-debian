/* tracker-history.c generated by valac 0.12.1, the Vala compiler
 * generated from tracker-history.vala, do not modify */

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
#include <glib/gstdio.h>
#include <gobject/gvaluecollector.h>


#define TRACKER_TYPE_HISTORY (tracker_history_get_type ())
#define TRACKER_HISTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_HISTORY, TrackerHistory))
#define TRACKER_HISTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_HISTORY, TrackerHistoryClass))
#define TRACKER_IS_HISTORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_HISTORY))
#define TRACKER_IS_HISTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_HISTORY))
#define TRACKER_HISTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_HISTORY, TrackerHistoryClass))

typedef struct _TrackerHistory TrackerHistory;
typedef struct _TrackerHistoryClass TrackerHistoryClass;
typedef struct _TrackerHistoryPrivate TrackerHistoryPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_key_file_free0(var) ((var == NULL) ? NULL : (var = (g_key_file_free (var), NULL)))
typedef struct _TrackerParamSpecHistory TrackerParamSpecHistory;

struct _TrackerHistory {
	GTypeInstance parent_instance;
	volatile int ref_count;
	TrackerHistoryPrivate * priv;
};

struct _TrackerHistoryClass {
	GTypeClass parent_class;
	void (*finalize) (TrackerHistory *self);
};

struct _TrackerHistoryPrivate {
	GKeyFile* data;
	gchar* filename;
	gchar** history;
	gint history_length1;
	gint _history_size_;
};

struct _TrackerParamSpecHistory {
	GParamSpec parent_instance;
};


static gpointer tracker_history_parent_class = NULL;

gpointer tracker_history_ref (gpointer instance);
void tracker_history_unref (gpointer instance);
GParamSpec* tracker_param_spec_history (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void tracker_value_set_history (GValue* value, gpointer v_object);
void tracker_value_take_history (GValue* value, gpointer v_object);
gpointer tracker_value_get_history (const GValue* value);
GType tracker_history_get_type (void) G_GNUC_CONST;
#define TRACKER_HISTORY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TRACKER_TYPE_HISTORY, TrackerHistoryPrivate))
enum  {
	TRACKER_HISTORY_DUMMY_PROPERTY
};
TrackerHistory* tracker_history_new (void);
TrackerHistory* tracker_history_construct (GType object_type);
void tracker_history_add (TrackerHistory* self, const gchar* criteria);
static void _vala_array_add1 (gchar*** array, int* length, int* size, gchar* value);
gchar** tracker_history_get (TrackerHistory* self, int* result_length1);
static gchar** _vala_array_dup1 (gchar** self, int length);
static void tracker_history_finalize (TrackerHistory* obj);
static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);


TrackerHistory* tracker_history_construct (GType object_type) {
	TrackerHistory* self = NULL;
	GKeyFile* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	gchar* _tmp2_ = NULL;
	gboolean _tmp3_;
	gsize _tmp4_;
	gchar** _tmp5_ = NULL;
	gchar** _tmp6_;
	gint _tmp6__length1;
	gint __tmp6__size_;
	GError * _inner_error_ = NULL;
	self = (TrackerHistory*) g_type_create_instance (object_type);
	g_debug ("tracker-history.vala:26: Loading history");
	_tmp0_ = g_key_file_new ();
	_g_key_file_free0 (self->priv->data);
	self->priv->data = _tmp0_;
	_tmp1_ = g_get_user_data_dir ();
	_tmp2_ = g_build_filename (_tmp1_, "tracker", "tracker-needle.txt", NULL, NULL);
	_g_free0 (self->priv->filename);
	self->priv->filename = _tmp2_;
	g_key_file_load_from_file (self->priv->data, self->priv->filename, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &_inner_error_);
	if (_inner_error_ != NULL) {
		if (_inner_error_->domain == G_KEY_FILE_ERROR) {
			goto __catch1_g_key_file_error;
		}
		if (_inner_error_->domain == G_FILE_ERROR) {
			goto __catch1_g_file_error;
		}
		g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	goto __finally1;
	__catch1_g_key_file_error:
	{
		GError * e1;
		e1 = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-history.vala:34: Could not load history from file:'%s': %s", self->priv->filename, e1->message);
		_g_error_free0 (e1);
		return self;
	}
	goto __finally1;
	__catch1_g_file_error:
	{
		GError * e2;
		e2 = _inner_error_;
		_inner_error_ = NULL;
		if (e2->code == 4) {
			g_message ("tracker-history.vala:38: Creating new history file:'%s'", self->priv->filename);
		} else {
			g_warning ("tracker-history.vala:40: Could not load history from file:'%s': %s", self->priv->filename, e2->message);
		}
		_g_error_free0 (e2);
		return self;
	}
	__finally1:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	_tmp3_ = g_key_file_has_group (self->priv->data, "History");
	if (_tmp3_ == FALSE) {
		g_debug ("tracker-history.vala:45:   No history found");
		return self;
	}
	_tmp5_ = g_key_file_get_string_list (self->priv->data, "History", "criteria", &_tmp4_, &_inner_error_);
	_tmp6_ = _tmp5_;
	_tmp6__length1 = _tmp4_;
	__tmp6__size_ = _tmp4_;
	if (_inner_error_ != NULL) {
		if (_inner_error_->domain == G_KEY_FILE_ERROR) {
			goto __catch2_g_key_file_error;
		}
		g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	self->priv->history = (_vala_array_free (self->priv->history, self->priv->history_length1, (GDestroyNotify) g_free), NULL);
	self->priv->history = _tmp6_;
	self->priv->history_length1 = _tmp6__length1;
	self->priv->_history_size_ = _tmp6__length1;
	goto __finally2;
	__catch2_g_key_file_error:
	{
		GError * e1;
		e1 = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-history.vala:52: Could not load history from file:'%s': %s", self->priv->filename, e1->message);
		_g_error_free0 (e1);
		return self;
	}
	__finally2:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	g_debug ("tracker-history.vala:56:   Found %d previous search histories", self->priv->history_length1);
	g_debug ("tracker-history.vala:58:   Done");
	return self;
}


TrackerHistory* tracker_history_new (void) {
	return tracker_history_construct (TRACKER_TYPE_HISTORY);
}


static void _vala_array_add1 (gchar*** array, int* length, int* size, gchar* value) {
	if ((*length) == (*size)) {
		*size = (*size) ? (2 * (*size)) : 4;
		*array = g_renew (gchar*, *array, (*size) + 1);
	}
	(*array)[(*length)++] = value;
	(*array)[*length] = NULL;
}


void tracker_history_add (TrackerHistory* self, const gchar* criteria) {
	gint _tmp0_;
	gchar* _tmp2_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (criteria != NULL);
	_tmp0_ = strlen (criteria);
	g_return_if_fail ((criteria != NULL) && (_tmp0_ > 0));
	{
		gchar** check_collection;
		int check_collection_length1;
		int check_it;
		check_collection = self->priv->history;
		check_collection_length1 = self->priv->history_length1;
		for (check_it = 0; check_it < self->priv->history_length1; check_it = check_it + 1) {
			gchar* _tmp1_;
			gchar* check;
			_tmp1_ = g_strdup (check_collection[check_it]);
			check = _tmp1_;
			{
				if (g_strcmp0 (check, criteria) == 0) {
					_g_free0 (check);
					return;
				}
				_g_free0 (check);
			}
		}
	}
	_tmp2_ = g_strdup (criteria);
	_vala_array_add1 (&self->priv->history, &self->priv->history_length1, &self->priv->_history_size_, _tmp2_);
}


static gchar** _vala_array_dup1 (gchar** self, int length) {
	gchar** result;
	int i;
	result = g_new0 (gchar*, length + 1);
	for (i = 0; i < length; i++) {
		result[i] = g_strdup (self[i]);
	}
	return result;
}


gchar** tracker_history_get (TrackerHistory* self, int* result_length1) {
	gchar** result = NULL;
	gchar** _tmp0_;
	gchar** _tmp1_;
	gchar** _tmp2_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = (_tmp1_ = self->priv->history, (_tmp1_ == NULL) ? ((gpointer) _tmp1_) : _vala_array_dup1 (_tmp1_, self->priv->history_length1));
	_tmp2_ = _tmp0_;
	if (result_length1) {
		*result_length1 = self->priv->history_length1;
	}
	result = _tmp2_;
	return result;
}


static void tracker_value_history_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void tracker_value_history_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		tracker_history_unref (value->data[0].v_pointer);
	}
}


static void tracker_value_history_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = tracker_history_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer tracker_value_history_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* tracker_value_history_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		TrackerHistory* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = tracker_history_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* tracker_value_history_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	TrackerHistory** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = tracker_history_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* tracker_param_spec_history (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	TrackerParamSpecHistory* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TRACKER_TYPE_HISTORY), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer tracker_value_get_history (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_HISTORY), NULL);
	return value->data[0].v_pointer;
}


void tracker_value_set_history (GValue* value, gpointer v_object) {
	TrackerHistory* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_HISTORY));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TRACKER_TYPE_HISTORY));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		tracker_history_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		tracker_history_unref (old);
	}
}


void tracker_value_take_history (GValue* value, gpointer v_object) {
	TrackerHistory* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TRACKER_TYPE_HISTORY));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TRACKER_TYPE_HISTORY));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		tracker_history_unref (old);
	}
}


static void tracker_history_class_init (TrackerHistoryClass * klass) {
	tracker_history_parent_class = g_type_class_peek_parent (klass);
	TRACKER_HISTORY_CLASS (klass)->finalize = tracker_history_finalize;
	g_type_class_add_private (klass, sizeof (TrackerHistoryPrivate));
}


static void tracker_history_instance_init (TrackerHistory * self) {
	self->priv = TRACKER_HISTORY_GET_PRIVATE (self);
	self->ref_count = 1;
}


static void tracker_history_finalize (TrackerHistory* obj) {
	TrackerHistory * self;
	gchar* _tmp0_ = NULL;
	gchar* output;
	GError * _inner_error_ = NULL;
	self = TRACKER_HISTORY (obj);
	g_debug ("tracker-history.vala:62: Saving history");
	g_key_file_set_string_list (self->priv->data, "History", "criteria", (const gchar* const*) self->priv->history, self->priv->history_length1);
	_tmp0_ = g_key_file_to_data (self->priv->data, NULL, NULL);
	output = _tmp0_;
	g_file_set_contents (self->priv->filename, output, (gssize) (-1), &_inner_error_);
	if (_inner_error_ != NULL) {
		_g_free0 (output);
		if (_inner_error_->domain == G_FILE_ERROR) {
			goto __catch0_g_file_error;
		}
		_g_free0 (output);
		g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
	}
	_g_free0 (output);
	goto __finally0;
	__catch0_g_file_error:
	{
		GError * e1;
		e1 = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("tracker-history.vala:71: Could not save history to file:'%s': %s", self->priv->filename, e1->message);
		_g_error_free0 (e1);
	}
	__finally0:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
	}
	g_debug ("tracker-history.vala:74:   Done");
	_g_key_file_free0 (self->priv->data);
	_g_free0 (self->priv->filename);
	self->priv->history = (_vala_array_free (self->priv->history, self->priv->history_length1, (GDestroyNotify) g_free), NULL);
}


GType tracker_history_get_type (void) {
	static volatile gsize tracker_history_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_history_type_id__volatile)) {
		static const GTypeValueTable g_define_type_value_table = { tracker_value_history_init, tracker_value_history_free_value, tracker_value_history_copy_value, tracker_value_history_peek_pointer, "p", tracker_value_history_collect_value, "p", tracker_value_history_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (TrackerHistoryClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_history_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerHistory), 0, (GInstanceInitFunc) tracker_history_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		GType tracker_history_type_id;
		tracker_history_type_id = g_type_register_fundamental (g_type_fundamental_next (), "TrackerHistory", &g_define_type_info, &g_define_type_fundamental_info, 0);
		g_once_init_leave (&tracker_history_type_id__volatile, tracker_history_type_id);
	}
	return tracker_history_type_id__volatile;
}


gpointer tracker_history_ref (gpointer instance) {
	TrackerHistory* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void tracker_history_unref (gpointer instance) {
	TrackerHistory* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		TRACKER_HISTORY_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
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



