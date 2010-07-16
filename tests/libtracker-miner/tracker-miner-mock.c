/* tracker-miner-mock.c generated by valac, the Vala compiler
 * generated from tracker-miner-mock.vala, do not modify */

/**/
/* Copyright (C) 2010, Nokia*/
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
#include <float.h>
#include <math.h>


#define TYPE_TRACKER_MINER_MOCK (tracker_miner_mock_get_type ())
#define TRACKER_MINER_MOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRACKER_MINER_MOCK, TrackerMinerMock))
#define TRACKER_MINER_MOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRACKER_MINER_MOCK, TrackerMinerMockClass))
#define IS_TRACKER_MINER_MOCK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRACKER_MINER_MOCK))
#define IS_TRACKER_MINER_MOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRACKER_MINER_MOCK))
#define TRACKER_MINER_MOCK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRACKER_MINER_MOCK, TrackerMinerMockClass))

typedef struct _TrackerMinerMock TrackerMinerMock;
typedef struct _TrackerMinerMockClass TrackerMinerMockClass;
typedef struct _TrackerMinerMockPrivate TrackerMinerMockPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))

struct _TrackerMinerMock {
	GObject parent_instance;
	TrackerMinerMockPrivate * priv;
	gboolean is_paused;
};

struct _TrackerMinerMockClass {
	GObjectClass parent_class;
};

struct _TrackerMinerMockPrivate {
	char* _pause_reason;
	char* _name;
	char** _apps;
	gint _apps_length1;
	gint __apps_size_;
	char** _reasons;
	gint _reasons_length1;
	gint __reasons_size_;
};


static gpointer tracker_miner_mock_parent_class = NULL;

GType tracker_miner_mock_get_type (void) G_GNUC_CONST;
#define TRACKER_MINER_MOCK_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_TRACKER_MINER_MOCK, TrackerMinerMockPrivate))
enum  {
	TRACKER_MINER_MOCK_DUMMY_PROPERTY,
	TRACKER_MINER_MOCK_PAUSE_REASON,
	TRACKER_MINER_MOCK_NAME,
	TRACKER_MINER_MOCK_APPS,
	TRACKER_MINER_MOCK_REASONS
};
void tracker_miner_mock_set_name (TrackerMinerMock* self, const char* value);
TrackerMinerMock* tracker_miner_mock_new (const char* name);
TrackerMinerMock* tracker_miner_mock_construct (GType object_type, const char* name);
void tracker_miner_mock_set_paused (TrackerMinerMock* self, gboolean paused);
gboolean tracker_miner_mock_get_paused (TrackerMinerMock* self);
static void _vala_array_add1 (char*** array, int* length, int* size, char* value);
static void _vala_array_add2 (char*** array, int* length, int* size, char* value);
void tracker_miner_mock_pause (TrackerMinerMock* self, const char* app, const char* reason);
void tracker_miner_mock_resume (TrackerMinerMock* self);
const char* tracker_miner_mock_get_pause_reason (TrackerMinerMock* self);
void tracker_miner_mock_set_pause_reason (TrackerMinerMock* self, const char* value);
const char* tracker_miner_mock_get_name (TrackerMinerMock* self);
char** tracker_miner_mock_get_apps (TrackerMinerMock* self, int* result_length1);
char** tracker_miner_mock_get_reasons (TrackerMinerMock* self, int* result_length1);
static void tracker_miner_mock_finalize (GObject* obj);
static void tracker_miner_mock_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void tracker_miner_mock_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);
static void _vala_array_destroy (gpointer array, gint array_length, GDestroyNotify destroy_func);
static void _vala_array_free (gpointer array, gint array_length, GDestroyNotify destroy_func);


static void g_cclosure_user_marshal_VOID__STRING_STRING_DOUBLE (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data);

