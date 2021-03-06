/* test-default-update.c generated by valac 0.24.0.131-42e78, the Vala compiler
 * generated from test-default-update.vala, do not modify */


#include <glib.h>
#include <glib-object.h>
#include "libtracker-sparql/tracker-sparql.h"
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Block1Data Block1Data;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_main_loop_unref0(var) ((var == NULL) ? NULL : (var = (g_main_loop_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))

#define TYPE_TEST_APP (test_app_get_type ())
#define TEST_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TEST_APP, TestApp))
#define TEST_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TEST_APP, TestAppClass))
#define IS_TEST_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TEST_APP))
#define IS_TEST_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TEST_APP))
#define TEST_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TEST_APP, TestAppClass))

typedef struct _TestApp TestApp;
typedef struct _TestAppClass TestAppClass;

struct _Block1Data {
	int _ref_count_;
	TrackerSparqlConnection* c;
};


extern gint res;
gint res = 0;
extern GMainLoop* loop;
GMainLoop* loop = NULL;

void test_async (void);
static Block1Data* block1_data_ref (Block1Data* _data1_);
static void block1_data_unref (void * _userdata_);
static void __lambda4_ (Block1Data* _data1_, GObject* o, GAsyncResult* res);
static void ___lambda4__gasync_ready_callback (GObject* source_object, GAsyncResult* res, gpointer self);
GType test_app_get_type (void) G_GNUC_CONST;
TestApp* test_app_new (TrackerSparqlConnection* connection);
TestApp* test_app_construct (GType object_type, TrackerSparqlConnection* connection);
gint test_app_run (TestApp* self);
void test_sync (void);
gint _vala_main (gchar** args, int args_length1);


static Block1Data* block1_data_ref (Block1Data* _data1_) {
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_atomic_int_inc (&_data1_->_ref_count_);
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	return _data1_;
#line 57 "test-default-update.c"
}


static void block1_data_unref (void * _userdata_) {
	Block1Data* _data1_;
	_data1_ = (Block1Data*) _userdata_;
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	if (g_atomic_int_dec_and_test (&_data1_->_ref_count_)) {
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (_data1_->c);
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_slice_free (Block1Data, _data1_);
#line 70 "test-default-update.c"
	}
}


static void __lambda4_ (Block1Data* _data1_, GObject* o, GAsyncResult* res) {
	GError * _inner_error_ = NULL;
#line 12 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_return_if_fail (res != NULL);
#line 79 "test-default-update.c"
	{
		TrackerSparqlConnection* _tmp0_ = NULL;
		GAsyncResult* _tmp1_ = NULL;
		TrackerSparqlConnection* _tmp2_ = NULL;
		TrackerSparqlConnection* _tmp3_ = NULL;
		GMainLoop* _tmp4_ = NULL;
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp1_ = res;
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp2_ = tracker_sparql_connection_get_finish (_tmp1_, &_inner_error_);
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp0_ = _tmp2_;
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 94 "test-default-update.c"
			goto __catch7_g_error;
		}
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp3_ = _tmp0_;
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp0_ = NULL;
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (_data1_->c);
#line 14 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_data1_->c = _tmp3_;
#line 15 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp4_ = loop;
#line 15 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_main_loop_quit (_tmp4_);
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (_tmp0_);
#line 111 "test-default-update.c"
	}
	goto __finally7;
	__catch7_g_error:
	{
		GError* e = NULL;
		GError* _tmp5_ = NULL;
		const gchar* _tmp6_ = NULL;
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		e = _inner_error_;
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_inner_error_ = NULL;
#line 17 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp5_ = e;
#line 17 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp6_ = _tmp5_->message;
#line 17 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_warning ("test-default-update.vala:17: Couldn't perform test: %s", _tmp6_);
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_error_free0 (e);
#line 131 "test-default-update.c"
	}
	__finally7:
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_clear_error (&_inner_error_);
#line 13 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		return;
#line 142 "test-default-update.c"
	}
}


static void ___lambda4__gasync_ready_callback (GObject* source_object, GAsyncResult* res, gpointer self) {
#line 12 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	__lambda4_ (self, source_object, res);
#line 12 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	block1_data_unref (self);
#line 152 "test-default-update.c"
}


