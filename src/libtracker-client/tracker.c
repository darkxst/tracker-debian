/* 
 * Copyright (C) 2006, Jamie McCracken <jamiemcc@gnome.org>
 * Copyright (C) 2008-2010, Nokia <ivan.frade@nokia.com>
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

#include <string.h>

#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>
#include <dbus/dbus-glib-bindings.h>

#include "tracker.h"

#include "tracker-resources-glue.h"
#include "tracker-statistics-glue.h"

/**
 * SECTION:tracker
 * @short_description: A client library for querying and inserting
 * data in Tracker.
 * @include: libtracker-client/tracker-client.h
 *
 * This API is for applications which want to integrate with Tracker
 * either by storing their data or by querying it. They are also not
 * limited to their application's data. Other data mined by other
 * applications is also available in some cases.
 **/

/**
 * SECTION:tracker_cancel
 * @short_description: Cancelling requests.
 * @include: libtracker-client/tracker-client.h
 *
 * Tracker allows you to cancel any request that has not been processed
 * yet. Aditionally, for fully synchronous requests, there is helper
 * API to cancel the last request.
 **/

/**
 * SECTION:tracker_resources
 * @short_description: Doing SPARQL queries to tracker-store.
 * @include: libtracker-client/tracker-client.h
 *
 * Tracker uses the SPARQL query language
 * <footnote><para><ulink url="http://www.w3.org/TR/rdf-sparql-query/">SPARQL</ulink> query language for RDF (W3C)</para></footnote>
 * to retrieve data from tracker-store, and the stored information applies to the Nepomuk
 * ontology
 * <footnote><para><ulink url="http://nepomuk.semanticdesktop.org/">Nepomuk</ulink> - The social semantic desktop</para></footnote>.
 **/

/**
 * SECTION:tracker_statistics
 * @short_description: Data statistics.
 * @include: libtracker-client/tracker-client.h
 *
 * This API is meant to get statistics about the stored data.
 **/

/**
 * SECTION:tracker_misc
 * @short_description: Utility and miscellaneous functions.
 * @include: libtracker-client/tracker-client.h
 *
 * This is miscellaneous API that may be useful to users.
 **/

/**
 * SECTION:tracker_search
 * @short_description: Simple search functions.
 * @include: libtracker-client/tracker-client.h
 *
 * Simple search API.
 **/

typedef struct {
	DBusGProxy *proxy_statistics;
	DBusGProxy *proxy_resources;

	GHashTable *pending_calls;
	guint last_call;

	gint timeout;
	gboolean enable_warnings;

	gboolean is_constructed;
} TrackerClientPrivate;

typedef struct {
	DBusGProxy *proxy;
	DBusGProxyCall *pending_call;
} PendingCallData;

typedef struct {
	TrackerReplyGPtrArray func;
	gpointer data;
	TrackerClient *client;
	guint id;
} CallbackGPtrArray;

typedef struct {
	TrackerReplyVoid func;
	gpointer data;
	TrackerClient *client;
	guint id;
} CallbackVoid;

#ifndef TRACKER_DISABLE_DEPRECATED

/* Deprecated and only used for 0.6 API */
typedef struct {
	TrackerReplyArray func;
	gpointer data;
	TrackerClient *client;
	guint id;
} CallbackArray;

#endif /* TRACKER_DISABLE_DEPRECATED */

static gboolean is_service_available (void);
static void     client_finalize      (GObject      *object);
static void     client_set_property  (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec);
static void     client_get_property  (GObject      *object,
                                      guint         prop_id,
                                      GValue       *value,
                                      GParamSpec   *pspec);
static void     client_constructed   (GObject      *object);

enum {
	PROP_0,
	PROP_ENABLE_WARNINGS,
	PROP_TIMEOUT,
};

static guint pending_call_id = 0;

G_DEFINE_TYPE(TrackerClient, tracker_client, G_TYPE_OBJECT)

#define TRACKER_CLIENT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TRACKER_TYPE_CLIENT, TrackerClientPrivate))

static void
pending_call_free (PendingCallData *data)
{
	g_slice_free (PendingCallData, data);
}

static guint
pending_call_new (TrackerClient  *client,
                  DBusGProxy     *proxy,
                  DBusGProxyCall *pending_call)
{
	TrackerClientPrivate *private;
	PendingCallData *data;
	guint id;

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	id = ++pending_call_id;

	data = g_slice_new (PendingCallData);
	data->proxy = proxy;
	data->pending_call = pending_call;

	g_hash_table_insert (private->pending_calls,
	                     GUINT_TO_POINTER (id),
	                     data);

	private->last_call = id;

	return id;
}