TrackerMinerMock* tracker_miner_mock_construct (GType object_type, const char* name) {
	TrackerMinerMock * self;
	char** _tmp1_;
	char** _tmp0_ = NULL;
	char** _tmp3_;
	char** _tmp2_ = NULL;
	g_return_val_if_fail (name != NULL, NULL);
	self = (TrackerMinerMock*) g_object_new (object_type, NULL);
	tracker_miner_mock_set_name (self, name);
	self->priv->_apps = (_tmp1_ = (_tmp0_ = g_new0 (char*, 0 + 1), _tmp0_), self->priv->_apps = (_vala_array_free (self->priv->_apps, self->priv->_apps_length1, (GDestroyNotify) g_free), NULL), self->priv->_apps_length1 = 0, self->priv->__apps_size_ = self->priv->_apps_length1, _tmp1_);
	self->priv->_reasons = (_tmp3_ = (_tmp2_ = g_new0 (char*, 0 + 1), _tmp2_), self->priv->_reasons = (_vala_array_free (self->priv->_reasons, self->priv->_reasons_length1, (GDestroyNotify) g_free), NULL), self->priv->_reasons_length1 = 0, self->priv->__reasons_size_ = self->priv->_reasons_length1, _tmp3_);
	return self;
}


TrackerMinerMock* tracker_miner_mock_new (const char* name) {
	return tracker_miner_mock_construct (TYPE_TRACKER_MINER_MOCK, name);
}


void tracker_miner_mock_set_paused (TrackerMinerMock* self, gboolean paused) {
	g_return_if_fail (self != NULL);
	self->is_paused = paused;
}


gboolean tracker_miner_mock_get_paused (TrackerMinerMock* self) {
	gboolean result = FALSE;
	g_return_val_if_fail (self != NULL, FALSE);
	result = self->is_paused;
	return result;
}


static void _vala_array_add1 (char*** array, int* length, int* size, char* value) {
	if ((*length) == (*size)) {
		*size = (*size) ? (2 * (*size)) : 4;
		*array = g_renew (char*, *array, (*size) + 1);
	}
	(*array)[(*length)++] = value;
	(*array)[*length] = NULL;
}


static void _vala_array_add2 (char*** array, int* length, int* size, char* value) {
	if ((*length) == (*size)) {
		*size = (*size) ? (2 * (*size)) : 4;
		*array = g_renew (char*, *array, (*size) + 1);
	}
	(*array)[(*length)++] = value;
	(*array)[*length] = NULL;
}


void tracker_miner_mock_pause (TrackerMinerMock* self, const char* app, const char* reason) {
	g_return_if_fail (self != NULL);
	g_return_if_fail (app != NULL);
	g_return_if_fail (reason != NULL);
	if (self->priv->_apps_length1 == 0) {
		char** _tmp1_;
		char** _tmp0_ = NULL;
		self->priv->_apps = (_tmp1_ = (_tmp0_ = g_new0 (char*, 1 + 1), _tmp0_[0] = g_strdup (app), _tmp0_), self->priv->_apps = (_vala_array_free (self->priv->_apps, self->priv->_apps_length1, (GDestroyNotify) g_free), NULL), self->priv->_apps_length1 = 1, self->priv->__apps_size_ = self->priv->_apps_length1, _tmp1_);
	} else {
		_vala_array_add1 (&self->priv->_apps, &self->priv->_apps_length1, &self->priv->__apps_size_, g_strdup (app));
	}
	if (self->priv->_reasons_length1 == 0) {
		char** _tmp3_;
		char** _tmp2_ = NULL;
		self->priv->_reasons = (_tmp3_ = (_tmp2_ = g_new0 (char*, 1 + 1), _tmp2_[0] = g_strdup (reason), _tmp2_), self->priv->_reasons = (_vala_array_free (self->priv->_reasons, self->priv->_reasons_length1, (GDestroyNotify) g_free), NULL), self->priv->_reasons_length1 = 1, self->priv->__reasons_size_ = self->priv->_reasons_length1, _tmp3_);
	} else {
		_vala_array_add2 (&self->priv->_reasons, &self->priv->_reasons_length1, &self->priv->__reasons_size_, g_strdup (reason));
	}
	self->is_paused = TRUE;
	g_signal_emit_by_name (self, "paused");
}


