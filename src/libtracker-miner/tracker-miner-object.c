/*
 * Copyright (C) 2009, Nokia <ivan.frade@nokia.com>
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

#include "config.h"

#include <libtracker-common/tracker-dbus.h>
#include <libtracker-common/tracker-type-utils.h>

#include "tracker-marshal.h"
#include "tracker-miner-object.h"
#include "tracker-miner-dbus.h"
#include "tracker-miner-glue.h"
#include "tracker-dbus.h"

/**
 * SECTION:tracker-miner
 * @short_description: Abstract base class for data miners
 * @include: libtracker-miner/tracker-miner.h
 *
 * #TrackerMiner is an abstract base class to help developing data miners
 * for tracker-store, being an abstract class it doesn't do much by itself,
 * but provides the basic signaling and operation control so the miners
 * implementing this class are properly recognized by Tracker, and can be
 * controlled properly by external means such as #TrackerMinerManager.
 **/

#define TRACKER_MINER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TRACKER_TYPE_MINER, TrackerMinerPrivate))

static GQuark miner_error_quark = 0;

struct TrackerMinerPrivate {
	TrackerClient *client;

	GHashTable *pauses;

	gboolean started;

	gchar *name;
	gchar *status;
	gdouble progress;

	gint availability_cookie;

	GPtrArray *async_calls;
};

typedef struct {
	gint cookie;
	gchar *application;
	gchar *reason;
} PauseData;

typedef struct {
	TrackerMiner *miner;
	GCancellable *cancellable;
	gpointer callback;
	gpointer user_data;
	gpointer source_function;

	guint id;
	guint signal_id;
} AsyncCallData;

enum {
	PROP_0,
	PROP_NAME,
	PROP_STATUS,
	PROP_PROGRESS
};