static void
tracker_client_class_init (TrackerClientClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = client_finalize;
	object_class->set_property = client_set_property;
	object_class->get_property = client_get_property;
	object_class->constructed = client_constructed;

	g_object_class_install_property (object_class,
	                                 PROP_ENABLE_WARNINGS,
	                                 g_param_spec_boolean ("enable-warnings",
	                                                       "Enable warnings",
	                                                       "Enable warnings",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
	                                 PROP_TIMEOUT,
	                                 g_param_spec_int ("timeout",
	                                                   "Timeout",
	                                                   "Timeout",
	                                                   -1,
	                                                   G_MAXINT,
	                                                   -1,
	                                                   G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_type_class_add_private (object_class, sizeof (TrackerClientPrivate));
}

static void
tracker_client_init (TrackerClient *client)
{
	TrackerClientPrivate *private = TRACKER_CLIENT_GET_PRIVATE (client);

	private->timeout = -1;
	private->pending_calls = g_hash_table_new_full (NULL, NULL, NULL,
	                                               (GDestroyNotify) pending_call_free);
}

static void
client_finalize (GObject *object)
{
	TrackerClientPrivate *private = TRACKER_CLIENT_GET_PRIVATE (object);

	if (private->proxy_statistics) {
		g_object_unref (private->proxy_statistics);
	}

	if (private->proxy_resources) {
		g_object_unref (private->proxy_resources);
	}

	if (private->pending_calls) {
		g_hash_table_unref (private->pending_calls);
	}
}

static void
client_set_property (GObject      *object,
                     guint         prop_id,
                     const GValue *value,
                     GParamSpec   *pspec)
{
	TrackerClientPrivate *private = TRACKER_CLIENT_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_ENABLE_WARNINGS:
		private->enable_warnings = g_value_get_boolean (value);
		g_object_notify (object, "enable_warnings");
		break;
	case PROP_TIMEOUT:
		private->timeout = g_value_get_int (value);

		/* Sanity check timeout */
		if (private->timeout == 0) {
			/* Can't use 0, no D-Bus calls are ever quick
			 * enough :) which is quite funny.
			 */
			private->timeout = -1;
		}

		if (private->is_constructed) {
			dbus_g_proxy_set_default_timeout (private->proxy_resources, 
			                                  private->timeout);
		}

		g_object_notify (object, "timeout");

		break;
	default:

		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
client_get_property (GObject    *object,
                     guint       prop_id,
                     GValue     *value,
                     GParamSpec *pspec)
{
	TrackerClientPrivate *private = TRACKER_CLIENT_GET_PRIVATE (object);

	switch (prop_id) {
	case PROP_ENABLE_WARNINGS:
		g_value_set_boolean (value, private->enable_warnings);
		break;
	case PROP_TIMEOUT:
		g_value_set_int (value, private->timeout);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
	}
}

static void
client_constructed (GObject *object)
{
	TrackerClientPrivate *private;
	DBusGConnection *connection;
	GError *error = NULL;

	private = TRACKER_CLIENT_GET_PRIVATE (object);
	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	if (!connection || error) {
		if (private->enable_warnings) {
			g_warning ("Could not connect to D-Bus session bus, %s\n",
			           error ? error->message : "no error given");
		}

		g_clear_error (&error);
		return;
	}

	private->proxy_statistics =
		dbus_g_proxy_new_for_name (connection,
		                           TRACKER_DBUS_SERVICE,
		                           TRACKER_DBUS_OBJECT "/Statistics",
		                           TRACKER_DBUS_INTERFACE_STATISTICS);

	private->proxy_resources =
		dbus_g_proxy_new_for_name (connection,
		                           TRACKER_DBUS_SERVICE,
		                           TRACKER_DBUS_OBJECT "/Resources",
		                           TRACKER_DBUS_INTERFACE_RESOURCES);

	/* NOTE: We don't need to set this for the stats proxy, the
	 * query takes no arguments and is generally really fast.
	 */
	dbus_g_proxy_set_default_timeout (private->proxy_resources, 
	                                  private->timeout);

	private->is_constructed = TRUE;
}

static void
callback_with_gptrarray (DBusGProxy *proxy,
                         GPtrArray  *OUT_result,
                         GError     *error,
                         gpointer    user_data)
{
	TrackerClientPrivate *private;
	CallbackGPtrArray *cb = user_data;

	private = TRACKER_CLIENT_GET_PRIVATE (cb->client);
	g_hash_table_remove (private->pending_calls,
	                     GUINT_TO_POINTER (cb->id));

	(*(TrackerReplyGPtrArray) cb->func) (OUT_result, error, cb->data);

	g_object_unref (cb->client);
	g_slice_free (CallbackGPtrArray, cb);
}

static void
callback_with_void (DBusGProxy *proxy,
                    GError     *error,
                    gpointer    user_data)
{
	TrackerClientPrivate *private;
	CallbackVoid *cb = user_data;

	private = TRACKER_CLIENT_GET_PRIVATE (cb->client);
	g_hash_table_remove (private->pending_calls,
	                     GUINT_TO_POINTER (cb->id));
	
	(*(TrackerReplyVoid) cb->func) (error, cb->data);

	g_object_unref (cb->client);
	g_slice_free (CallbackVoid, cb);
}

/* Deprecated and only used for 0.6 API */
static void
callback_with_array (DBusGProxy *proxy,
                     GPtrArray  *OUT_result,
                     GError     *error,
                     gpointer    user_data)
{
	TrackerClientPrivate *private;
	CallbackArray *cb = user_data;
	gchar **uris;
	gint i;

	private = TRACKER_CLIENT_GET_PRIVATE (cb->client);
	g_hash_table_remove (private->pending_calls,
	                     GUINT_TO_POINTER (cb->id));
	
	uris = g_new0 (gchar *, OUT_result->len + 1);
	for (i = 0; i < OUT_result->len; i++) {
		uris[i] = ((gchar **) OUT_result->pdata[i])[0];
	}
	
	(*(TrackerReplyArray) cb->func) (uris, error, cb->data);
	
	g_ptr_array_foreach (OUT_result, (GFunc) g_free, NULL);
	g_ptr_array_free (OUT_result, TRUE);
	
	g_object_unref (cb->client);
	g_slice_free (CallbackArray, cb);
}

static gboolean
is_service_available (void)
{
	GError *error = NULL;
	DBusGConnection *conn;
	DBusGProxy *proxy;
	GStrv result, p;
	gboolean found = FALSE;

	conn = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

	if (!conn) {
		g_critical ("Could not connect to the D-Bus session bus, %s",
		            error ? error->message : "no error given.");
		g_error_free (error);
		return FALSE;
	}

	proxy =	dbus_g_proxy_new_for_name (conn,
		                           DBUS_SERVICE_DBUS,
		                           DBUS_PATH_DBUS,
		                           DBUS_INTERFACE_DBUS);

	if (!proxy) {
		g_critical ("Could not create a proxy for the Freedesktop service, %s",
		            error ? error->message : "no error given.");
		g_error_free (error);
		return FALSE;
	}

	org_freedesktop_DBus_list_activatable_names (proxy, &result, &error);
	g_object_unref (proxy);

	if (error) {
		g_critical ("Could not start Tracker service '%s', %s",
		            TRACKER_DBUS_SERVICE,
		            error ? error->message : "no error given");
		g_clear_error (&error);

		return FALSE;
	}

	if (!result) {
		return FALSE;
	}

	for (p = result; *p && !found; p++) {
		if (strcmp (*p, TRACKER_DBUS_SERVICE) == 0) {
			found = TRUE;
		}
	}

	g_strfreev (result);

	return found;
}

/**
 * tracker_sparql_escape:
 * @str: a string to escape.
 *
 * Escapes a string so it can be passed as a SPARQL parameter in
 * any query/update.
 *
 * Returns: the newly allocated escaped string which must be freed
 * using g_free().
 *
 * Since: 0.8
 *
 **/
gchar *
tracker_sparql_escape (const gchar *str)
{
	gchar  *escaped_string;
	const gchar *p;
	gchar *q;

	g_return_val_if_fail (str != NULL, NULL);

	escaped_string = g_malloc (2 * strlen (str) + 1);

	p = str;
	q = escaped_string;

	while (*p != '\0') {
		switch (*p) {
		case '\t':
			*q++ = '\\';
			*q++ = 't';
			break;
		case '\n':
			*q++ = '\\';
			*q++ = 'n';
			break;
		case '\r':
			*q++ = '\\';
			*q++ = 'r';
			break;
		case '\b':
			*q++ = '\\';
			*q++ = 'b';
			break;
		case '\f':
			*q++ = '\\';
			*q++ = 'f';
			break;
		case '"':
			*q++ = '\\';
			*q++ = '"';
			break;
		case '\\':
			*q++ = '\\';
			*q++ = '\\';
			break;
		default:
			*q++ = *p;
			break;
		}
		p++;
	}
	*q = '\0';

	return escaped_string;
}

static const char *
find_conversion (const char  *format,
                 const char **after)
{
	const char *start = format;
	const char *cp;

	while (*start != '\0' && *start != '%')
		start++;

	if (*start == '\0') {
		*after = start;
		return NULL;
	}

	cp = start + 1;

	if (*cp == '\0') {
		*after = cp;
		return NULL;
	}

	/* Test for positional argument.  */
	if (*cp >= '0' && *cp <= '9') {
		const char *np;

		for (np = cp; *np >= '0' && *np <= '9'; np++)
			;
		if (*np == '$')
			cp = np + 1;
	}

	/* Skip the flags.  */
	for (;;) {
		if (*cp == '\'' ||
		    *cp == '-' ||
		    *cp == '+' ||
		    *cp == ' ' ||
		    *cp == '#' ||
		    *cp == '0')
			cp++;
		else
			break;
	}

	/* Skip the field width.  */
	if (*cp == '*') {
		cp++;

		/* Test for positional argument.  */
		if (*cp >= '0' && *cp <= '9') {
			const char *np;

			for (np = cp; *np >= '0' && *np <= '9'; np++)
				;
			if (*np == '$')
				cp = np + 1;
		}
	} else {
		for (; *cp >= '0' && *cp <= '9'; cp++)
			;
	}

	/* Skip the precision.  */
	if (*cp == '.') {
		cp++;
		if (*cp == '*') {
			/* Test for positional argument.  */
			if (*cp >= '0' && *cp <= '9') {
				const char *np;

				for (np = cp; *np >= '0' && *np <= '9'; np++)
					;
				if (*np == '$')
					cp = np + 1;
			}
		} else {
			for (; *cp >= '0' && *cp <= '9'; cp++)
				;
		}
	}

	/* Skip argument type/size specifiers.  */
	while (*cp == 'h' ||
	       *cp == 'L' ||
	       *cp == 'l' ||
	       *cp == 'j' ||
	       *cp == 'z' ||
	       *cp == 'Z' ||
	       *cp == 't')
		cp++;

	/* Skip the conversion character.  */
	cp++;

	*after = cp;
	return start;
}

/**
 * tracker_uri_vprintf_escaped:
 * @format: a standard printf() format string, but notice
 *     <link linkend="string-precision">string precision pitfalls</link>
 * @args: the list of parameters to insert into the format string
 * 
 * Similar to the standard C vsprintf() function but safer, since it 
 * calculates the maximum space required and allocates memory to hold 
 * the result. 
 *
 * The result is escaped using g_uri_escape_string().
 *
 * Returns: a newly-allocated string holding the result which should
 * be freed with g_free() when finished with.
 *
 * Since: 0.8
 */
gchar *
tracker_uri_vprintf_escaped (const gchar *format,
                             va_list      args)
{
	GString *format1;
	GString *format2;
	GString *result = NULL;
	gchar *output1 = NULL;
	gchar *output2 = NULL;
	const char *p;
	gchar *op1, *op2;
	va_list args2;

	format1 = g_string_new (NULL);
	format2 = g_string_new (NULL);
	p = format;
	while (TRUE) {
		const char *after;
		const char *conv = find_conversion (p, &after);
		if (!conv)
			break;

		g_string_append_len (format1, conv, after - conv);
		g_string_append_c (format1, 'X');
		g_string_append_len (format2, conv, after - conv);
		g_string_append_c (format2, 'Y');

		p = after;
	}

	/* Use them to format the arguments
	 */
	G_VA_COPY (args2, args);

	output1 = g_strdup_vprintf (format1->str, args);
	va_end (args);
	if (!output1) {
		va_end (args2);
		goto cleanup;
        }

	output2 = g_strdup_vprintf (format2->str, args2);
	va_end (args2);
	if (!output2)
		goto cleanup;

	result = g_string_new (NULL);

	op1 = output1;
	op2 = output2;
	p = format;
	while (TRUE) {
		const char *after;
		const char *output_start;
		const char *conv = find_conversion (p, &after);
		char *escaped;

		if (!conv) {
			g_string_append_len (result, p, after - p);
			break;
		}

		g_string_append_len (result, p, conv - p);
		output_start = op1;
		while (*op1 == *op2) {
			op1++;
			op2++;
		}

		*op1 = '\0';
		escaped = g_uri_escape_string (output_start, NULL, FALSE);
		g_string_append (result, escaped);
		g_free (escaped);

		p = after;
		op1++;
		op2++;
	}

 cleanup:
	g_string_free (format1, TRUE);
	g_string_free (format2, TRUE);
	g_free (output1);
	g_free (output2);

	if (result)
		return g_string_free (result, FALSE);
	else
		return NULL;
}

/**
 * tracker_uri_printf_escaped:
 * @format: a standard printf() format string, but notice
 *     <link linkend="string-precision">string precision pitfalls</link>
 * @Varargs: the parameters to insert into the format string
 *
 * Calls tracker_uri_vprintf_escaped() with the @Varargs supplied.
 
 * Returns: a newly-allocated string holding the result which should
 * be freed with g_free() when finished with.
 *
 * Since: 0.8
 **/
gchar *
tracker_uri_printf_escaped (const gchar *format, ...)
{
	gchar *result;
	va_list args;

	va_start (args, format);
	result = tracker_uri_vprintf_escaped (format, args);
	va_end (args);

	return result;
}

/**
 * tracker_client_new:
 * @flags: This can be one or more combinations of #TrackerClientFlags
 * @timeout: a #gint used for D-Bus call timeouts.
 *
 * Creates a connection over D-Bus to the Tracker store for doing data
 * querying and inserting.
 *
 * The @timeout is only used if it is > 0. If it is, then it is used
 * with dbus_g_proxy_set_default_timeout().
 *
 * Returns: the #TrackerClient which should be freed with
 * g_object_unref() when finished with.
 **/
TrackerClient *
tracker_client_new (TrackerClientFlags flags,
                    gint               timeout)
{
	gboolean enable_warnings;

	g_type_init ();

	if (!is_service_available ()) {
		return NULL;
	}

	enable_warnings = (flags & TRACKER_CLIENT_ENABLE_WARNINGS);

	return g_object_new (TRACKER_TYPE_CLIENT,
	                     "enable-warnings", enable_warnings,
	                     "timeout", timeout,
	                     NULL);
}

/**
 * tracker_cancel_call:
 * @client: a #TrackerClient.
 * @call_id: a #guint id for the API call you want to cancel.
 *
 * The @call_id is a #guint which increments with each asynchronous
 * API call made using libtracker-client. For synchronous API calls,
 * see tracker_cancel_last_call() which is more useful.
 *
 * Returns: A @gboolean indicating if the call was cancelled or not.
 **/
gboolean
tracker_cancel_call (TrackerClient *client,
                     guint          call_id)
{
	TrackerClientPrivate *private;
	PendingCallData *data;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (call_id >= 1, FALSE);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	data = g_hash_table_lookup (private->pending_calls,
	                            GUINT_TO_POINTER (call_id));

	if (!data) {
		return FALSE;
	}

	dbus_g_proxy_cancel_call (data->proxy, data->pending_call);
	g_hash_table_remove (private->pending_calls,
	                     GUINT_TO_POINTER (call_id));

	return TRUE;
}

/**
 * tracker_cancel_last_call:
 * @client: a #TrackerClient.
 *
 * Cancels the last API call made using tracker_cancel_call(). the
 * last API call ID is always tracked so you don't have to provide it
 * with this API.
 *
 * Returns: A #gboolean indicating if the call was cancelled or not.
 **/
gboolean
tracker_cancel_last_call (TrackerClient *client)
{
	TrackerClientPrivate *private;
	gboolean cancelled;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), FALSE);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	g_return_val_if_fail (private->last_call != 0, FALSE);

	cancelled = tracker_cancel_call (client, private->last_call);
	private->last_call = 0;
	
	return cancelled;
}

/**
 * tracker_statistics_get:
 * @client: a #TrackerClient.
 * @error: a #GError.
 *
 * Requests statistics about each class in the ontology (for example,
 * nfo:Image and nmm:Photo which indicate the number of images and the
 * number of photos).
 *
 * The returned #GPtrArray contains an array of #GStrv which have 2
 * strings. The first is the class (e.g. nfo:Image), the second is the
 * count for that class.
 *
 * This API call is completely synchronous so it may block.
 *
 * Returns: A #GPtrArray with the statistics which must be freed using
 * g_ptr_array_free().
 *
 * Since: 0.8
 **/
GPtrArray *
tracker_statistics_get (TrackerClient  *client,
                        GError        **error)
{
	TrackerClientPrivate *private;
	GPtrArray *table;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), NULL);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	if (!org_freedesktop_Tracker1_Statistics_get (private->proxy_statistics,
	                                              &table,
	                                              error)) {
		return NULL;
	}

	return table;
}

