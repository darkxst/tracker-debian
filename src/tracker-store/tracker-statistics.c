/* tracker-statistics.c generated by valac 0.24.0.131-42e78, the Vala compiler
 * generated from tracker-statistics.vala, do not modify */

/*
 * Copyright (C) 2011, Nokia <ivan.frade@nokia.com>
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
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include "libtracker-common/tracker-common.h"
#include "libtracker-data/tracker-db-interface.h"
#include "libtracker-data/tracker-db-manager.h"
#include "libtracker-data/tracker-ontologies.h"
#include "libtracker-data/tracker-class.h"
#include "libtracker-sparql/tracker-sparql.h"


#define TRACKER_TYPE_STATISTICS (tracker_statistics_get_type ())
#define TRACKER_STATISTICS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_STATISTICS, TrackerStatistics))
#define TRACKER_STATISTICS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_STATISTICS, TrackerStatisticsClass))
#define TRACKER_IS_STATISTICS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_STATISTICS))
#define TRACKER_IS_STATISTICS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_STATISTICS))
#define TRACKER_STATISTICS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRACKER_TYPE_STATISTICS, TrackerStatisticsClass))

typedef struct _TrackerStatistics TrackerStatistics;
typedef struct _TrackerStatisticsClass TrackerStatisticsClass;
typedef struct _TrackerStatisticsPrivate TrackerStatisticsPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_variant_builder_unref0(var) ((var == NULL) ? NULL : (var = (g_variant_builder_unref (var), NULL)))
#define _g_variant_unref0(var) ((var == NULL) ? NULL : (var = (g_variant_unref (var), NULL)))

struct _TrackerStatistics {
	GObject parent_instance;
	TrackerStatisticsPrivate * priv;
};

struct _TrackerStatisticsClass {
	GObjectClass parent_class;
};


static gpointer tracker_statistics_parent_class = NULL;
static gboolean tracker_statistics_initialized;
static gboolean tracker_statistics_initialized = FALSE;

GType tracker_statistics_get_type (void) G_GNUC_CONST;
guint tracker_statistics_register_object (void* object, GDBusConnection* connection, const gchar* path, GError** error);
enum  {
	TRACKER_STATISTICS_DUMMY_PROPERTY
};
#define TRACKER_STATISTICS_PATH "/org/freedesktop/Tracker1/Statistics"
GVariant* tracker_statistics_get (TrackerStatistics* self, const char* sender, GError** error);
TrackerStatistics* tracker_statistics_new (void);
TrackerStatistics* tracker_statistics_construct (GType object_type);
static void tracker_statistics_finalize (GObject* obj);
static void _dbus_tracker_statistics_get (TrackerStatistics* self, GVariant* _parameters_, GDBusMethodInvocation* invocation);
static void tracker_statistics_dbus_interface_method_call (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* method_name, GVariant* parameters, GDBusMethodInvocation* invocation, gpointer user_data);
static GVariant* tracker_statistics_dbus_interface_get_property (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* property_name, GError** error, gpointer user_data);
static gboolean tracker_statistics_dbus_interface_set_property (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* property_name, GVariant* value, GError** error, gpointer user_data);
static void _tracker_statistics_unregister_object (gpointer user_data);

static const GDBusArgInfo _tracker_statistics_dbus_arg_info_get_result = {-1, "result", "aas"};
static const GDBusArgInfo * const _tracker_statistics_dbus_arg_info_get_in[] = {NULL};
static const GDBusArgInfo * const _tracker_statistics_dbus_arg_info_get_out[] = {&_tracker_statistics_dbus_arg_info_get_result, NULL};
static const GDBusMethodInfo _tracker_statistics_dbus_method_info_get = {-1, "Get", (GDBusArgInfo **) (&_tracker_statistics_dbus_arg_info_get_in), (GDBusArgInfo **) (&_tracker_statistics_dbus_arg_info_get_out)};
static const GDBusMethodInfo * const _tracker_statistics_dbus_method_info[] = {&_tracker_statistics_dbus_method_info_get, NULL};
static const GDBusSignalInfo * const _tracker_statistics_dbus_signal_info[] = {NULL};
static const GDBusPropertyInfo * const _tracker_statistics_dbus_property_info[] = {NULL};
static const GDBusInterfaceInfo _tracker_statistics_dbus_interface_info = {-1, "org.freedesktop.Tracker1.Statistics", (GDBusMethodInfo **) (&_tracker_statistics_dbus_method_info), (GDBusSignalInfo **) (&_tracker_statistics_dbus_signal_info), (GDBusPropertyInfo **) (&_tracker_statistics_dbus_property_info)};
static const GDBusInterfaceVTable _tracker_statistics_dbus_interface_vtable = {tracker_statistics_dbus_interface_method_call, tracker_statistics_dbus_interface_get_property, tracker_statistics_dbus_interface_set_property};

static gpointer _g_object_ref0 (gpointer self) {
#line 31 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	return self ? g_object_ref (self) : NULL;
#line 95 "tracker-statistics.c"
}


GVariant* tracker_statistics_get (TrackerStatistics* self, const char* sender, GError** error) {
	GVariant* result = NULL;
	TrackerDBusRequest* request = NULL;
	const char* _tmp0_ = NULL;
	TrackerDBusRequest* _tmp1_ = NULL;
	gboolean _tmp2_ = FALSE;
	GVariantBuilder* builder = NULL;
	GVariantBuilder* _tmp28_ = NULL;
	gint _tmp29_ = 0;
	TrackerClass** _tmp30_ = NULL;
	TrackerDBusRequest* _tmp47_ = NULL;
	GVariantBuilder* _tmp48_ = NULL;
	GVariant* _tmp49_ = NULL;
	GError * _inner_error_ = NULL;
#line 27 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	g_return_val_if_fail (self != NULL, NULL);
#line 27 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	g_return_val_if_fail (sender != NULL, NULL);
#line 28 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp0_ = sender;
#line 28 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp1_ = tracker_dbus_request_begin ((const gchar*) _tmp0_, "Statistics.Get", NULL);
#line 28 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	request = _tmp1_;
#line 30 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp2_ = tracker_statistics_initialized;
#line 30 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	if (!_tmp2_) {
#line 127 "tracker-statistics.c"
		TrackerDBInterface* iface = NULL;
		TrackerDBInterface* _tmp3_ = NULL;
		TrackerDBInterface* _tmp4_ = NULL;
		gint _tmp5_ = 0;
		TrackerClass** _tmp6_ = NULL;
#line 31 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		_tmp3_ = tracker_db_manager_get_db_interface ();
#line 31 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		_tmp4_ = _g_object_ref0 (_tmp3_);
#line 31 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		iface = _tmp4_;
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		_tmp6_ = tracker_ontologies_get_classes (&_tmp5_);
#line 141 "tracker-statistics.c"
		{
			TrackerClass** cl_collection = NULL;
			gint cl_collection_length1 = 0;
			gint _cl_collection_size_ = 0;
			gint cl_it = 0;
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
			cl_collection = _tmp6_;
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
			cl_collection_length1 = _tmp5_;
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
			for (cl_it = 0; cl_it < _tmp5_; cl_it = cl_it + 1) {
#line 153 "tracker-statistics.c"
				TrackerClass* _tmp7_ = NULL;
				TrackerClass* cl = NULL;
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp7_ = _g_object_ref0 (cl_collection[cl_it]);
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				cl = _tmp7_;
#line 160 "tracker-statistics.c"
				{
					TrackerClass* _tmp8_ = NULL;
					const gchar* _tmp9_ = NULL;
					const gchar* _tmp10_ = NULL;
					gboolean _tmp11_ = FALSE;
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_tmp8_ = cl;
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_tmp9_ = tracker_class_get_name (_tmp8_);
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_tmp10_ = _tmp9_;
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_tmp11_ = g_str_has_prefix (_tmp10_, "xsd:");
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					if (!_tmp11_) {
#line 176 "tracker-statistics.c"
						TrackerDBStatement* stmt = NULL;
						TrackerDBInterface* _tmp12_ = NULL;
						TrackerClass* _tmp13_ = NULL;
						const gchar* _tmp14_ = NULL;
						const gchar* _tmp15_ = NULL;
						TrackerDBStatement* _tmp16_ = NULL;
						TrackerDBCursor* stat_cursor = NULL;
						TrackerDBStatement* _tmp17_ = NULL;
						TrackerDBCursor* _tmp18_ = NULL;
						gboolean _tmp19_ = FALSE;
						TrackerDBCursor* _tmp20_ = NULL;
						gboolean _tmp21_ = FALSE;
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp12_ = iface;
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp13_ = cl;
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp14_ = tracker_class_get_name (_tmp13_);
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp15_ = _tmp14_;
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp16_ = tracker_db_interface_create_statement (_tmp12_, TRACKER_DB_STATEMENT_CACHE_TYPE_NONE, &_inner_error_, "SELECT COUNT(1) FROM \"%s\"", _tmp15_);
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						stmt = _tmp16_;
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							g_propagate_error (error, _inner_error_);
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (cl);
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (iface);
#line 37 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							return NULL;
#line 211 "tracker-statistics.c"
						}
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp17_ = stmt;
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp18_ = tracker_db_statement_start_cursor (_tmp17_, &_inner_error_);
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						stat_cursor = _tmp18_;
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							g_propagate_error (error, _inner_error_);
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (stmt);
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (cl);
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (iface);
#line 41 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							return NULL;
#line 231 "tracker-statistics.c"
						}
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp20_ = stat_cursor;
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp21_ = tracker_sparql_cursor_next ((TrackerSparqlCursor*) _tmp20_, NULL, &_inner_error_);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_tmp19_ = _tmp21_;
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						if (G_UNLIKELY (_inner_error_ != NULL)) {
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							g_propagate_error (error, _inner_error_);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (stat_cursor);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (stmt);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (cl);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_g_object_unref0 (iface);
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							return NULL;
#line 253 "tracker-statistics.c"
						}
#line 42 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						if (_tmp19_) {
#line 257 "tracker-statistics.c"
							TrackerClass* _tmp22_ = NULL;
							TrackerDBCursor* _tmp23_ = NULL;
							gint64 _tmp24_ = 0LL;
#line 43 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp22_ = cl;
#line 43 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp23_ = stat_cursor;
#line 43 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp24_ = tracker_sparql_cursor_get_integer ((TrackerSparqlCursor*) _tmp23_, 0);
#line 43 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							tracker_class_set_count (_tmp22_, (gint) _tmp24_);
#line 269 "tracker-statistics.c"
						} else {
							TrackerClass* _tmp25_ = NULL;
							const gchar* _tmp26_ = NULL;
							const gchar* _tmp27_ = NULL;
#line 45 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp25_ = cl;
#line 45 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp26_ = tracker_class_get_name (_tmp25_);
#line 45 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							_tmp27_ = _tmp26_;
#line 45 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
							g_warning ("Unable to query instance count for class %s", _tmp27_);
#line 282 "tracker-statistics.c"
						}
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_g_object_unref0 (stat_cursor);
#line 35 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
						_g_object_unref0 (stmt);
#line 288 "tracker-statistics.c"
					}
#line 33 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_g_object_unref0 (cl);
#line 292 "tracker-statistics.c"
				}
			}
		}
#line 50 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		tracker_statistics_initialized = TRUE;
#line 30 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		_g_object_unref0 (iface);
#line 300 "tracker-statistics.c"
	}
#line 53 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp28_ = g_variant_builder_new ((const GVariantType*) "aas");
#line 53 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	builder = _tmp28_;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp30_ = tracker_ontologies_get_classes (&_tmp29_);
#line 308 "tracker-statistics.c"
	{
		TrackerClass** cl_collection = NULL;
		gint cl_collection_length1 = 0;
		gint _cl_collection_size_ = 0;
		gint cl_it = 0;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		cl_collection = _tmp30_;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		cl_collection_length1 = _tmp29_;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
		for (cl_it = 0; cl_it < _tmp29_; cl_it = cl_it + 1) {
#line 320 "tracker-statistics.c"
			TrackerClass* _tmp31_ = NULL;
			TrackerClass* cl = NULL;
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
			_tmp31_ = _g_object_ref0 (cl_collection[cl_it]);
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
			cl = _tmp31_;
#line 327 "tracker-statistics.c"
			{
				TrackerClass* _tmp32_ = NULL;
				gint _tmp33_ = 0;
				gint _tmp34_ = 0;
				GVariantBuilder* _tmp35_ = NULL;
				GVariantBuilder* _tmp36_ = NULL;
				TrackerClass* _tmp37_ = NULL;
				const gchar* _tmp38_ = NULL;
				const gchar* _tmp39_ = NULL;
				GVariantBuilder* _tmp40_ = NULL;
				TrackerClass* _tmp41_ = NULL;
				gint _tmp42_ = 0;
				gint _tmp43_ = 0;
				gchar* _tmp44_ = NULL;
				gchar* _tmp45_ = NULL;
				GVariantBuilder* _tmp46_ = NULL;
#line 56 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp32_ = cl;
#line 56 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp33_ = tracker_class_get_count (_tmp32_);
#line 56 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp34_ = _tmp33_;
#line 56 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				if (_tmp34_ == 0) {
#line 58 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					_g_object_unref0 (cl);
#line 58 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
					continue;
#line 356 "tracker-statistics.c"
				}
#line 61 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp35_ = builder;
#line 61 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				g_variant_builder_open (_tmp35_, (const GVariantType*) "as");
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp36_ = builder;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp37_ = cl;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp38_ = tracker_class_get_name (_tmp37_);
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp39_ = _tmp38_;
#line 62 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				g_variant_builder_add (_tmp36_, "s", _tmp39_, NULL);
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp40_ = builder;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp41_ = cl;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp42_ = tracker_class_get_count (_tmp41_);
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp43_ = _tmp42_;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp44_ = g_strdup_printf ("%i", _tmp43_);
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp45_ = _tmp44_;
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				g_variant_builder_add (_tmp40_, "s", _tmp45_, NULL);
#line 63 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_g_free0 (_tmp45_);
#line 64 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_tmp46_ = builder;
#line 64 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				g_variant_builder_close (_tmp46_);
#line 55 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
				_g_object_unref0 (cl);
#line 394 "tracker-statistics.c"
			}
		}
	}
#line 67 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp47_ = request;
#line 67 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	tracker_dbus_request_end (_tmp47_, NULL);
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp48_ = builder;
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_tmp49_ = g_variant_builder_end (_tmp48_);
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	g_variant_ref_sink (_tmp49_);
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	result = _tmp49_;
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	_g_variant_builder_unref0 (builder);
#line 69 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	return result;
#line 414 "tracker-statistics.c"
}


TrackerStatistics* tracker_statistics_construct (GType object_type) {
	TrackerStatistics * self = NULL;
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	self = (TrackerStatistics*) g_object_new (object_type, NULL);
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	return self;
#line 424 "tracker-statistics.c"
}


TrackerStatistics* tracker_statistics_new (void) {
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	return tracker_statistics_construct (TRACKER_TYPE_STATISTICS);
#line 431 "tracker-statistics.c"
}


static void tracker_statistics_class_init (TrackerStatisticsClass * klass) {
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	tracker_statistics_parent_class = g_type_class_peek_parent (klass);
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	G_OBJECT_CLASS (klass)->finalize = tracker_statistics_finalize;
#line 440 "tracker-statistics.c"
}


static void tracker_statistics_instance_init (TrackerStatistics * self) {
}


static void tracker_statistics_finalize (GObject* obj) {
	TrackerStatistics * self;
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, TRACKER_TYPE_STATISTICS, TrackerStatistics);
#line 21 "/home/martyn/Source/checkout/gnome/tracker/src/tracker-store/tracker-statistics.vala"
	G_OBJECT_CLASS (tracker_statistics_parent_class)->finalize (obj);
#line 454 "tracker-statistics.c"
}


GType tracker_statistics_get_type (void) {
	static volatile gsize tracker_statistics_type_id__volatile = 0;
	if (g_once_init_enter (&tracker_statistics_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (TrackerStatisticsClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) tracker_statistics_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (TrackerStatistics), 0, (GInstanceInitFunc) tracker_statistics_instance_init, NULL };
		GType tracker_statistics_type_id;
		tracker_statistics_type_id = g_type_register_static (G_TYPE_OBJECT, "TrackerStatistics", &g_define_type_info, 0);
		g_type_set_qdata (tracker_statistics_type_id, g_quark_from_static_string ("vala-dbus-register-object"), (void*) tracker_statistics_register_object);
		g_once_init_leave (&tracker_statistics_type_id__volatile, tracker_statistics_type_id);
	}
	return tracker_statistics_type_id__volatile;
}


static void _dbus_tracker_statistics_get (TrackerStatistics* self, GVariant* _parameters_, GDBusMethodInvocation* invocation) {
	GError* error = NULL;
	GVariantIter _arguments_iter;
	GDBusMessage* _reply_message;
	GVariant* _reply;
	GVariantBuilder _reply_builder;
	GVariant* result;
	g_variant_iter_init (&_arguments_iter, _parameters_);
	result = tracker_statistics_get (self, g_dbus_method_invocation_get_sender (invocation), &error);
	if (error) {
		g_dbus_method_invocation_return_gerror (invocation, error);
		return;
	}
	_reply_message = g_dbus_message_new_method_reply (g_dbus_method_invocation_get_message (invocation));
	g_variant_builder_init (&_reply_builder, G_VARIANT_TYPE_TUPLE);
	g_variant_builder_add_value (&_reply_builder, result);
	_g_variant_unref0 (result);
	_reply = g_variant_builder_end (&_reply_builder);
	g_dbus_message_set_body (_reply_message, _reply);
	g_dbus_connection_send_message (g_dbus_method_invocation_get_connection (invocation), _reply_message, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, NULL);
	g_object_unref (invocation);
	g_object_unref (_reply_message);
}


static void tracker_statistics_dbus_interface_method_call (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* method_name, GVariant* parameters, GDBusMethodInvocation* invocation, gpointer user_data) {
	gpointer* data;
	gpointer object;
	data = user_data;
	object = data[0];
	if (strcmp (method_name, "Get") == 0) {
		_dbus_tracker_statistics_get (object, parameters, invocation);
	} else {
		g_object_unref (invocation);
	}
}


static GVariant* tracker_statistics_dbus_interface_get_property (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* property_name, GError** error, gpointer user_data) {
	gpointer* data;
	gpointer object;
	data = user_data;
	object = data[0];
	return NULL;
}


static gboolean tracker_statistics_dbus_interface_set_property (GDBusConnection* connection, const gchar* sender, const gchar* object_path, const gchar* interface_name, const gchar* property_name, GVariant* value, GError** error, gpointer user_data) {
	gpointer* data;
	gpointer object;
	data = user_data;
	object = data[0];
	return FALSE;
}


guint tracker_statistics_register_object (gpointer object, GDBusConnection* connection, const gchar* path, GError** error) {
	guint result;
	gpointer *data;
	data = g_new (gpointer, 3);
	data[0] = g_object_ref (object);
	data[1] = g_object_ref (connection);
	data[2] = g_strdup (path);
	result = g_dbus_connection_register_object (connection, path, (GDBusInterfaceInfo *) (&_tracker_statistics_dbus_interface_info), &_tracker_statistics_dbus_interface_vtable, data, _tracker_statistics_unregister_object, error);
	if (!result) {
		return 0;
	}
	return result;
}


static void _tracker_statistics_unregister_object (gpointer user_data) {
	gpointer* data;
	data = user_data;
	g_object_unref (data[0]);
	g_object_unref (data[1]);
	g_free (data[2]);
	g_free (data);
}



