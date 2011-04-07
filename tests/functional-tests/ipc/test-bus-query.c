/* test-bus-query.c generated by valac 0.12.0.69-6c49, the Vala compiler
 * generated from test-bus-query.vala, do not modify */


#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <libtracker-bus/tracker-bus.h>
#include <libtracker-sparql/tracker-sparql.h>


#define TYPE_TEST_APP (test_app_get_type ())
#define TEST_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TEST_APP, TestApp))
#define TEST_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TEST_APP, TestAppClass))
#define IS_TEST_APP(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TEST_APP))
#define IS_TEST_APP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TEST_APP))
#define TEST_APP_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TEST_APP, TestAppClass))

typedef struct _TestApp TestApp;
typedef struct _TestAppClass TestAppClass;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))



gint _vala_main (gchar** args, int args_length1);
TestApp* test_app_new (TrackerSparqlConnection* connection);
TestApp* test_app_construct (GType object_type, TrackerSparqlConnection* connection);
GType test_app_get_type (void) G_GNUC_CONST;
gint test_app_run (TestApp* self);


gint _vala_main (gchar** args, int args_length1) {
	gint result = 0;
	TrackerBusConnection* _tmp0_ = NULL;
	TrackerBusConnection* _tmp1_;
	TrackerBusConnection* _tmp2_;
	TestApp* _tmp3_ = NULL;
	TestApp* _tmp4_;
	TestApp* app;
	gint _tmp5_;
	GError * _inner_error_ = NULL;
	_tmp0_ = tracker_bus_connection_new (&_inner_error_);
	_tmp1_ = _tmp0_;
	if (_inner_error_ != NULL) {
		goto __catch3_g_error;
	}
	_tmp2_ = _tmp1_;
	_tmp3_ = test_app_new ((TrackerSparqlConnection*) _tmp2_);
	_tmp4_ = _tmp3_;
	_g_object_unref0 (_tmp2_);
	app = _tmp4_;
	_tmp5_ = test_app_run (app);
	result = _tmp5_;
	_g_object_unref0 (app);
	return result;
	_g_object_unref0 (app);
	goto __finally3;
	__catch3_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		g_warning ("test-bus-query.vala:12: Couldn't perform test: %s", e->message);
		result = 1;
		_g_error_free0 (e);
		return result;
	}
	__finally3:
	g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
	g_clear_error (&_inner_error_);
	return 0;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return _vala_main (argv, argc);
}