enum {
	STARTED,
	STOPPED,
	PAUSED,
	RESUMED,
	PROGRESS,
	ERROR,
	IGNORE_NEXT_UPDATE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void       miner_set_property           (GObject       *object,
                                                guint          param_id,
                                                const GValue  *value,
                                                GParamSpec    *pspec);
static void       miner_get_property           (GObject       *object,
                                                guint          param_id,
                                                GValue        *value,
                                                GParamSpec    *pspec);
static void       miner_finalize               (GObject       *object);
static void       miner_dispose                (GObject       *object);
static void       miner_constructed            (GObject       *object);
static void       pause_data_destroy           (gpointer       data);
static PauseData *pause_data_new               (const gchar   *application,
                                                const gchar   *reason);
static void       async_call_data_notify_error (AsyncCallData *data,
                                                gint           code,
                                                const gchar   *message);
static void       async_call_data_destroy      (AsyncCallData *data,
                                                gboolean       remove);
static void       sparql_cancelled_cb          (GCancellable  *cancellable,
                                                AsyncCallData *data);
static void       store_name_monitor_cb (TrackerMiner *miner,
                                         const gchar  *name,
                                         gboolean      available);

G_DEFINE_ABSTRACT_TYPE (TrackerMiner, tracker_miner, G_TYPE_OBJECT)

static void
tracker_miner_class_init (TrackerMinerClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->set_property = miner_set_property;
	object_class->get_property = miner_get_property;
	object_class->finalize     = miner_finalize;
	object_class->constructed  = miner_constructed;
	object_class->dispose      = miner_dispose;

	/**
	 * TrackerMiner::started:
	 * @miner: the #TrackerMiner
	 *
	 * the ::started signal is emitted in the miner
	 * right after it has been started through
	 * tracker_miner_start().
	 **/
	signals[STARTED] =
		g_signal_new ("started",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, started),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	/**
	 * TrackerMiner::stopped:
	 * @miner: the #TrackerMiner
	 *
	 * the ::stopped signal is emitted in the miner
	 * right after it has been stopped through
	 * tracker_miner_stop().
	 **/
	signals[STOPPED] =
		g_signal_new ("stopped",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, stopped),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	/**
	 * TrackerMiner::paused:
	 * @miner: the #TrackerMiner
	 *
	 * the ::paused signal is emitted whenever
	 * there is any reason to pause, either
	 * internal (through tracker_miner_pause()) or
	 * external (through DBus, see #TrackerMinerManager).
	 **/
	signals[PAUSED] =
		g_signal_new ("paused",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, paused),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	/**
	 * TrackerMiner::resumed:
	 * @miner: the #TrackerMiner
	 *
	 * the ::resumed signal is emitted whenever
	 * all reasons to pause have disappeared, see
	 * tracker_miner_resume() and #TrackerMinerManager.
	 **/
	signals[RESUMED] =
		g_signal_new ("resumed",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, resumed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__VOID,
		              G_TYPE_NONE, 0);
	/**
	 * TrackerMiner::progress:
	 * @miner: the #TrackerMiner
	 * @status: miner status
	 * @progress: a #gdouble indicating miner progress, from 0 to 1.
	 *
	 * the ::progress signal will be emitted by TrackerMiner implementations
	 * to indicate progress about the data mining process. @status will
	 * contain a translated string with the current miner status and @progress
	 * will indicate how much has been processed so far.
	 **/
	signals[PROGRESS] =
		g_signal_new ("progress",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, progress),
		              NULL, NULL,
		              tracker_marshal_VOID__STRING_DOUBLE,
		              G_TYPE_NONE, 2,
		              G_TYPE_STRING,
		              G_TYPE_DOUBLE);
	/**
	 * TrackerMiner::error:
	 * @miner: the #TrackerMiner
	 * @error: the error that happened
	 *
	 * The ::error signal will be emitted by TrackerMiner implementations to
	 * indicate some error in the data mining process.
	 **/
	signals[ERROR] =
		g_signal_new ("error",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, error),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__POINTER,
		              G_TYPE_NONE, 1,
		              G_TYPE_POINTER);

	/**
	 * TrackerMiner::ignore-next-update:
	 * @miner: the #TrackerMiner
	 * @urls: the urls to mark as ignore on next update
	 *
	 * the ::ignore-next-update signal is emitted in the miner
	 * right after it has been asked to mark @urls as to ignore on next update
	 * through tracker_miner_ignore_next_update().
	 **/
	signals[IGNORE_NEXT_UPDATE] =
		g_signal_new ("ignore-next-update",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerClass, ignore_next_update),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__BOXED,
		              G_TYPE_NONE, 1,
		              G_TYPE_STRV);

	g_object_class_install_property (object_class,
	                                 PROP_NAME,
	                                 g_param_spec_string ("name",
	                                                      "Miner name",
	                                                      "Miner name",
	                                                      NULL,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (object_class,
	                                 PROP_STATUS,
	                                 g_param_spec_string ("status",
	                                                      "Status",
	                                                      "Translatable string with status description",
	                                                      NULL,
	                                                      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
	                                 PROP_PROGRESS,
	                                 g_param_spec_double ("progress",
	                                                      "Progress",
	                                                      "Miner progress",
	                                                      0.0,
	                                                      1.0,
	                                                      0.0,
	                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_type_class_add_private (object_class, sizeof (TrackerMinerPrivate));

	miner_error_quark = g_quark_from_static_string ("TrackerMiner");
}

static void
tracker_miner_init (TrackerMiner *miner)
{
	TrackerMinerPrivate *priv;

	miner->private = priv = TRACKER_MINER_GET_PRIVATE (miner);

	/* Set the timeout to 0 so we don't have one */
	priv->client = tracker_client_new (TRACKER_CLIENT_ENABLE_WARNINGS, G_MAXINT);

	priv->pauses = g_hash_table_new_full (g_direct_hash,
	                                      g_direct_equal,
	                                      NULL,
	                                      pause_data_destroy);
	priv->async_calls = g_ptr_array_new ();
}

static void
miner_update_progress (TrackerMiner *miner)
{
	g_signal_emit (miner, signals[PROGRESS], 0,
	               miner->private->status,
	               miner->private->progress);
}

static void
miner_set_property (GObject      *object,
                    guint         prop_id,
                    const GValue *value,
                    GParamSpec   *pspec)
{
	TrackerMiner *miner = TRACKER_MINER (object);

	switch (prop_id) {
	case PROP_NAME:
		g_free (miner->private->name);
		miner->private->name = g_value_dup_string (value);
		break;
	case PROP_STATUS: {
		const gchar *new_status;

		new_status = g_value_get_string (value);
		if (miner->private->status && new_status &&
		    strcmp (miner->private->status, new_status) == 0) {
			/* Same, do nothing */
			break;
		}

		g_free (miner->private->status);
		miner->private->status = g_strdup (new_status);
		miner_update_progress (miner);
		break;
	}
	case PROP_PROGRESS: {
		gdouble new_progress;

		new_progress = g_value_get_double (value);

		/* Only notify 1% changes */
		if ((gint) (miner->private->progress * 100) == (gint) (new_progress * 100)) {
			/* Same, do nothing */
			break;
		}

		miner->private->progress = new_progress;
		miner_update_progress (miner);
		break;
	}
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
miner_get_property (GObject    *object,
                    guint       prop_id,
                    GValue     *value,
                    GParamSpec *pspec)
{
	TrackerMiner *miner = TRACKER_MINER (object);

	switch (prop_id) {
	case PROP_NAME:
		g_value_set_string (value, miner->private->name);
		break;
	case PROP_STATUS:
		g_value_set_string (value, miner->private->status);
		break;
	case PROP_PROGRESS:
		g_value_set_double (value, miner->private->progress);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
async_call_finalize_foreach (AsyncCallData *data,
                             gpointer       user_data)
{
	async_call_data_notify_error (data, 0, "Miner is being finalized");
	async_call_data_destroy (data, TRUE);
}

static void
miner_dispose (GObject *object)
{
	TrackerMiner *miner = TRACKER_MINER (object);

	g_ptr_array_foreach (miner->private->async_calls,
	                     (GFunc) async_call_finalize_foreach,
	                     object);
}

static void
miner_finalize (GObject *object)
{
	TrackerMiner *miner = TRACKER_MINER (object);

	g_free (miner->private->status);
	g_free (miner->private->name);

	if (miner->private->client) {
		g_object_unref (miner->private->client);
	}

	g_hash_table_unref (miner->private->pauses);
	g_ptr_array_free (miner->private->async_calls, TRUE);

	_tracker_miner_dbus_shutdown (miner);

	G_OBJECT_CLASS (tracker_miner_parent_class)->finalize (object);
}

static void
miner_constructed (GObject *object)
{
	TrackerMiner *miner = TRACKER_MINER (object);

	_tracker_miner_dbus_init (miner, &dbus_glib__tracker_miner_object_info);
	_tracker_miner_dbus_add_name_watch (miner, "org.freedesktop.Tracker1",
                                            store_name_monitor_cb);
}

static void
store_name_monitor_cb (TrackerMiner *miner,
                       const gchar  *name,
                       gboolean      available)
{
	GError *error = NULL;

	g_debug ("Tracker-store availability has changed to %d", available);

	if (available && miner->private->availability_cookie != 0) {
		tracker_miner_resume (miner,
		                      miner->private->availability_cookie,
		                      &error);

		if (error) {
			g_warning ("Error happened resuming miner: %s\n", error->message);
			g_error_free (error);
		}

		miner->private->availability_cookie = 0;
	} else if (!available && miner->private->availability_cookie == 0) {
		gint cookie_id;

		cookie_id = tracker_miner_pause (miner,
		                                 _("Data store is not available"),
		                                 &error);

		if (error) {
			g_warning ("Could not pause: %s", error->message);
			g_error_free (error);
		} else {
			miner->private->availability_cookie = cookie_id;
		}
	}
}

static PauseData *
pause_data_new (const gchar *application,
                const gchar *reason)
{
	PauseData *data;
	static gint cookie = 1;

	data = g_slice_new0 (PauseData);

	data->cookie = cookie++;
	data->application = g_strdup (application);
	data->reason = g_strdup (reason);

	return data;
}

static void
pause_data_destroy (gpointer data)
{
	PauseData *pd;

	pd = data;

	g_free (pd->reason);
	g_free (pd->application);

	g_slice_free (PauseData, pd);
}

static void
async_call_data_destroy (AsyncCallData *data,
                         gboolean       remove)
{
	TrackerMiner *miner = data->miner;

	if (data->cancellable) {
		if (data->signal_id) {
			g_signal_handler_disconnect (data->cancellable, data->signal_id);
		}

		g_object_unref (data->cancellable);
	}

	if (data->id != 0) {
		tracker_cancel_call (miner->private->client, data->id);
		data->id = 0;
	}

	if (remove) {
		g_ptr_array_remove_fast (miner->private->async_calls, data);
	}

	g_slice_free (AsyncCallData, data);
}

static AsyncCallData *
async_call_data_new (TrackerMiner *miner,
                     GCancellable *cancellable,
                     gpointer      callback,
                     gpointer      user_data,
                     gpointer      source_function)
{
	AsyncCallData *data;

	data = g_slice_new0 (AsyncCallData);
	data->miner = miner;
	data->callback = callback;
	data->user_data = user_data;
	data->source_function = source_function;

	if (cancellable) {
		data->cancellable = g_object_ref (cancellable);

		data->signal_id = g_signal_connect (cancellable, "cancelled",
		                                    G_CALLBACK (sparql_cancelled_cb), data);
	}

	g_ptr_array_add (miner->private->async_calls, data);

	return data;
}

static void
async_call_data_update_callback (AsyncCallData *data,
                                 GError        *error)
{
	GAsyncReadyCallback callback;
	GSimpleAsyncResult *result;

	callback = data->callback;

	if (error) {
		result = g_simple_async_result_new_from_error (G_OBJECT (data->miner),
		                                               callback,
		                                               data->user_data,
		                                               (GError *) error);
	} else {
		result = g_simple_async_result_new (G_OBJECT (data->miner),
		                                    callback,
		                                    data->user_data,
		                                    data->source_function);
	}

	g_simple_async_result_complete (result);
	g_object_unref (result);
}

static void
async_call_data_query_callback (AsyncCallData   *data,
                                const GPtrArray *query_results,
                                GError          *error)
{
	GAsyncReadyCallback callback;
	GSimpleAsyncResult *result;

	callback = data->callback;

	if (error) {
		result = g_simple_async_result_new_from_error (G_OBJECT (data->miner),
		                                               callback,
		                                               data->user_data,
		                                               error);
	} else {
		result = g_simple_async_result_new (G_OBJECT (data->miner),
		                                    callback,
		                                    data->user_data,
		                                    data->source_function);
	}

	g_simple_async_result_set_op_res_gpointer (result, (gpointer) query_results, NULL);
	g_simple_async_result_complete (result);
	g_object_unref (result);
}

static void
async_call_data_notify_error (AsyncCallData *data,
                              gint           code,
                              const gchar   *message)
{
	TrackerMiner *miner;
	GError *error;

	miner = data->miner;

	if (data->id != 0) {
		tracker_cancel_call (miner->private->client, data->id);
		data->id = 0;
	}

	if (data->callback) {
		error = g_error_new_literal (miner_error_quark, code, message);

		if (data->source_function == tracker_miner_execute_update ||
		    data->source_function == tracker_miner_execute_batch_update ||
		    data->source_function == tracker_miner_commit) {
			async_call_data_update_callback (data, error);
		} else {
			async_call_data_query_callback (data, NULL, error);
		}

		g_error_free (error);
	}
}

static void
sparql_update_cb (GError   *error,
                  gpointer  user_data)
{
	AsyncCallData *data = user_data;

	async_call_data_update_callback (data, error);
	async_call_data_destroy (data, TRUE);

	if (error) {
		g_error_free (error);
	}
}

static void
sparql_query_cb (GPtrArray *result,
                 GError    *error,
                 gpointer   user_data)
{
	AsyncCallData *data = user_data;

	async_call_data_query_callback (data, result, error);

	if (error) {
		g_error_free (error);
	} else {
		if (result) {
			tracker_dbus_results_ptr_array_free (&result);
		}
	}

	async_call_data_destroy (data, TRUE);
}

static void
sparql_cancelled_cb (GCancellable  *cancellable,
                     AsyncCallData *data)
{
	TrackerMinerPrivate *priv;

	async_call_data_notify_error (data, 0, "SPARQL operation was cancelled");

	priv = TRACKER_MINER_GET_PRIVATE (data->miner);

	async_call_data_destroy (data, TRUE);
}

/**
 * tracker_miner_error_quark:
 *
 * Returns the #GQuark used to identify miner errors in GError structures.
 *
 * Returns: the error #GQuark
 **/
GQuark
tracker_miner_error_quark (void)
{
	return g_quark_from_static_string (TRACKER_MINER_ERROR_DOMAIN);
}

/**
 * tracker_miner_start:
 * @miner: a #TrackerMiner
 *
 * Tells the miner to start processing data.
 **/
void
tracker_miner_start (TrackerMiner *miner)
{
	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (miner->private->started == FALSE);

	miner->private->started = TRUE;

	g_signal_emit (miner, signals[STARTED], 0);
}

/**
 * tracker_miner_stop:
 * @miner: a #TrackerMiner
 *
 * Tells the miner to stop processing data.
 **/
void
tracker_miner_stop (TrackerMiner *miner)
{
	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (miner->private->started == TRUE);

	miner->private->started = FALSE;

	g_signal_emit (miner, signals[STOPPED], 0);
}

/**
 * tracker_miner_ignore_next_update:
 * @miner: a #TrackerMiner
 * @urls: the urls to mark as to ignore on next update
 *
 * Tells the miner to mark @urls are to ignore on next update.
 **/
void
tracker_miner_ignore_next_update (TrackerMiner *miner,
                                  const GStrv   urls)
{
	g_return_if_fail (TRACKER_IS_MINER (miner));

	g_signal_emit (miner, signals[IGNORE_NEXT_UPDATE], 0, urls);
}

/**
 * tracker_miner_is_started:
 * @miner: a #TrackerMiner
 *
 * Returns #TRUE if the miner has been started.
 *
 * Returns: #TRUE if the miner is already started.
 **/
gboolean
tracker_miner_is_started (TrackerMiner *miner)
{
	g_return_val_if_fail (TRACKER_IS_MINER (miner), TRUE);

	return miner->private->started;
}

/**
 * tracker_miner_execute_update:
 * @miner: a #TrackerMiner
 * @sparql: a SPARQL query
 * @cancellable: a #GCancellable to control the operation
 * @callback: a #GAsyncReadyCallback to call when the operation is finished
 * @user_data: data to pass to @callback
 *
 * Executes an update SPARQL query on tracker-store, use this
 * whenever you want to perform data insertions or modifications.
 *
 * When the operation is finished, @callback will be called, providing a #GAsyncResult
 * object. Call tracker_miner_execute_sparql_finish on it to get the returned #GError,
 * if there is one.
 *
 * If the operation is cancelled, @callback will be called anyway, with the #GAsyncResult
 * object containing an error.
 **/
void
tracker_miner_execute_update (TrackerMiner        *miner,
                              const gchar         *sparql,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
	TrackerMinerPrivate *priv;
	AsyncCallData *data;

	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (sparql != NULL);
	g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

	priv = TRACKER_MINER_GET_PRIVATE (miner);
	data = async_call_data_new (miner,
	                            cancellable,
	                            callback,
	                            user_data,
	                            tracker_miner_execute_sparql);

	data->id = tracker_resources_sparql_update_async (miner->private->client,
	                                                  sparql,
	                                                  sparql_update_cb,
	                                                  data);
}

/**
 * tracker_miner_execute_update_finish:
 * @miner: a #TrackerMiner
 * @result: a #GAsyncResult
 * @error: a #GError
 *
 * Finishes the async update operation. If an error occured during the update,
 * @error will be set.
 *
 **/
void
tracker_miner_execute_update_finish (TrackerMiner  *miner,
                                     GAsyncResult  *result,
                                     GError       **error)
{
	GSimpleAsyncResult *r = G_SIMPLE_ASYNC_RESULT (result);

	g_simple_async_result_propagate_error (r, error);
}

/**
 * tracker_miner_execute_sparql:
 * @miner: a #TrackerMiner
 * @sparql: a SPARQL query
 * @cancellable: a #GCancellable to control the operation
 * @callback: a #GAsyncReadyCallback to call when the operation is finished
 * @user_data: data to pass to @callback
 *
 * Executes the SPARQL query on tracker-store and returns asynchronously
 * the queried data. Use this whenever you need to get data from
 * already stored information.
 *
 * When the operation is finished, @callback will be called, providing a #GAsyncResult
 * object. Call tracker_miner_execute_sparql_finish on it to get the query results, or
 * the GError object if an error occured.
 *
 * If the operation is cancelled, @callback will be called anyway, with the #GAsyncResult
 * object containing an error.
 **/
void
tracker_miner_execute_sparql (TrackerMiner        *miner,
                              const gchar         *sparql,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
	TrackerMinerPrivate *priv;
	AsyncCallData *data;

	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (sparql != NULL);
	g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));
	g_return_if_fail (callback != NULL);

	priv = TRACKER_MINER_GET_PRIVATE (miner);
	data = async_call_data_new (miner, cancellable, callback, user_data, tracker_miner_execute_sparql);

	data->id = tracker_resources_sparql_query_async (miner->private->client,
	                                                 sparql, sparql_query_cb,
	                                                 data);
}

/**
 * tracker_miner_execute_sparql_finish:
 * @miner: a #TrackerMiner
 * @result: a #GAsyncResult object holding the result of the query
 * @error: a #GError
 *
 * Finishes the async operation and returns the query results. If an error
 * occured during the query, @error will be set.
 *
 * Returns: a #GPtrArray with the sparql results which should not be freed.
 **/
const GPtrArray *
tracker_miner_execute_sparql_finish (TrackerMiner  *miner,
                                     GAsyncResult  *result,
                                     GError       **error)
{
	GSimpleAsyncResult *r = G_SIMPLE_ASYNC_RESULT (result);

	if (g_simple_async_result_propagate_error (r, error)) {
		return NULL;
	}

	return (const GPtrArray*) g_simple_async_result_get_op_res_gpointer (r);
}

/**
 * tracker_miner_execute_batch_update:
 * @miner: a #TrackerMiner
 * @sparql: a set of SPARQL updates
 * @cancellable: a #GCancellable to control the operation
 * @callback: a #GAsyncReadyCallback to call when the operation is finished
 * @user_data: data to pass to @callback
 *
 * Executes a batch of update SPARQL queries on tracker-store, use this
 * whenever you want to perform data insertions or modifications in
 * batches.
 *
 * When the operation is finished, @callback will be called, providing a #GAsyncResult
 * object. Call tracker_miner_execute_batch_update_finish on it to get the returned #GError,
 * if there is one.
 *
 * If the operation is cancelled, @callback will be called anyway, with the #GAsyncResult
 * object containing an error.
 **/
void
tracker_miner_execute_batch_update (TrackerMiner        *miner,
                                    const gchar         *sparql,
                                    GCancellable        *cancellable,
                                    GAsyncReadyCallback  callback,
                                    gpointer             user_data)
{
	TrackerMinerPrivate *priv;
	AsyncCallData *data;

	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (sparql != NULL);
	g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

	priv = TRACKER_MINER_GET_PRIVATE (miner);
	data = async_call_data_new (miner, cancellable, callback, user_data, tracker_miner_execute_batch_update);

	data->id = tracker_resources_batch_sparql_update_async (miner->private->client,
	                                                        sparql, sparql_update_cb,
	                                                        data);
}

/**
 * tracker_miner_execute_batch_update_finish:
 * @miner: a #TrackerMiner
 * @result: a #GAsyncResult
 * @error: a #GError
 *
 * Finishes the async batch update operation. If an error occured during the update,
 * @error will be set.
 *
 **/
void
tracker_miner_execute_batch_update_finish (TrackerMiner *miner,
                                           GAsyncResult *result,
                                           GError      **error)
{
	GSimpleAsyncResult *r = G_SIMPLE_ASYNC_RESULT (result);

	g_simple_async_result_propagate_error (r, error);
}

/**
 * tracker_miner_commit:
 * @miner: a #TrackerMiner
 * @cancellable: a #GCancellable to control the operation
 * @callback: a #GAsyncReadyCallback to call when the operation is finished
 * @user_data: data to pass to @callback
 *
 * Commits all pending batch updates. See tracker_miner_execute_batch_update().
 *
 * When the operation is finished, @callback will be called, providing a #GAsyncResult
 * object. Call tracker_miner_commit_finish on it to get the returned #GError,
 * if there is one.
 *
 * If the operation is cancelled, @callback will be called anyway, with the #GAsyncResult
 * object containing an error.
 **/

void
tracker_miner_commit (TrackerMiner        *miner,
                      GCancellable        *cancellable,
                      GAsyncReadyCallback  callback,
                      gpointer             user_data)

{
	TrackerMinerPrivate *priv;
	AsyncCallData *data;

	g_return_if_fail (TRACKER_IS_MINER (miner));
	g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

	priv = TRACKER_MINER_GET_PRIVATE (miner);
	data = async_call_data_new (miner, cancellable, callback, user_data, tracker_miner_commit);

	data->id = tracker_resources_batch_commit_async (miner->private->client,
	                                                 sparql_update_cb,
	                                                 data);
}

/**
 * tracker_miner_commit_finish:
 * @miner: a #TrackerMiner
 * @result: a #GAsyncResult
 * @error: a #GError
 *
 * Finishes the async comit operation. If an error occured during the commit,
 * @error will be set.
 *
 **/
void
tracker_miner_commit_finish (TrackerMiner  *miner,
                             GAsyncResult  *result,
                             GError       **error)
{
	GSimpleAsyncResult *r = G_SIMPLE_ASYNC_RESULT (result);

	g_simple_async_result_propagate_error (r, error);
}

static gint
tracker_miner_pause_internal (TrackerMiner  *miner,
                              const gchar   *application,
                              const gchar   *reason,
                              GError       **error)
{
	PauseData *pd;
	GHashTableIter iter;
	gpointer key, value;

	/* Check this is not a duplicate pause */
	g_hash_table_iter_init (&iter, miner->private->pauses);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		PauseData *pd = value;

		if (g_strcmp0 (application, pd->application) == 0 &&
		    g_strcmp0 (reason, pd->reason) == 0) {
			/* Can't use duplicate pauses */
			g_set_error_literal (error, TRACKER_MINER_ERROR, 0,
			                     _("Pause application and reason match an already existing pause request"));
			return -1;
		}
	}

	pd = pause_data_new (application, reason);

	g_hash_table_insert (miner->private->pauses,
	                     GINT_TO_POINTER (pd->cookie),
	                     pd);

	if (g_hash_table_size (miner->private->pauses) == 1) {
		/* Pause */
		g_message ("Miner is pausing");
		g_signal_emit (miner, signals[PAUSED], 0);
	}

	return pd->cookie;
}

/**
 * tracker_miner_pause:
 * @miner: a #TrackerMiner
 * @reason: reason to pause
 * @error: return location for errors
 *
 * Asks @miner to pause. On success the cookie ID is returned,
 * this is what must be used in tracker_miner_resume() to resume
 * operations. On failure @error will be set and -1 will be returned.
 *
 * Returns: The pause cookie ID.
 **/
gint
tracker_miner_pause (TrackerMiner  *miner,
                     const gchar   *reason,
                     GError       **error)
{
	const gchar *application;

	g_return_val_if_fail (TRACKER_IS_MINER (miner), -1);
	g_return_val_if_fail (reason != NULL, -1);

	application = g_get_application_name ();

	if (!application) {
		application = miner->private->name;
	}

	return tracker_miner_pause_internal (miner, application, reason, error);
}

/**
 * tracker_miner_resume:
 * @miner: a #TrackerMiner
 * @cookie: pause cookie
 * @error: return location for errors
 *
 * Asks the miner to resume processing. The cookie must be something
 * returned by tracker_miner_pause(). The miner won't actually resume
 * operations until all pause requests have been resumed.
 *
 * Returns: #TRUE if the cookie was valid.
 **/
gboolean
tracker_miner_resume (TrackerMiner  *miner,
                      gint           cookie,
                      GError       **error)
{
	g_return_val_if_fail (TRACKER_IS_MINER (miner), FALSE);

	if (!g_hash_table_remove (miner->private->pauses, GINT_TO_POINTER (cookie))) {
		g_set_error_literal (error, TRACKER_MINER_ERROR, 0,
		                     _("Cookie not recognized to resume paused miner"));
		return FALSE;
	}

	if (g_hash_table_size (miner->private->pauses) == 0) {
		/* Resume */
		g_message ("Miner is resuming");
		g_signal_emit (miner, signals[RESUMED], 0);
	}

	return TRUE;
}

/* DBus methods */
void
_tracker_miner_dbus_get_status (TrackerMiner           *miner,
                                DBusGMethodInvocation  *context,
                                GError                **error)
{
	guint request_id;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);

	tracker_dbus_request_new (request_id, context, "%s()", __PRETTY_FUNCTION__);

	tracker_dbus_request_success (request_id, context);
	dbus_g_method_return (context, miner->private->status);
}

void
_tracker_miner_dbus_get_progress (TrackerMiner           *miner,
                                  DBusGMethodInvocation  *context,
                                  GError                **error)
{
	guint request_id;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);

	tracker_dbus_request_new (request_id, context, "%s()", __PRETTY_FUNCTION__);

	dbus_g_method_return (context, miner->private->progress);

	tracker_dbus_request_success (request_id, context);
}

void
_tracker_miner_dbus_get_pause_details (TrackerMiner           *miner,
                                       DBusGMethodInvocation  *context,
                                       GError                **error)
{
	GSList *applications, *reasons;
	GStrv applications_strv, reasons_strv;
	GHashTableIter iter;
	gpointer key, value;
	guint request_id;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);

	tracker_dbus_request_new (request_id, context, "%s()", __PRETTY_FUNCTION__);

	applications = NULL;
	reasons = NULL;

	g_hash_table_iter_init (&iter, miner->private->pauses);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		PauseData *pd = value;

		applications = g_slist_prepend (applications, pd->application);
		reasons = g_slist_prepend (reasons, pd->reason);
	}

	applications = g_slist_reverse (applications);
	reasons = g_slist_reverse (reasons);

	applications_strv = tracker_gslist_to_string_list (applications);
	reasons_strv = tracker_gslist_to_string_list (reasons);

	tracker_dbus_request_success (request_id, context);
	dbus_g_method_return (context, applications_strv, reasons_strv);

	g_strfreev (applications_strv);
	g_strfreev (reasons_strv);

	g_slist_free (applications);
	g_slist_free (reasons);
}

void
_tracker_miner_dbus_pause (TrackerMiner           *miner,
                           const gchar            *application,
                           const gchar            *reason,
                           DBusGMethodInvocation  *context,
                           GError                **error)
{
	GError *local_error = NULL;
	guint request_id;
	gint cookie;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);
	tracker_dbus_async_return_if_fail (application != NULL, context);
	tracker_dbus_async_return_if_fail (reason != NULL, context);

	tracker_dbus_request_new (request_id, context,
	                          "%s(application:'%s', reason:'%s')",
	                          __PRETTY_FUNCTION__,
	                          application,
	                          reason);

	cookie = tracker_miner_pause_internal (miner, application, reason, &local_error);
	if (cookie == -1) {
		GError *actual_error = NULL;

		tracker_dbus_request_failed (request_id,
		                             context,
		                             &actual_error,
		                             local_error ? local_error->message : NULL);
		dbus_g_method_return_error (context, actual_error);

		g_error_free (actual_error);
		g_error_free (local_error);

		return;
	}

	tracker_dbus_request_success (request_id, context);
	dbus_g_method_return (context, cookie);
}