void
tracker_resources_load (TrackerClient  *client,
                        const gchar    *uri,
                        GError        **error)
{
	TrackerClientPrivate *private;

	g_return_if_fail (TRACKER_IS_CLIENT (client));
	g_return_if_fail (uri != NULL);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	org_freedesktop_Tracker1_Resources_load (private->proxy_resources,
	                                         uri,
	                                         error);
}

/**
 * tracker_resources_sparql_query:
 * @client: a #TrackerClient.
 * @query: a string representing SPARQL.
 * @error: a #GError.
 *
 * Queries the database using SPARQL. An example query would be:
 *
 * <example>
 * <title>Using tracker_resource_sparql_query(<!-- -->)</title>
 * An example of using tracker_resource_sparql_query() to list all
 * albums by title and include their song count and song total length.
 * <programlisting>
 *  TrackerClient *client;
 *  GPtrArray *array;
 *  GError *error = NULL;
 *  const gchar *query;
 *
 *  /&ast; Create D-Bus connection with no warnings and maximum timeout. &ast;/
 *  client = tracker_client_new (0, G_MAXINT);
 *  query = "SELECT {"
 *          "  ?album"
 *          "  ?title"
 *          "  COUNT(?song) AS songs"
 *          "  SUM(?length) AS totallength"
 *          "} WHERE {"
 *          "  ?album a nmm:MusicAlbum ;"
 *          "  nie:title ?title ."
 *          "  ?song nmm:musicAlbum ?album ;"
 *          "  nfo:duration ?length"
 *          "} "
 *          "GROUP BY (?album");
 *
 *  array = tracker_resources_sparql_query (client, query, &error);
 *
 *  if (error) {
 *          g_warning ("Could not query Tracker, %s", error->message);
 *          g_error_free (error);
 *          g_object_unref (client);
 *          return;
 *  }
 *
 *  /&ast; Do something with the array &ast;/
 *
 *  g_ptr_array_free (array, TRUE);
 * </programlisting>
 * </example>
 *
 * This API call is completely synchronous so it may block.
 *
 * Returns: A #GPtrArray with the query results which must be freed
 * using g_ptr_array_free().
 *
 * Since: 0.8
 **/