void tracker_miner_mock_resume (TrackerMinerMock* self) {
	char** _tmp0_;
	char** _tmp1_;
	g_return_if_fail (self != NULL);
	self->priv->_apps = (_tmp0_ = NULL, self->priv->_apps = (_vala_array_free (self->priv->_apps, self->priv->_apps_length1, (GDestroyNotify) g_free), NULL), self->priv->_apps_length1 = 0, self->priv->__apps_size_ = self->priv->_apps_length1, _tmp0_);
	self->priv->_reasons = (_tmp1_ = NULL, self->priv->_reasons = (_vala_array_free (self->priv->_reasons, self->priv->_reasons_length1, (GDestroyNotify) g_free), NULL), self->priv->_reasons_length1 = 0, self->priv->__reasons_size_ = self->priv->_reasons_length1, _tmp1_);
	self->is_paused = FALSE;
	g_signal_emit_by_name (self, "resumed");
}


const char* tracker_miner_mock_get_pause_reason (TrackerMinerMock* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_pause_reason;
	return result;
}


void tracker_miner_mock_set_pause_reason (TrackerMinerMock* self, const char* value) {
	char* _tmp0_;
	g_return_if_fail (self != NULL);
	self->priv->_pause_reason = (_tmp0_ = g_strdup (value), _g_free0 (self->priv->_pause_reason), _tmp0_);
	g_object_notify ((GObject *) self, "pause-reason");
}


const char* tracker_miner_mock_get_name (TrackerMinerMock* self) {
	const char* result;
	g_return_val_if_fail (self != NULL, NULL);
	result = self->priv->_name;
	return result;
}


void tracker_miner_mock_set_name (TrackerMinerMock* self, const char* value) {
	char* _tmp0_;
	g_return_if_fail (self != NULL);
	self->priv->_name = (_tmp0_ = g_strdup (value), _g_free0 (self->priv->_name), _tmp0_);
	g_object_notify ((GObject *) self, "name");
}


char** tracker_miner_mock_get_apps (TrackerMinerMock* self, int* result_length1) {
	char** result;
	char** _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	result = (_tmp0_ = self->priv->_apps, *result_length1 = self->priv->_apps_length1, _tmp0_);
	return result;
}


char** tracker_miner_mock_get_reasons (TrackerMinerMock* self, int* result_length1) {
	char** result;
	char** _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	result = (_tmp0_ = self->priv->_apps, *result_length1 = self->priv->_apps_length1, _tmp0_);
	return result;
}


