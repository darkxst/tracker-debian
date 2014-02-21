/*
 * Generated by gdbus-codegen 2.39.90. DO NOT EDIT.
 *
 * The license of this code is the same as for the source it was derived from.
 */

#ifndef __TRACKER_EXTRACT_PRIORITY_DBUS_H__
#define __TRACKER_EXTRACT_PRIORITY_DBUS_H__

#include <gio/gio.h>

G_BEGIN_DECLS


/* ------------------------------------------------------------------------ */
/* Declarations for org.freedesktop.Tracker1.Extract.Priority */

#define TRACKER_EXTRACT_DBUS_TYPE_PRIORITY (tracker_extract_dbus_priority_get_type ())
#define TRACKER_EXTRACT_DBUS_PRIORITY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY, TrackerExtractDBusPriority))
#define TRACKER_EXTRACT_DBUS_IS_PRIORITY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY))
#define TRACKER_EXTRACT_DBUS_PRIORITY_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY, TrackerExtractDBusPriorityIface))

struct _TrackerExtractDBusPriority;
typedef struct _TrackerExtractDBusPriority TrackerExtractDBusPriority;
typedef struct _TrackerExtractDBusPriorityIface TrackerExtractDBusPriorityIface;

struct _TrackerExtractDBusPriorityIface
{
  GTypeInterface parent_iface;


  gboolean (*handle_clear_rdf_types) (
    TrackerExtractDBusPriority *object,
    GDBusMethodInvocation *invocation);

  gboolean (*handle_set_rdf_types) (
    TrackerExtractDBusPriority *object,
    GDBusMethodInvocation *invocation,
    const gchar *const *arg_rdf_types);

  const gchar *const * (*get_supported_rdf_types) (TrackerExtractDBusPriority *object);

};

GType tracker_extract_dbus_priority_get_type (void) G_GNUC_CONST;

GDBusInterfaceInfo *tracker_extract_dbus_priority_interface_info (void);
guint tracker_extract_dbus_priority_override_properties (GObjectClass *klass, guint property_id_begin);


/* D-Bus method call completion functions: */
void tracker_extract_dbus_priority_complete_set_rdf_types (
    TrackerExtractDBusPriority *object,
    GDBusMethodInvocation *invocation);

void tracker_extract_dbus_priority_complete_clear_rdf_types (
    TrackerExtractDBusPriority *object,
    GDBusMethodInvocation *invocation);