GPtrArray *
tracker_resources_sparql_query (TrackerClient  *client,
                                const gchar    *query,
                                GError        **error)
{
	TrackerClientPrivate *private;
	GPtrArray *table;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), FALSE);
	g_return_val_if_fail (query != NULL, FALSE);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	if (!org_freedesktop_Tracker1_Resources_sparql_query (private->proxy_resources,
	                                                      query,
	                                                      &table,
	                                                      error)) {
		return NULL;
	}

	return table;
}

/**
 * tracker_resources_sparql_update:
 * @client: a #TrackerClient.
 * @query: a string representing SPARQL.
 * @error: a #GError.
 *
 * Updates the database using SPARQL.
 *
 * This API behaves the same way tracker_resources_sparql_query() does
 * but with the difference that it is intended to be used for data
 * updates.
 *
 * This API call is completely synchronous so it may block.
 *
 * Since: 0.8
 **/
void
tracker_resources_sparql_update (TrackerClient  *client,
                                 const gchar    *query,
                                 GError        **error)
{
	TrackerClientPrivate *private;

	g_return_if_fail (TRACKER_IS_CLIENT (client));
	g_return_if_fail (query != NULL);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	org_freedesktop_Tracker1_Resources_sparql_update (private->proxy_resources,
	                                                  query,
	                                                  error);
}