void test_async (void) {
	Block1Data* _data1_;
	GMainLoop* _tmp0_ = NULL;
	GMainLoop* _tmp1_ = NULL;
	TestApp* app = NULL;
	TestApp* _tmp2_ = NULL;
	gint _tmp3_ = 0;
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_data1_ = g_slice_new0 (Block1Data);
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_data1_->_ref_count_ = 1;
#line 7 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_data1_->c = NULL;
#line 10 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("Getting connection async\n");
#line 11 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp0_ = g_main_loop_new (NULL, FALSE);
#line 11 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_g_main_loop_unref0 (loop);
#line 11 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	loop = _tmp0_;
#line 12 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	tracker_sparql_connection_get_async (NULL, ___lambda4__gasync_ready_callback, block1_data_ref (_data1_));
#line 20 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp1_ = loop;
#line 20 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_main_loop_run (_tmp1_);
#line 22 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("Got it %p\n", _data1_->c);
#line 24 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("Creating app with connection\n");
#line 25 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp2_ = test_app_new (_data1_->c);
#line 25 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	app = _tmp2_;
#line 27 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("Running app\n");
#line 28 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp3_ = test_app_run (app);
#line 28 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	res = _tmp3_;
#line 30 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("\n");
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_g_object_unref0 (app);
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	block1_data_unref (_data1_);
#line 6 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_data1_ = NULL;
#line 205 "test-default-update.c"
}


void test_sync (void) {
	GError * _inner_error_ = NULL;
	{
		TrackerSparqlConnection* c = NULL;
		TrackerSparqlConnection* _tmp0_ = NULL;
		TrackerSparqlConnection* _tmp1_ = NULL;
		TrackerSparqlConnection* _tmp2_ = NULL;
		TrackerSparqlConnection* _tmp3_ = NULL;
		TestApp* app = NULL;
		TrackerSparqlConnection* _tmp4_ = NULL;
		TestApp* _tmp5_ = NULL;
		TestApp* _tmp6_ = NULL;
		gint _tmp7_ = 0;
#line 38 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_print ("Getting connection\n");
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp1_ = tracker_sparql_connection_get (NULL, &_inner_error_);
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp0_ = _tmp1_;
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
			_g_object_unref0 (c);
#line 232 "test-default-update.c"
			goto __catch8_g_error;
		}
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp2_ = _tmp0_;
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp0_ = NULL;
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (c);
#line 39 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		c = _tmp2_;
#line 41 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp3_ = c;
#line 41 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_print ("Got it %p\n", _tmp3_);
#line 43 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_print ("Creating app with connection\n");
#line 44 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp4_ = c;
#line 44 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp5_ = test_app_new (_tmp4_);
#line 44 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		app = _tmp5_;
#line 46 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_print ("Running app\n");
#line 47 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp6_ = app;
#line 47 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp7_ = test_app_run (_tmp6_);
#line 47 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		res = _tmp7_;
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (app);
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (_tmp0_);
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_object_unref0 (c);
#line 269 "test-default-update.c"
	}
	goto __finally8;
	__catch8_g_error:
	{
		GError* e = NULL;
		GError* _tmp8_ = NULL;
		const gchar* _tmp9_ = NULL;
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		e = _inner_error_;
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_inner_error_ = NULL;
#line 49 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp8_ = e;
#line 49 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp9_ = _tmp8_->message;
#line 49 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_warning ("test-default-update.vala:49: Couldn't perform test: %s", _tmp9_);
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_g_error_free0 (e);
#line 289 "test-default-update.c"
	}
	__finally8:
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		g_clear_error (&_inner_error_);
#line 34 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		return;
#line 300 "test-default-update.c"
	}
#line 52 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("\n");
#line 304 "test-default-update.c"
}


gint _vala_main (gchar** args, int args_length1) {
	gint result = 0;
	gint _tmp0_ = 0;
	gint _tmp2_ = 0;
#line 58 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	g_print ("Starting...\n");
#line 60 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	test_sync ();
#line 62 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp0_ = res;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	if (_tmp0_ < 0) {
#line 320 "test-default-update.c"
		gint _tmp1_ = 0;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		_tmp1_ = res;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		result = _tmp1_;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
		return result;
#line 328 "test-default-update.c"
	}
#line 67 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	test_async ();
#line 69 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	_tmp2_ = res;
#line 69 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	result = _tmp2_;
#line 69 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	return result;
#line 338 "test-default-update.c"
}


int main (int argc, char ** argv) {
#if !GLIB_CHECK_VERSION (2,35,0)
	g_type_init ();
#endif
#line 55 "/home/martyn/Source/checkout/gnome/tracker/tests/functional-tests/ipc/test-default-update.vala"
	return _vala_main (argv, argc);
#line 348 "test-default-update.c"
}



