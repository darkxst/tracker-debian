/* Generated by dbus-binding-tool; do not edit! */


#ifndef __dbus_glib_marshal_tracker_resources_MARSHAL_H__
#define __dbus_glib_marshal_tracker_resources_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

#ifdef G_ENABLE_DEBUG
#define g_marshal_value_peek_boolean(v)  g_value_get_boolean (v)
#define g_marshal_value_peek_char(v)     g_value_get_char (v)
#define g_marshal_value_peek_uchar(v)    g_value_get_uchar (v)
#define g_marshal_value_peek_int(v)      g_value_get_int (v)
#define g_marshal_value_peek_uint(v)     g_value_get_uint (v)
#define g_marshal_value_peek_long(v)     g_value_get_long (v)
#define g_marshal_value_peek_ulong(v)    g_value_get_ulong (v)
#define g_marshal_value_peek_int64(v)    g_value_get_int64 (v)
#define g_marshal_value_peek_uint64(v)   g_value_get_uint64 (v)
#define g_marshal_value_peek_enum(v)     g_value_get_enum (v)
#define g_marshal_value_peek_flags(v)    g_value_get_flags (v)
#define g_marshal_value_peek_float(v)    g_value_get_float (v)
#define g_marshal_value_peek_double(v)   g_value_get_double (v)
#define g_marshal_value_peek_string(v)   (char*) g_value_get_string (v)
#define g_marshal_value_peek_param(v)    g_value_get_param (v)
#define g_marshal_value_peek_boxed(v)    g_value_get_boxed (v)
#define g_marshal_value_peek_pointer(v)  g_value_get_pointer (v)
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
#else /* !G_ENABLE_DEBUG */
/* WARNING: This code accesses GValues directly, which is UNSUPPORTED API.
 *          Do not access GValues directly in your code. Instead, use the
 *          g_value_get_*() functions
 */
#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer
#endif /* !G_ENABLE_DEBUG */


/* NONE:STRING,POINTER */
extern void dbus_glib_marshal_tracker_resources_VOID__STRING_POINTER (GClosure     *closure,
                                                                      GValue       *return_value,
                                                                      guint         n_param_values,
                                                                      const GValue *param_values,
                                                                      gpointer      invocation_hint,
                                                                      gpointer      marshal_data);
void
dbus_glib_marshal_tracker_resources_VOID__STRING_POINTER (GClosure     *closure,
                                                          GValue       *return_value G_GNUC_UNUSED,
                                                          guint         n_param_values,
                                                          const GValue *param_values,
                                                          gpointer      invocation_hint G_GNUC_UNUSED,
                                                          gpointer      marshal_data)
{
  typedef void (*GMarshalFunc_VOID__STRING_POINTER) (gpointer     data1,
                                                     gpointer     arg_1,
                                                     gpointer     arg_2,
                                                     gpointer     data2);
  register GMarshalFunc_VOID__STRING_POINTER callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;

  g_return_if_fail (n_param_values == 3);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_VOID__STRING_POINTER) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_marshal_value_peek_string (param_values + 1),
            g_marshal_value_peek_pointer (param_values + 2),
            data2);
}
#define dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER	dbus_glib_marshal_tracker_resources_VOID__STRING_POINTER

/* NONE:POINTER */
#define dbus_glib_marshal_tracker_resources_VOID__POINTER	g_cclosure_marshal_VOID__POINTER
#define dbus_glib_marshal_tracker_resources_NONE__POINTER	dbus_glib_marshal_tracker_resources_VOID__POINTER

G_END_DECLS

#endif /* __dbus_glib_marshal_tracker_resources_MARSHAL_H__ */

#include <dbus/dbus-glib.h>
static const DBusGMethodInfo dbus_glib_tracker_resources_methods[] = {
  { (GCallback) tracker_resources_load, dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER, 0 },
  { (GCallback) tracker_resources_sparql_query, dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER, 51 },
  { (GCallback) tracker_resources_sparql_update, dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER, 128 },
  { (GCallback) tracker_resources_sparql_update_blank, dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER, 189 },
  { (GCallback) tracker_resources_sync, dbus_glib_marshal_tracker_resources_NONE__POINTER, 276 },
  { (GCallback) tracker_resources_batch_sparql_update, dbus_glib_marshal_tracker_resources_NONE__STRING_POINTER, 319 },
  { (GCallback) tracker_resources_batch_commit, dbus_glib_marshal_tracker_resources_NONE__POINTER, 385 },
};

const DBusGObjectInfo dbus_glib_tracker_resources_object_info = {
  0,
  dbus_glib_tracker_resources_methods,
  7,
"org.freedesktop.Tracker1.Resources\0Load\0A\0uri\0I\0s\0\0org.freedesktop.Tracker1.Resources\0SparqlQuery\0A\0query\0I\0s\0result\0O\0F\0N\0aas\0\0org.freedesktop.Tracker1.Resources\0SparqlUpdate\0A\0query\0I\0s\0\0org.freedesktop.Tracker1.Resources\0SparqlUpdateBlank\0A\0query\0I\0s\0result\0O\0F\0N\0aaa{ss}\0\0org.freedesktop.Tracker1.Resources\0Sync\0A\0\0org.freedesktop.Tracker1.Resources\0BatchSparqlUpdate\0A\0query\0I\0s\0\0org.freedesktop.Tracker1.Resources\0BatchCommit\0A\0\0\0",
"org.freedesktop.Tracker1.Resources\0Writeback\0\0",
"\0"
};