GPtrArray *
tracker_resources_sparql_update_blank (TrackerClient  *client,
                                       const gchar    *query,
                                       GError        **error)
{
	TrackerClientPrivate *private;
	GPtrArray *result;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), NULL);
	g_return_val_if_fail (query != NULL, NULL);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	if (!org_freedesktop_Tracker1_Resources_sparql_update_blank (private->proxy_resources,
	                                                             query,
	                                                             &result,
	                                                             error)) {
		return NULL;
	}

	return result;
}

/**
 * tracker_resources_batch_sparql_update:
 * @client: a #TrackerClient.
 * @query: a string representing SPARQL.
 * @error: return location for errors.
 *
 * Updates the database using SPARQL. Updates done this way have to be committed
 * explicitly through tracker_resources_batch_commit() or
 * tracker_resources_batch_commit_async(). This API call is synchronous so it may
 * block.
 *
 * Since: 0.8
 **/
void
tracker_resources_batch_sparql_update (TrackerClient  *client,
                                       const gchar    *query,
                                       GError        **error)
{
	TrackerClientPrivate *private;

	g_return_if_fail (TRACKER_IS_CLIENT (client));
	g_return_if_fail (query != NULL);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	org_freedesktop_Tracker1_Resources_batch_sparql_update (private->proxy_resources,
	                                                        query,
	                                                        error);
}