void
_tracker_miner_dbus_resume (TrackerMiner           *miner,
                            gint                    cookie,
                            DBusGMethodInvocation  *context,
                            GError                **error)
{
	GError *local_error = NULL;
	guint request_id;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);

	tracker_dbus_request_new (request_id,
	                          context,
	                          "%s(cookie:%d)",
	                          __PRETTY_FUNCTION__,
	                          cookie);

	if (!tracker_miner_resume (miner, cookie, &local_error)) {
		GError *actual_error = NULL;

		tracker_dbus_request_failed (request_id,
		                             context,
		                             &actual_error,
		                             local_error ? local_error->message : NULL);
		dbus_g_method_return_error (context, actual_error);

		g_error_free (actual_error);
		g_error_free (local_error);

		return;
	}

	tracker_dbus_request_success (request_id, context);
	dbus_g_method_return (context);
}

void
_tracker_miner_dbus_ignore_next_update (TrackerMiner           *miner,
                                        const GStrv             urls,
                                        DBusGMethodInvocation  *context,
                                        GError                **error)
{
	guint request_id;

	request_id = tracker_dbus_get_next_request_id ();

	tracker_dbus_async_return_if_fail (miner != NULL, context);

	tracker_dbus_request_new (request_id, context, "%s()", __PRETTY_FUNCTION__);

	tracker_miner_ignore_next_update (miner, urls);

	tracker_dbus_request_success (request_id, context);
	dbus_g_method_return (context);
}