static void tracker_miner_mock_class_init (TrackerMinerMockClass * klass) {
	tracker_miner_mock_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (TrackerMinerMockPrivate));
	G_OBJECT_CLASS (klass)->get_property = tracker_miner_mock_get_property;
	G_OBJECT_CLASS (klass)->set_property = tracker_miner_mock_set_property;
	G_OBJECT_CLASS (klass)->finalize = tracker_miner_mock_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), TRACKER_MINER_MOCK_PAUSE_REASON, g_param_spec_string ("pause-reason", "pause-reason", "pause-reason", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), TRACKER_MINER_MOCK_NAME, g_param_spec_string ("name", "name", "name", NULL, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), TRACKER_MINER_MOCK_APPS, g_param_spec_boxed ("apps", "apps", "apps", G_TYPE_STRV, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), TRACKER_MINER_MOCK_REASONS, g_param_spec_boxed ("reasons", "reasons", "reasons", G_TYPE_STRV, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	g_signal_new ("progress", TYPE_TRACKER_MINER_MOCK, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_user_marshal_VOID__STRING_STRING_DOUBLE, G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_DOUBLE);
	g_signal_new ("paused", TYPE_TRACKER_MINER_MOCK, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
	g_signal_new ("resumed", TYPE_TRACKER_MINER_MOCK, G_SIGNAL_RUN_LAST, 0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}


static void tracker_miner_mock_instance_init (TrackerMinerMock * self) {
	self->priv = TRACKER_MINER_MOCK_GET_PRIVATE (self);
	self->priv->_pause_reason = g_strdup ("");
	self->priv->_name = g_strdup ("");
}


static void tracker_miner_mock_finalize (GObject* obj) {
	TrackerMinerMock * self;
	self = TRACKER_MINER_MOCK (obj);
	_g_free0 (self->priv->_pause_reason);
	_g_free0 (self->priv->_name);
	self->priv->_apps = (_vala_array_free (self->priv->_apps, self->priv->_apps_length1, (GDestroyNotify) g_free), NULL);
	self->priv->_reasons = (_vala_array_free (self->priv->_reasons, self->priv->_reasons_length1, (GDestroyNotify) g_free), NULL);
	G_OBJECT_CLASS (tracker_miner_mock_parent_class)->finalize (obj);
}


GType tracker_miner_mock_get_type (void) {
	static volatile gsize tracker_miner_mock_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_miner_mock_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TrackerMinerMockClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_miner_mock_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerMinerMock), 0, (GInstanceInitFunc) tracker_miner_mock_instance_init, NULL };
		GType tracker_miner_mock_type_id;
		tracker_miner_mock_type_id = g_type_register_static (G_TYPE_OBJECT, "TrackerMinerMock", &g_define_type_info, 0);
		g_once_init_leave (&tracker_miner_mock_type_id__volatile, tracker_miner_mock_type_id);
	}
	return tracker_miner_mock_type_id__volatile;
}


static void tracker_miner_mock_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	TrackerMinerMock * self;
	int length;
	self = TRACKER_MINER_MOCK (object);
	switch (property_id) {
		case TRACKER_MINER_MOCK_PAUSE_REASON:
		g_value_set_string (value, tracker_miner_mock_get_pause_reason (self));
		break;
		case TRACKER_MINER_MOCK_NAME:
		g_value_set_string (value, tracker_miner_mock_get_name (self));
		break;
		case TRACKER_MINER_MOCK_APPS:
		g_value_set_boxed (value, tracker_miner_mock_get_apps (self, &length));
		break;
		case TRACKER_MINER_MOCK_REASONS:
		g_value_set_boxed (value, tracker_miner_mock_get_reasons (self, &length));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void tracker_miner_mock_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	TrackerMinerMock * self;
	self = TRACKER_MINER_MOCK (object);
	switch (property_id) {
		case TRACKER_MINER_MOCK_PAUSE_REASON:
		tracker_miner_mock_set_pause_reason (self, g_value_get_string (value));
		break;
		case TRACKER_MINER_MOCK_NAME:
		tracker_miner_mock_set_name (self, g_value_get_string (value));
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



static void g_cclosure_user_marshal_VOID__STRING_STRING_DOUBLE (GClosure * closure, GValue * return_value, guint n_param_values, const GValue * param_values, gpointer invocation_hint, gpointer marshal_data) {
	typedef void (*GMarshalFunc_VOID__STRING_STRING_DOUBLE) (gpointer data1, const char* arg_1, const char* arg_2, double arg_3, gpointer data2);
	register GMarshalFunc_VOID__STRING_STRING_DOUBLE callback;
	register GCClosure * cc;
	register gpointer data1, data2;
	cc = (GCClosure *) closure;
	g_return_if_fail (n_param_values == 4);
	if (G_CCLOSURE_SWAP_DATA (closure)) {
		data1 = closure->data;
		data2 = param_values->data[0].v_pointer;
	} else {
		data1 = param_values->data[0].v_pointer;
		data2 = closure->data;
	}
	callback = (GMarshalFunc_VOID__STRING_STRING_DOUBLE) (marshal_data ? marshal_data : cc->callback);
	callback (data1, g_value_get_string (param_values + 1), g_value_get_string (param_values + 2), g_value_get_double (param_values + 3), data2);
}