/**
 * tracker_resources_batch_commit:
 * @client: a #TrackerClient.
 * @error: return location for errors.
 *
 * Commits a batch of already issued SPARQL updates. This API call is
 * synchronous so it may block.
 *
 * Since: 0.8
 **/
void
tracker_resources_batch_commit (TrackerClient  *client,
                                GError        **error)
{
	TrackerClientPrivate *private;

	g_return_if_fail (TRACKER_IS_CLIENT (client));

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	org_freedesktop_Tracker1_Resources_batch_commit (private->proxy_resources,
	                                                 error);
}

/**
 * tracker_statistics_get_async:
 * @client: a #TrackerClient.
 * @callback: a #TrackerReplyGPtrArray to be used when the data is
 * available.
 * @user_data: user data to pass to @callback.
 *
 * This behaves exactly as tracker_statistics_get() but asynchronously.
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Since: 0.8
 **/
guint
tracker_statistics_get_async (TrackerClient         *client,
                              TrackerReplyGPtrArray  callback,
                              gpointer               user_data)
{
	TrackerClientPrivate *private;
	CallbackGPtrArray *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackGPtrArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Statistics_get_async (private->proxy_statistics,
	                                                      callback_with_gptrarray,
	                                                      cb);

	cb->id = pending_call_new (client, private->proxy_statistics, call);

	return cb->id;
}