/* D-Bus method calls: */
void tracker_extract_dbus_priority_call_set_rdf_types (
    TrackerExtractDBusPriority *proxy,
    const gchar *const *arg_rdf_types,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tracker_extract_dbus_priority_call_set_rdf_types_finish (
    TrackerExtractDBusPriority *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tracker_extract_dbus_priority_call_set_rdf_types_sync (
    TrackerExtractDBusPriority *proxy,
    const gchar *const *arg_rdf_types,
    GCancellable *cancellable,
    GError **error);

void tracker_extract_dbus_priority_call_clear_rdf_types (
    TrackerExtractDBusPriority *proxy,
    GCancellable *cancellable,
    GAsyncReadyCallback callback,
    gpointer user_data);

gboolean tracker_extract_dbus_priority_call_clear_rdf_types_finish (
    TrackerExtractDBusPriority *proxy,
    GAsyncResult *res,
    GError **error);

gboolean tracker_extract_dbus_priority_call_clear_rdf_types_sync (
    TrackerExtractDBusPriority *proxy,
    GCancellable *cancellable,
    GError **error);



/* D-Bus property accessors: */
const gchar *const *tracker_extract_dbus_priority_get_supported_rdf_types (TrackerExtractDBusPriority *object);
gchar **tracker_extract_dbus_priority_dup_supported_rdf_types (TrackerExtractDBusPriority *object);
void tracker_extract_dbus_priority_set_supported_rdf_types (TrackerExtractDBusPriority *object, const gchar *const *value);


/* ---- */

#define TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY (tracker_extract_dbus_priority_proxy_get_type ())
#define TRACKER_EXTRACT_DBUS_PRIORITY_PROXY(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY, TrackerExtractDBusPriorityProxy))
#define TRACKER_EXTRACT_DBUS_PRIORITY_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY, TrackerExtractDBusPriorityProxyClass))
#define TRACKER_EXTRACT_DBUS_PRIORITY_PROXY_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY, TrackerExtractDBusPriorityProxyClass))
#define TRACKER_EXTRACT_DBUS_IS_PRIORITY_PROXY(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY))
#define TRACKER_EXTRACT_DBUS_IS_PRIORITY_PROXY_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_PROXY))

typedef struct _TrackerExtractDBusPriorityProxy TrackerExtractDBusPriorityProxy;
typedef struct _TrackerExtractDBusPriorityProxyClass TrackerExtractDBusPriorityProxyClass;
typedef struct _TrackerExtractDBusPriorityProxyPrivate TrackerExtractDBusPriorityProxyPrivate;

struct _TrackerExtractDBusPriorityProxy
{
  /*< private >*/
  GDBusProxy parent_instance;
  TrackerExtractDBusPriorityProxyPrivate *priv;
};

struct _TrackerExtractDBusPriorityProxyClass
{
  GDBusProxyClass parent_class;
};

GType tracker_extract_dbus_priority_proxy_get_type (void) G_GNUC_CONST;

void tracker_extract_dbus_priority_proxy_new (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
TrackerExtractDBusPriority *tracker_extract_dbus_priority_proxy_new_finish (
    GAsyncResult        *res,
    GError             **error);
TrackerExtractDBusPriority *tracker_extract_dbus_priority_proxy_new_sync (
    GDBusConnection     *connection,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);

void tracker_extract_dbus_priority_proxy_new_for_bus (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GAsyncReadyCallback  callback,
    gpointer             user_data);
TrackerExtractDBusPriority *tracker_extract_dbus_priority_proxy_new_for_bus_finish (
    GAsyncResult        *res,
    GError             **error);
TrackerExtractDBusPriority *tracker_extract_dbus_priority_proxy_new_for_bus_sync (
    GBusType             bus_type,
    GDBusProxyFlags      flags,
    const gchar         *name,
    const gchar         *object_path,
    GCancellable        *cancellable,
    GError             **error);


/* ---- */

#define TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON (tracker_extract_dbus_priority_skeleton_get_type ())
#define TRACKER_EXTRACT_DBUS_PRIORITY_SKELETON(o) (G_TYPE_CHECK_INSTANCE_CAST ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON, TrackerExtractDBusPrioritySkeleton))
#define TRACKER_EXTRACT_DBUS_PRIORITY_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_CAST ((k), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON, TrackerExtractDBusPrioritySkeletonClass))
#define TRACKER_EXTRACT_DBUS_PRIORITY_SKELETON_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON, TrackerExtractDBusPrioritySkeletonClass))
#define TRACKER_EXTRACT_DBUS_IS_PRIORITY_SKELETON(o) (G_TYPE_CHECK_INSTANCE_TYPE ((o), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON))
#define TRACKER_EXTRACT_DBUS_IS_PRIORITY_SKELETON_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), TRACKER_EXTRACT_DBUS_TYPE_PRIORITY_SKELETON))

typedef struct _TrackerExtractDBusPrioritySkeleton TrackerExtractDBusPrioritySkeleton;
typedef struct _TrackerExtractDBusPrioritySkeletonClass TrackerExtractDBusPrioritySkeletonClass;
typedef struct _TrackerExtractDBusPrioritySkeletonPrivate TrackerExtractDBusPrioritySkeletonPrivate;

struct _TrackerExtractDBusPrioritySkeleton
{
  /*< private >*/
  GDBusInterfaceSkeleton parent_instance;
  TrackerExtractDBusPrioritySkeletonPrivate *priv;
};

struct _TrackerExtractDBusPrioritySkeletonClass
{
  GDBusInterfaceSkeletonClass parent_class;
};

GType tracker_extract_dbus_priority_skeleton_get_type (void) G_GNUC_CONST;

TrackerExtractDBusPriority *tracker_extract_dbus_priority_skeleton_new (void);


G_END_DECLS

#endif /* __TRACKER_EXTRACT_PRIORITY_DBUS_H__ */
