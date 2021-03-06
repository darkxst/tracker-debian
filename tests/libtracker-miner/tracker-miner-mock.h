/* tracker-miner-mock.h generated by valac 0.24.0.131-42e78, the Vala compiler, do not modify */


#ifndef __TRACKER_MINER_MOCK_H__
#define __TRACKER_MINER_MOCK_H__

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include "libtracker-sparql/tracker-sparql.h"
#include <gio/gio.h>

G_BEGIN_DECLS


#define TYPE_TRACKER_MINER_MOCK (tracker_miner_mock_get_type ())
#define TRACKER_MINER_MOCK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRACKER_MINER_MOCK, TrackerMinerMock))
#define TRACKER_MINER_MOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRACKER_MINER_MOCK, TrackerMinerMockClass))
#define IS_TRACKER_MINER_MOCK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRACKER_MINER_MOCK))
#define IS_TRACKER_MINER_MOCK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRACKER_MINER_MOCK))
#define TRACKER_MINER_MOCK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRACKER_MINER_MOCK, TrackerMinerMockClass))

typedef struct _TrackerMinerMock TrackerMinerMock;
typedef struct _TrackerMinerMockClass TrackerMinerMockClass;
typedef struct _TrackerMinerMockPrivate TrackerMinerMockPrivate;

#define TYPE_TRACKER_MOCK_RESULTS (tracker_mock_results_get_type ())
#define TRACKER_MOCK_RESULTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRACKER_MOCK_RESULTS, TrackerMockResults))
#define TRACKER_MOCK_RESULTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRACKER_MOCK_RESULTS, TrackerMockResultsClass))
#define IS_TRACKER_MOCK_RESULTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRACKER_MOCK_RESULTS))
#define IS_TRACKER_MOCK_RESULTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRACKER_MOCK_RESULTS))
#define TRACKER_MOCK_RESULTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRACKER_MOCK_RESULTS, TrackerMockResultsClass))

typedef struct _TrackerMockResults TrackerMockResults;
typedef struct _TrackerMockResultsClass TrackerMockResultsClass;
typedef struct _TrackerMockResultsPrivate TrackerMockResultsPrivate;

#define TYPE_TRACKER_MOCK_CONNECTION (tracker_mock_connection_get_type ())
#define TRACKER_MOCK_CONNECTION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRACKER_MOCK_CONNECTION, TrackerMockConnection))
#define TRACKER_MOCK_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRACKER_MOCK_CONNECTION, TrackerMockConnectionClass))
#define IS_TRACKER_MOCK_CONNECTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_TRACKER_MOCK_CONNECTION))
#define IS_TRACKER_MOCK_CONNECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_TRACKER_MOCK_CONNECTION))
#define TRACKER_MOCK_CONNECTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRACKER_MOCK_CONNECTION, TrackerMockConnectionClass))

typedef struct _TrackerMockConnection TrackerMockConnection;
typedef struct _TrackerMockConnectionClass TrackerMockConnectionClass;
typedef struct _TrackerMockConnectionPrivate TrackerMockConnectionPrivate;

struct _TrackerMinerMock {
	GObject parent_instance;
	TrackerMinerMockPrivate * priv;
	gboolean is_paused;
};

struct _TrackerMinerMockClass {
	GObjectClass parent_class;
};

struct _TrackerMockResults {
	TrackerSparqlCursor parent_instance;
	TrackerMockResultsPrivate * priv;
};

struct _TrackerMockResultsClass {
	TrackerSparqlCursorClass parent_class;
};

struct _TrackerMockConnection {
	TrackerSparqlConnection parent_instance;
	TrackerMockConnectionPrivate * priv;
};

struct _TrackerMockConnectionClass {
	TrackerSparqlConnectionClass parent_class;
};


GType tracker_miner_mock_get_type (void) G_GNUC_CONST;
TrackerMinerMock* tracker_miner_mock_new (const gchar* name);
TrackerMinerMock* tracker_miner_mock_construct (GType object_type, const gchar* name);
void tracker_miner_mock_set_paused (TrackerMinerMock* self, gboolean paused);
gboolean tracker_miner_mock_get_paused (TrackerMinerMock* self);
void tracker_miner_mock_pause (TrackerMinerMock* self, const gchar* app, const gchar* reason);
void tracker_miner_mock_resume (TrackerMinerMock* self);
const gchar* tracker_miner_mock_get_pause_reason (TrackerMinerMock* self);
void tracker_miner_mock_set_pause_reason (TrackerMinerMock* self, const gchar* value);
const gchar* tracker_miner_mock_get_name (TrackerMinerMock* self);
void tracker_miner_mock_set_name (TrackerMinerMock* self, const gchar* value);
gchar** tracker_miner_mock_get_apps (TrackerMinerMock* self, int* result_length1);
gchar** tracker_miner_mock_get_reasons (TrackerMinerMock* self, int* result_length1);
GType tracker_mock_results_get_type (void) G_GNUC_CONST;
TrackerMockResults* tracker_mock_results_new (gchar** results, int results_length1, int results_length2, gint rows, gint cols, gchar** var_names, int var_names_length1, TrackerSparqlValueType* types, int types_length1);
TrackerMockResults* tracker_mock_results_construct (GType object_type, gchar** results, int results_length1, int results_length2, gint rows, gint cols, gchar** var_names, int var_names_length1, TrackerSparqlValueType* types, int types_length1);
GType tracker_mock_connection_get_type (void) G_GNUC_CONST;
void tracker_mock_connection_set_results (TrackerMockConnection* self, TrackerMockResults* results);
TrackerMockConnection* tracker_mock_connection_new (void);
TrackerMockConnection* tracker_mock_connection_construct (GType object_type);


G_END_DECLS

#endif