guint
tracker_resources_load_async (TrackerClient    *client,
                              const gchar      *uri,
                              TrackerReplyVoid  callback,
                              gpointer          user_data)
{
	TrackerClientPrivate *private;
	CallbackVoid *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (uri != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackVoid);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_load_async (private->proxy_resources,
	                                                      uri,
	                                                      callback_with_void,
	                                                      cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

/**
 * tracker_resources_sparql_query_async:
 * @client: a #TrackerClient
 * @query: a string representing SPARQL.
 * @callback: callback function to be called when the data is ready.
 * @user_data: user data to pass to @callback
 *
 * Does an asynchronous SPARQL query. See tracker_resources_sparql_query()
 * to see how an SPARLQL query should be like.
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Since: 0.8
 **/
guint
tracker_resources_sparql_query_async (TrackerClient         *client,
                                      const gchar           *query,
                                      TrackerReplyGPtrArray  callback,
                                      gpointer               user_data)
{
	TrackerClientPrivate *private;
	CallbackGPtrArray *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackGPtrArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_sparql_query_async (private->proxy_resources,
	                                                              query,
	                                                              callback_with_gptrarray,
	                                                              cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

/**
 * tracker_resources_sparql_update_async:
 * @client: a #TrackerClient.
 * @query: a string representing an SPARQL update.
 * @callback: callback function to be called when the update has been processed.
 * @user_data: user data to pass to @callback.
 *
 * Does an asynchronous SPARQL update.
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Since: 0.8
 **/
guint
tracker_resources_sparql_update_async (TrackerClient    *client,
                                       const gchar      *query,
                                       TrackerReplyVoid  callback,
                                       gpointer          user_data)
{
	TrackerClientPrivate *private;
	CallbackVoid *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackVoid);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_sparql_update_async (private->proxy_resources,
	                                                               query,
	                                                               callback_with_void,
	                                                               cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

guint
tracker_resources_sparql_update_blank_async (TrackerClient         *client,
                                             const gchar           *query,
                                             TrackerReplyGPtrArray  callback,
                                             gpointer               user_data)
{
	TrackerClientPrivate *private;
	CallbackGPtrArray *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackGPtrArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_sparql_update_blank_async (private->proxy_resources,
	                                                                     query,
	                                                                     callback_with_gptrarray,
	                                                                     callback);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

/**
 * tracker_resources_batch_sparql_update_async:
 * @client: a #TrackerClient.
 * @query: a string representing SPARQL.
 * @callback: function to be called when the batch update has been performed.
 * @user_data: user data to pass to @callback.
 *
 * Updates the database using SPARQL. see tracker_resources_batch_sparql_update().
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Since: 0.8
 **/
guint
tracker_resources_batch_sparql_update_async (TrackerClient    *client,
                                             const gchar      *query,
                                             TrackerReplyVoid  callback,
                                             gpointer          user_data)
{
	TrackerClientPrivate *private;
	CallbackVoid *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackVoid);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_batch_sparql_update_async (private->proxy_resources,
	                                                                     query,
	                                                                     callback_with_void,
	                                                                     cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

/**
 * tracker_resources_batch_commit_async:
 * @client: a #TrackerClient.
 * @callback: callback to be called when the operation is finished.
 * @user_data: user data to pass to @callback.
 *
 * Commits a batch of already issued SPARQL updates.
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Since: 0.8
 **/
guint
tracker_resources_batch_commit_async (TrackerClient    *client,
                                      TrackerReplyVoid  callback,
                                      gpointer          user_data)
{
	TrackerClientPrivate *private;
	CallbackVoid *cb;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackVoid);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	call = org_freedesktop_Tracker1_Resources_batch_commit_async (private->proxy_resources,
	                                                              callback_with_void,
	                                                              cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	return cb->id;
}

/* tracker_search_metadata_by_text_async is used by GTK+ */

static void
sparql_append_string_literal (GString     *sparql,
                              const gchar *str)
{
	g_string_append_c (sparql, '"');

	while (*str != '\0') {
		gsize len = strcspn (str, "\t\n\r\"\\");
		g_string_append_len (sparql, str, len);
		str += len;
		switch (*str) {
		case '\t':
			g_string_append (sparql, "\\t");
			break;
		case '\n':
			g_string_append (sparql, "\\n");
			break;
		case '\r':
			g_string_append (sparql, "\\r");
			break;
		case '"':
			g_string_append (sparql, "\\\"");
			break;
		case '\\':
			g_string_append (sparql, "\\\\");
			break;
		default:
			continue;
		}
		str++;
	}

	g_string_append_c (sparql, '"');
}

#ifndef TRACKER_DISABLE_DEPRECATED

/**
 * tracker_connect:
 * @enable_warnings: a #gboolean to determine if warnings are issued in
 * cases where they are found.
 * @timeout: a #gint used for D-Bus call timeouts.
 *
 * This function calls tracker_client_new().
 *
 * Deprecated: 0.8: Use tracker_client_new() instead.
 *
 * Returns: a #TrackerClient #GObject which must be freed with
 * g_object_unref().
 **/
TrackerClient *
tracker_connect (gboolean enable_warnings,
                 gint     timeout)
{
	TrackerClientFlags flags = 0;

	if (enable_warnings) {
		flags |= TRACKER_CLIENT_ENABLE_WARNINGS;
	}

	return tracker_client_new (flags, timeout);
}

/**
 * tracker_disconnect:
 * @client: a #TrackerClient.
 *
 * This will disconnect the D-Bus connections to Tracker services and
 * free the allocated #TrackerClient by tracker_connect().
 *
 * Deprecated: 0.8: Use g_object_unref() instead.
 **/
void
tracker_disconnect (TrackerClient *client)
{
	g_return_if_fail (TRACKER_IS_CLIENT (client));

	g_object_unref (client);
}

/**
 * tracker_search_metadata_by_text_async:
 * @client: a #TrackerClient.
 * @query: a string representing what to search for.
 * @callback: callback function to be called when the update has been processed.
 * @user_data: user data to pass to @callback.
 *
 * Searches for @query in all URIs with the prefix @location.
 *
 * NOTE: @query is found using FTS (Full Text Search).
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Deprecated: 0.8: Use tracker_resources_sparql_query() instead.
 **/
guint
tracker_search_metadata_by_text_async (TrackerClient     *client,
                                       const gchar       *query,
                                       TrackerReplyArray  callback,
                                       gpointer           user_data)
{
	TrackerClientPrivate *private;
	CallbackArray *cb;
	GString *sparql;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	sparql = g_string_new ("SELECT nie:url (?file) WHERE { ?file a nfo:FileDataObject ; fts:match ");
	sparql_append_string_literal (sparql, query);
	g_string_append (sparql, " }");

	call = org_freedesktop_Tracker1_Resources_sparql_query_async (private->proxy_resources,
	                                                              sparql->str,
	                                                              callback_with_array,
	                                                              cb);
	cb->id = pending_call_new (client, private->proxy_resources, call);

 	g_string_free (sparql, TRUE);

	return cb->id;
}

/**
 * tracker_search_metadata_by_text_and_location_async:
 * @client: a #TrackerClient.
 * @query: a string representing what to search for.
 * @location: a string representing a path.
 * @callback: callback function to be called when the update has been processed.
 * @user_data: user data to pass to @callback.
 *
 * Searches for @query in all URIs with the prefix @location.
 *
 * NOTE: @query is found using FTS (Full Text Search).
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Deprecated: 0.8: Use tracker_resources_sparql_query() instead.
 **/
guint
tracker_search_metadata_by_text_and_location_async (TrackerClient     *client,
                                                    const gchar       *query,
                                                    const gchar       *location,
                                                    TrackerReplyArray  callback,
                                                    gpointer           user_data)
{
	TrackerClientPrivate *private;
	CallbackArray *cb;
	GString *sparql;
	DBusGProxyCall *call;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (location != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	sparql = g_string_new ("SELECT nie:url (?file) WHERE { ?file a nfo:FileDataObject ; fts:match ");
	sparql_append_string_literal (sparql, query);
	g_string_append (sparql, " . FILTER (fn:starts-with(nie:url (?file),");
	sparql_append_string_literal (sparql, location);
	g_string_append (sparql, ")) }");

	call = org_freedesktop_Tracker1_Resources_sparql_query_async (private->proxy_resources,
	                                                              sparql->str,
	                                                              callback_with_array,
	                                                              cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	g_string_free (sparql, TRUE);

	return cb->id;
}

/**
 * tracker_search_metadata_by_text_and_mime_async:
 * @client: a #TrackerClient.
 * @query: a string representing what to search for.
 * @mimes: a #GStrv representing mime types.
 * @callback: callback function to be called when the update has been processed.
 * @user_data: user data to pass to @callback.
 *
 * Searches for @query in all URIs with a mime type matching any of
 * the values in @mime.
 *
 * NOTE: @query is found using FTS (Full Text Search).
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Deprecated: 0.8: Use tracker_resources_sparql_query() instead.
 **/
guint
tracker_search_metadata_by_text_and_mime_async (TrackerClient      *client,
                                                const gchar        *query,
                                                const gchar       **mimes,
                                                TrackerReplyArray   callback,
                                                gpointer            user_data)
{
	TrackerClientPrivate *private;
	CallbackArray *cb;
	GString *sparql;
	DBusGProxyCall *call;
	gint i;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (mimes != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	sparql = g_string_new ("SELECT nie:url (?file) WHERE { ?file a nfo:FileDataObject ; nie:mimeType ?mime ; fts:match ");
	sparql_append_string_literal (sparql, query);
	g_string_append (sparql, " . FILTER (");

	for (i = 0; mimes[i]; i++) {
		if (i > 0) {
			g_string_append (sparql, " || ");
		}

		g_string_append (sparql, "?mime = ");
		sparql_append_string_literal (sparql, mimes[i]);
	}
	g_string_append (sparql, ") }");

	call = org_freedesktop_Tracker1_Resources_sparql_query_async (private->proxy_resources,
	                                                              sparql->str,
	                                                              callback_with_array,
	                                                              cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	g_string_free (sparql, TRUE);

	return cb->id;
}

/**
 * tracker_search_metadata_by_text_and_mime_and_location_async:
 * @client: a #TrackerClient.
 * @query: a string representing what to search for.
 * @mimes: a #GStrv representing mime types.
 * @location: a string representing a path.
 * @callback: callback function to be called when the update has been processed.
 * @user_data: user data to pass to @callback.
 *
 * Searches for @query in all URIs with the prefix @location and with
 * a mime type matching any of the values in @mime.
 *
 * NOTE: @query is found using FTS (Full Text Search).
 *
 * Returns: A #guint representing the operation ID. See
 * tracker_cancel_call(). In the event of failure, 0 is returned.
 *
 * Deprecated: 0.8: Use tracker_resources_sparql_query() instead.
 **/
guint
tracker_search_metadata_by_text_and_mime_and_location_async (TrackerClient      *client,
                                                             const gchar        *query,
                                                             const gchar       **mimes,
                                                             const gchar        *location,
                                                             TrackerReplyArray   callback,
                                                             gpointer            user_data)
{
	TrackerClientPrivate *private;
	CallbackArray *cb;
	GString *sparql;
	DBusGProxyCall *call;
	gint i;

	g_return_val_if_fail (TRACKER_IS_CLIENT (client), 0);
	g_return_val_if_fail (query != NULL, 0);
	g_return_val_if_fail (mimes != NULL, 0);
	g_return_val_if_fail (location != NULL, 0);
	g_return_val_if_fail (callback != NULL, 0);

	private = TRACKER_CLIENT_GET_PRIVATE (client);

	cb = g_slice_new0 (CallbackArray);
	cb->func = callback;
	cb->data = user_data;
	cb->client = g_object_ref (client);

	sparql = g_string_new ("SELECT nie:url (?file) WHERE { ?file a nfo:FileDataObject ; nie:mimeType ?mime ; fts:match ");
	sparql_append_string_literal (sparql, query);

	g_string_append (sparql, " . FILTER (fn:starts-with(nie:url (?file),");
	sparql_append_string_literal (sparql, location);

	g_string_append (sparql, ")");
	g_string_append (sparql, " && (");

	for (i = 0; mimes[i]; i++) {
		if (i > 0) {
			g_string_append (sparql, " || ");
		}

		g_string_append (sparql, "?mime = ");
		sparql_append_string_literal (sparql, mimes[i]);
	}

	g_string_append (sparql, ")");
	g_string_append (sparql, ") }");

	call = org_freedesktop_Tracker1_Resources_sparql_query_async (private->proxy_resources,
	                                                              sparql->str,
	                                                              callback_with_array,
	                                                              cb);

	cb->id = pending_call_new (client, private->proxy_resources, call);

	g_string_free (sparql, TRUE);

	return cb->id;
}

#endif /* TRACKER_DISABLE_DEPRECATED */
