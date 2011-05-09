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
#include <libtracker-common/tracker-file-utils.h>
#include <libtracker-common/tracker-log.h>
#include <libtracker-common/tracker-utils.h>

#include "tracker-crawler.h"
#include "tracker-marshal.h"
#include "tracker-miner-fs.h"
#include "tracker-monitor.h"
#include "tracker-utils.h"
#include "tracker-thumbnailer.h"
#include "tracker-miner-fs-processing-pool.h"

/* If defined will print the tree from GNode while running */
#ifdef CRAWLED_TREE_ENABLE_TRACE
#warning Tree debugging traces enabled
#endif /* CRAWLED_TREE_ENABLE_TRACE */

/* If defined will print contents of populated IRI cache while running */
#ifdef IRI_CACHE_ENABLE_TRACE
#warning IRI cache contents traces enabled
#endif /* IRI_CACHE_ENABLE_TRACE */

/* If defined will print contents of populated mtime cache while running */
#ifdef MTIME_CACHE_ENABLE_TRACE
#warning MTIME cache contents traces enabled
#endif /* MTIME_CACHE_ENABLE_TRACE */

/* If defined will print push/pop actions on queues */
#ifdef EVENT_QUEUE_ENABLE_TRACE
#warning Event Queue traces enabled
#define EVENT_QUEUE_LOG_PREFIX "[Event Queues] "
#define EVENT_QUEUE_STATUS_TIMEOUT_SECS 30
#define trace_eq(message, ...) g_debug (EVENT_QUEUE_LOG_PREFIX message, ##__VA_ARGS__)
#define trace_eq_action(pushed, queue_name, position, gfile1, gfile2, reason) \
	do { \
		gchar *uri1 = g_file_get_uri (gfile1); \
		gchar *uri2 = gfile2 ? g_file_get_uri (gfile2) : NULL; \
		g_debug ("%s%s '%s%s%s' %s %s of queue '%s'%s%s", \
		         EVENT_QUEUE_LOG_PREFIX, \
		         pushed ? "Pushed" : "Popped", \
		         uri1, \
		         uri2 ? "->" : "", \
		         uri2 ? uri2 : "", \
		         pushed ? "to" : "from", \
		         position, \
		         queue_name, \
		         reason ? ": " : "", \
		         reason ? reason : ""); \
		g_free (uri1); \
		g_free (uri2); \
	} while (0)
#define trace_eq_push_tail(queue_name, gfile, reason)	  \
	trace_eq_action (TRUE, queue_name, "tail", gfile, NULL, reason)
#define trace_eq_push_head(queue_name, gfile, reason)	  \
	trace_eq_action (TRUE, queue_name, "head", gfile, NULL, reason)
#define trace_eq_push_tail_2(queue_name, gfile1, gfile2, reason)	  \
	trace_eq_action (TRUE, queue_name, "tail", gfile1, gfile2, reason)
#define trace_eq_push_head_2(queue_name, gfile1, gfile2, reason)	  \
	trace_eq_action (TRUE, queue_name, "head", gfile1, gfile2, reason)
#define trace_eq_pop_head(queue_name, gfile)	  \
	trace_eq_action (FALSE, queue_name, "head", gfile, NULL, NULL)
#define trace_eq_pop_head_2(queue_name, gfile1, gfile2)	  \
	trace_eq_action (FALSE, queue_name, "head", gfile1, gfile2, NULL)
static gboolean miner_fs_queues_status_trace_timeout_cb (gpointer data);
#else
#define trace_eq(...)
#define trace_eq_push_tail(...)
#define trace_eq_push_head(...)
#define trace_eq_push_tail_2(...)
#define trace_eq_push_head_2(...)
#define trace_eq_pop_head(...)
#define trace_eq_pop_head_2(...)
#endif /* EVENT_QUEUE_ENABLE_TRACE */

/* Default processing pool limits to be set */
#define DEFAULT_WAIT_POOL_LIMIT 1
#define DEFAULT_READY_POOL_LIMIT 1
#define DEFAULT_N_REQUESTS_POOL_LIMIT 10

/**
 * SECTION:tracker-miner-fs
 * @short_description: Abstract base class for filesystem miners
 * @include: libtracker-miner/tracker-miner.h
 *
 * #TrackerMinerFS is an abstract base class for miners that collect data
 * from the filesystem, all the filesystem crawling and monitoring is
 * abstracted away, leaving to implementations the decisions of what
 * directories/files should it process, and the actual data extraction.
 **/

#define TRACKER_MINER_FS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TRACKER_TYPE_MINER_FS, TrackerMinerFSPrivate))

typedef struct {
	GFile *file;
	GFile *source_file;
} ItemMovedData;

typedef struct {
	GFile    *file;
	guint     recurse : 1;
	guint     ref_count : 7;
} DirectoryData;

typedef struct {
	gchar *urn;
	gchar *parent_urn;
	GCancellable *cancellable;
	TrackerSparqlBuilder *builder;
	TrackerMiner *miner;
} UpdateProcessingTaskContext;

typedef struct {
	GMainLoop *main_loop;
	const gchar *uri;
	gchar *iri;
	gchar *mime;
	gboolean get_mime;
} ItemQueryExistsData;

typedef struct {
	GMainLoop *main_loop;
	GHashTable *values;
} CacheQueryData;

typedef struct {
	GMainLoop *main_loop;
	GString   *sparql;
	const gchar *source_uri;
	const gchar *uri;
} RecursiveMoveData;

typedef struct {
	GNode *tree;
	GQueue *nodes;
	guint n_items;
	guint n_items_processed;
} CrawledDirectoryData;

struct _TrackerMinerFSPrivate {
	TrackerMonitor *monitor;
	TrackerCrawler *crawler;

	GQueue         *crawled_directories;

	/* File queues for indexer */
	GQueue         *items_created;
	GQueue         *items_updated;
	GQueue         *items_deleted;
	GQueue         *items_moved;
#ifdef EVENT_QUEUE_ENABLE_TRACE
	guint           queue_status_timeout_id;
#endif /* EVENT_QUEUE_ENABLE_TRACE */

	GHashTable     *items_ignore_next_update;

	GQuark          quark_ignore_file;
	GQuark          quark_attribute_updated;
	GQuark          quark_check_existence;

	GList          *config_directories;

	GList          *directories;
	DirectoryData  *current_directory;

	GTimer         *timer;

	guint           crawl_directories_id;
	guint           item_queues_handler_id;

	gdouble         throttle;

	TrackerProcessingPool *processing_pool;

	/* URI mtime cache */
	GFile          *current_mtime_cache_parent;
	GHashTable     *mtime_cache;

	/* File -> iri cache */
	GFile          *current_iri_cache_parent;
	gchar          *current_iri_cache_parent_urn;
	GHashTable     *iri_cache;

	GList          *dirs_without_parent;

	/* Files to check if no longer exist */
	GHashTable     *check_removed;

	/* Status */
	guint           been_started : 1;     /* TRUE if miner has been started */
	guint           been_crawled : 1;     /* TRUE if initial crawling has been
	                                       * done */
	guint           shown_totals : 1;     /* TRUE if totals have been shown */
	guint           is_paused : 1;        /* TRUE if miner is paused */
	guint           is_crawling : 1;      /* TRUE if currently is crawling
	                                       * (initial or non-initial) */
	guint           mtime_checking : 1;   /* TRUE if mtime checks should be done
	                                       * during initial crawling. */
	guint           initial_crawling : 1; /* TRUE if initial crawling should be
	                                       * done */

	/* Statistics */
	guint           total_directories_found;
	guint           total_directories_ignored;
	guint           total_files_found;
	guint           total_files_ignored;

	guint           directories_found;
	guint           directories_ignored;
	guint           files_found;
	guint           files_ignored;

	guint           total_files_processed;
	guint           total_files_notified;
	guint           total_files_notified_error;
};

typedef enum {
	QUEUE_NONE,
	QUEUE_CREATED,
	QUEUE_UPDATED,
	QUEUE_DELETED,
	QUEUE_MOVED,
	QUEUE_IGNORE_NEXT_UPDATE,
	QUEUE_WAIT
} QueueState;

enum {
	CHECK_FILE,
	CHECK_DIRECTORY,
	CHECK_DIRECTORY_CONTENTS,
	MONITOR_DIRECTORY,
	PROCESS_FILE,
	PROCESS_FILE_ATTRIBUTES,
	IGNORE_NEXT_UPDATE_FILE,
	FINISHED,
	LAST_SIGNAL
};

enum {
	PROP_0,
	PROP_THROTTLE,
	PROP_WAIT_POOL_LIMIT,
	PROP_READY_POOL_LIMIT,
	PROP_N_REQUESTS_POOL_LIMIT,
	PROP_MTIME_CHECKING,
	PROP_INITIAL_CRAWLING
};

static void           fs_finalize                         (GObject              *object);
static void           fs_set_property                     (GObject              *object,
                                                           guint                 prop_id,
                                                           const GValue         *value,
                                                           GParamSpec           *pspec);
static void           fs_get_property                     (GObject              *object,
                                                           guint                 prop_id,
                                                           GValue               *value,
                                                           GParamSpec           *pspec);
static gboolean       fs_defaults                         (TrackerMinerFS       *fs,
                                                           GFile                *file);
static gboolean       fs_contents_defaults                (TrackerMinerFS       *fs,
                                                           GFile                *parent,
                                                           GList                *children);
static void           miner_started                       (TrackerMiner         *miner);
static void           miner_stopped                       (TrackerMiner         *miner);
static void           miner_paused                        (TrackerMiner         *miner);
static void           miner_resumed                       (TrackerMiner         *miner);
static void           miner_ignore_next_update            (TrackerMiner         *miner,
                                                           const GStrv           subjects);
static DirectoryData *directory_data_new                  (GFile                *file,
                                                           gboolean              recurse);
static DirectoryData *directory_data_ref                  (DirectoryData        *dd);
static void           directory_data_unref                (DirectoryData        *dd);
static ItemMovedData *item_moved_data_new                 (GFile                *file,
                                                           GFile                *source_file);
static void           item_moved_data_free                (ItemMovedData        *data);
static void           monitor_item_created_cb             (TrackerMonitor       *monitor,
                                                           GFile                *file,
                                                           gboolean              is_directory,
                                                           gpointer              user_data);
static void           monitor_item_updated_cb             (TrackerMonitor       *monitor,
                                                           GFile                *file,
                                                           gboolean              is_directory,
                                                           gpointer              user_data);
static void           monitor_item_attribute_updated_cb   (TrackerMonitor       *monitor,
                                                           GFile                *file,
                                                           gboolean              is_directory,
                                                           gpointer              user_data);
static void           monitor_item_deleted_cb             (TrackerMonitor       *monitor,
                                                           GFile                *file,
                                                           gboolean              is_directory,
                                                           gpointer              user_data);
static void           monitor_item_moved_cb               (TrackerMonitor       *monitor,
                                                           GFile                *file,
                                                           GFile                *other_file,
                                                           gboolean              is_directory,
                                                           gboolean              is_source_monitored,
                                                           gpointer              user_data);
static gboolean       crawler_check_file_cb               (TrackerCrawler       *crawler,
                                                           GFile                *file,
                                                           gpointer              user_data);
static gboolean       crawler_check_directory_cb          (TrackerCrawler       *crawler,
                                                           GFile                *file,
                                                           gpointer              user_data);
static gboolean       crawler_check_directory_contents_cb (TrackerCrawler       *crawler,
                                                           GFile                *parent,
                                                           GList                *children,
                                                           gpointer              user_data);
static void           crawler_directory_crawled_cb        (TrackerCrawler       *crawler,
                                                           GFile                *directory,
                                                           GNode                *tree,
                                                           guint                 directories_found,
                                                           guint                 directories_ignored,
                                                           guint                 files_found,
                                                           guint                 files_ignored,
                                                           gpointer              user_data);
static void           crawler_finished_cb                 (TrackerCrawler       *crawler,
                                                           gboolean              was_interrupted,
                                                           gpointer              user_data);
static void           crawl_directories_start             (TrackerMinerFS       *fs);
static void           crawl_directories_stop              (TrackerMinerFS       *fs);
static void           item_queue_handlers_set_up          (TrackerMinerFS       *fs);
static void           item_update_children_uri            (TrackerMinerFS       *fs,
                                                           RecursiveMoveData    *data,
                                                           const gchar          *source_uri,
                                                           const gchar          *uri);
static void           crawled_directory_data_free         (CrawledDirectoryData *data);

static gboolean       should_recurse_for_directory            (TrackerMinerFS *fs,
                                                               GFile          *file);
static void           tracker_miner_fs_directory_add_internal (TrackerMinerFS *fs,
                                                               GFile          *file);
static gboolean       miner_fs_has_children_without_parent (TrackerMinerFS *fs,
                                                            GFile          *file);

static guint signals[LAST_SIGNAL] = { 0, };

G_DEFINE_ABSTRACT_TYPE (TrackerMinerFS, tracker_miner_fs, TRACKER_TYPE_MINER)

static void
tracker_miner_fs_class_init (TrackerMinerFSClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	TrackerMinerFSClass *fs_class = TRACKER_MINER_FS_CLASS (klass);
	TrackerMinerClass *miner_class = TRACKER_MINER_CLASS (klass);

	object_class->finalize = fs_finalize;
	object_class->set_property = fs_set_property;
	object_class->get_property = fs_get_property;

	miner_class->started = miner_started;
	miner_class->stopped = miner_stopped;
	miner_class->paused  = miner_paused;
	miner_class->resumed = miner_resumed;
	miner_class->ignore_next_update = miner_ignore_next_update;

	fs_class->check_file        = fs_defaults;
	fs_class->check_directory   = fs_defaults;
	fs_class->monitor_directory = fs_defaults;
	fs_class->check_directory_contents = fs_contents_defaults;

	g_object_class_install_property (object_class,
	                                 PROP_THROTTLE,
	                                 g_param_spec_double ("throttle",
	                                                      "Throttle",
	                                                      "Modifier for the indexing speed, 0 is max speed",
	                                                      0, 1, 0,
	                                                      G_PARAM_READWRITE));
	g_object_class_install_property (object_class,
	                                 PROP_WAIT_POOL_LIMIT,
	                                 g_param_spec_uint ("processing-pool-wait-limit",
	                                                    "Processing pool limit for WAIT tasks",
	                                                    "Maximum number of files that can be concurrently "
	                                                    "processed by the upper layer",
	                                                    1, G_MAXUINT, DEFAULT_WAIT_POOL_LIMIT,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
	                                 PROP_READY_POOL_LIMIT,
	                                 g_param_spec_uint ("processing-pool-ready-limit",
	                                                    "Processing pool limit for READY tasks",
	                                                    "Maximum number of SPARQL updates that can be merged "
	                                                    "in a single connection to the store",
	                                                    1, G_MAXUINT, DEFAULT_READY_POOL_LIMIT,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
	                                 PROP_N_REQUESTS_POOL_LIMIT,
	                                 g_param_spec_uint ("processing-pool-requests-limit",
	                                                    "Processing pool limit for number of requests",
	                                                    "Maximum number of SPARQL requests that can be sent "
	                                                    "to the store in parallel.",
	                                                    1, G_MAXUINT, DEFAULT_N_REQUESTS_POOL_LIMIT,
	                                                    G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
	                                 PROP_MTIME_CHECKING,
	                                 g_param_spec_boolean ("mtime-checking",
	                                                       "Mtime checking",
	                                                       "Whether to perform mtime checks during initial crawling or not",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
	g_object_class_install_property (object_class,
	                                 PROP_INITIAL_CRAWLING,
	                                 g_param_spec_boolean ("initial-crawling",
	                                                       "Initial crawling",
	                                                       "Whether to perform initial crawling or not",
	                                                       TRUE,
	                                                       G_PARAM_READWRITE));

	/**
	 * TrackerMinerFS::check-file:
	 * @miner_fs: the #TrackerMinerFS
	 * @file: a #GFile
	 *
	 * The ::check-file signal is emitted either on the filesystem crawling
	 * phase or whenever a new file appears in a monitored directory
	 * in order to check whether @file must be inspected my @miner_fs.
	 *
	 * Returns: %TRUE if @file must be inspected.
	 *
	 * Since: 0.8
	 **/
	signals[CHECK_FILE] =
		g_signal_new ("check-file",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, check_file),
		              tracker_accumulator_check_file,
		              NULL,
		              tracker_marshal_BOOLEAN__OBJECT,
		              G_TYPE_BOOLEAN, 1, G_TYPE_FILE);
	/**
	 * TrackerMinerFS::check-directory:
	 * @miner_fs: the #TrackerMinerFS
	 * @directory: a #GFile
	 *
	 * The ::check-directory signal is emitted either on the filesystem crawling
	 * phase or whenever a new directory appears in a monitored directory
	 * in order to check whether @directory must be inspected my @miner_fs.
	 *
	 * Returns: %TRUE if @directory must be inspected.
	 *
	 * Since: 0.8
	 **/
	signals[CHECK_DIRECTORY] =
		g_signal_new ("check-directory",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, check_directory),
		              tracker_accumulator_check_file,
		              NULL,
		              tracker_marshal_BOOLEAN__OBJECT,
		              G_TYPE_BOOLEAN, 1, G_TYPE_FILE);
	/**
	 * TrackerMinerFS::check-directory-contents:
	 * @miner_fs: the #TrackerMinerFS
	 * @directory: a #GFile
	 * @children: #GList of #GFile<!-- -->s
	 *
	 * The ::check-directory-contents signal is emitted either on the filesystem
	 * crawling phase or whenever a new directory appears in a monitored directory
	 * in order to check whether @directory must be inspected my @miner_fs based on
	 * the directory contents, for some implementations this signal may be useful
	 * to discard backup directories for example.
	 *
	 * Returns: %TRUE if @directory must be inspected.
	 *
	 * Since: 0.8
	 **/
	signals[CHECK_DIRECTORY_CONTENTS] =
		g_signal_new ("check-directory-contents",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, check_directory_contents),
		              tracker_accumulator_check_file,
		              NULL,
		              tracker_marshal_BOOLEAN__OBJECT_POINTER,
		              G_TYPE_BOOLEAN, 2, G_TYPE_FILE, G_TYPE_POINTER);
	/**
	 * TrackerMinerFS::monitor-directory:
	 * @miner_fs: the #TrackerMinerFS
	 * @directory: a #GFile
	 *
	 * The ::monitor-directory is emitted either on the filesystem crawling phase
	 * or whenever a new directory appears in a monitored directory in order to
	 * check whether @directory must be monitored for filesystem changes or not.
	 *
	 * Returns: %TRUE if the directory must be monitored for changes.
	 *
	 * Since: 0.8
	 **/
	signals[MONITOR_DIRECTORY] =
		g_signal_new ("monitor-directory",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, monitor_directory),
		              tracker_accumulator_check_file,
		              NULL,
		              tracker_marshal_BOOLEAN__OBJECT,
		              G_TYPE_BOOLEAN, 1, G_TYPE_FILE);
	/**
	 * TrackerMinerFS::process-file:
	 * @miner_fs: the #TrackerMinerFS
	 * @file: a #GFile
	 * @builder: a #TrackerSparqlBuilder
	 * @cancellable: a #GCancellable
	 *
	 * The ::process-file signal is emitted whenever a file should
	 * be processed, and it's metadata extracted.
	 *
	 * @builder is the #TrackerSparqlBuilder where all sparql updates
	 * to be performed for @file will be appended.
	 *
	 * This signal allows both synchronous and asynchronous extraction,
	 * in the synchronous case @cancellable can be safely ignored. In
	 * either case, on successful metadata extraction, implementations
	 * must call tracker_miner_fs_file_notify() to indicate that
	 * processing has finished on @file, so the miner can execute
	 * the SPARQL updates and continue processing other files.
	 *
	 * Returns: %TRUE if the file is accepted for processing,
	 *          %FALSE if the file should be ignored.
	 *
	 * Since: 0.8
	 **/
	signals[PROCESS_FILE] =
		g_signal_new ("process-file",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, process_file),
		              NULL, NULL,
		              tracker_marshal_BOOLEAN__OBJECT_OBJECT_OBJECT,
		              G_TYPE_BOOLEAN,
		              3, G_TYPE_FILE, TRACKER_SPARQL_TYPE_BUILDER, G_TYPE_CANCELLABLE);

	/**
	 * TrackerMinerFS::process-file-attributes:
	 * @miner_fs: the #TrackerMinerFS
	 * @file: a #GFile
	 * @builder: a #TrackerSparqlBuilder
	 * @cancellable: a #GCancellable
	 *
	 * The ::process-file-attributes signal is emitted whenever a file should
	 * be processed, but only the attribute-related metadata extracted.
	 *
	 * @builder is the #TrackerSparqlBuilder where all sparql updates
	 * to be performed for @file will be appended. For the properties being
	 * updated, the DELETE statements should be included as well.
	 *
	 * This signal allows both synchronous and asynchronous extraction,
	 * in the synchronous case @cancellable can be safely ignored. In
	 * either case, on successful metadata extraction, implementations
	 * must call tracker_miner_fs_file_notify() to indicate that
	 * processing has finished on @file, so the miner can execute
	 * the SPARQL updates and continue processing other files.
	 *
	 * Returns: %TRUE if the file is accepted for processing,
	 *          %FALSE if the file should be ignored.
	 *
	 * Since: 0.10
	 **/
	signals[PROCESS_FILE_ATTRIBUTES] =
		g_signal_new ("process-file-attributes",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, process_file_attributes),
		              NULL, NULL,
		              tracker_marshal_BOOLEAN__OBJECT_OBJECT_OBJECT,
		              G_TYPE_BOOLEAN,
		              3, G_TYPE_FILE, TRACKER_SPARQL_TYPE_BUILDER, G_TYPE_CANCELLABLE);

	/**
	 * TrackerMinerFS::ignore-next-update-file:
	 * @miner_fs: the #TrackerMinerFS
	 * @file: a #GFile
	 * @builder: a #TrackerSparqlBuilder
	 * @cancellable: a #GCancellable
	 *
	 * The ::ignore-next-update-file signal is emitted whenever a file should
	 * be marked as to ignore on next update, and it's metadata prepared for that.
	 *
	 * @builder is the #TrackerSparqlBuilder where all sparql updates
	 * to be performed for @file will be appended.
	 *
	 * Returns: %TRUE on success
	 *          %FALSE on failure
	 *
	 * Since: 0.8
	 **/
	signals[IGNORE_NEXT_UPDATE_FILE] =
		g_signal_new ("ignore-next-update-file",
		              G_OBJECT_CLASS_TYPE (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, ignore_next_update_file),
		              NULL, NULL,
		              tracker_marshal_BOOLEAN__OBJECT_OBJECT_OBJECT,
		              G_TYPE_BOOLEAN,
		              3, G_TYPE_FILE, TRACKER_SPARQL_TYPE_BUILDER, G_TYPE_CANCELLABLE);

	/**
	 * TrackerMinerFS::finished:
	 * @miner_fs: the #TrackerMinerFS
	 * @elapsed: elapsed time since mining was started
	 * @directories_found: number of directories found
	 * @directories_ignored: number of ignored directories
	 * @files_found: number of files found
	 * @files_ignored: number of ignored files
	 *
	 * The ::finished signal is emitted when @miner_fs has finished
	 * all pending processing.
	 *
	 * Since: 0.8
	 **/
	signals[FINISHED] =
		g_signal_new ("finished",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (TrackerMinerFSClass, finished),
		              NULL, NULL,
		              tracker_marshal_VOID__DOUBLE_UINT_UINT_UINT_UINT,
		              G_TYPE_NONE,
		              5,
		              G_TYPE_DOUBLE,
		              G_TYPE_UINT,
		              G_TYPE_UINT,
		              G_TYPE_UINT,
		              G_TYPE_UINT);

	g_type_class_add_private (object_class, sizeof (TrackerMinerFSPrivate));
}

static void
tracker_miner_fs_init (TrackerMinerFS *object)
{
	TrackerMinerFSPrivate *priv;

	object->private = TRACKER_MINER_FS_GET_PRIVATE (object);

	priv = object->private;

	priv->crawled_directories = g_queue_new ();
	priv->items_created = g_queue_new ();
	priv->items_updated = g_queue_new ();
	priv->items_deleted = g_queue_new ();
	priv->items_moved = g_queue_new ();

#ifdef EVENT_QUEUE_ENABLE_TRACE
	priv->queue_status_timeout_id = g_timeout_add_seconds (EVENT_QUEUE_STATUS_TIMEOUT_SECS,
	                                                       miner_fs_queues_status_trace_timeout_cb,
	                                                       object);
#endif /* PROCESSING_POOL_ENABLE_TRACE */

	priv->items_ignore_next_update = g_hash_table_new_full (g_str_hash, g_str_equal,
	                                                        (GDestroyNotify) g_free,
	                                                        (GDestroyNotify) NULL);

	/* Create processing pool */
	priv->processing_pool = tracker_processing_pool_new (object,
	                                                     DEFAULT_WAIT_POOL_LIMIT,
	                                                     DEFAULT_READY_POOL_LIMIT,
	                                                     DEFAULT_N_REQUESTS_POOL_LIMIT);

	/* Set up the crawlers now we have config and hal */
	priv->crawler = tracker_crawler_new ();

	g_signal_connect (priv->crawler, "check-file",
	                  G_CALLBACK (crawler_check_file_cb),
	                  object);
	g_signal_connect (priv->crawler, "check-directory",
	                  G_CALLBACK (crawler_check_directory_cb),
	                  object);
	g_signal_connect (priv->crawler, "check-directory-contents",
	                  G_CALLBACK (crawler_check_directory_contents_cb),
	                  object);
	g_signal_connect (priv->crawler, "directory-crawled",
	                  G_CALLBACK (crawler_directory_crawled_cb),
	                  object);
	g_signal_connect (priv->crawler, "finished",
	                  G_CALLBACK (crawler_finished_cb),
	                  object);

	/* Set up the monitor */
	priv->monitor = tracker_monitor_new ();

	g_signal_connect (priv->monitor, "item-created",
	                  G_CALLBACK (monitor_item_created_cb),
	                  object);
	g_signal_connect (priv->monitor, "item-updated",
	                  G_CALLBACK (monitor_item_updated_cb),
	                  object);
	g_signal_connect (priv->monitor, "item-attribute-updated",
	                  G_CALLBACK (monitor_item_attribute_updated_cb),
	                  object);
	g_signal_connect (priv->monitor, "item-deleted",
	                  G_CALLBACK (monitor_item_deleted_cb),
	                  object);
	g_signal_connect (priv->monitor, "item-moved",
	                  G_CALLBACK (monitor_item_moved_cb),
	                  object);

	priv->quark_ignore_file = g_quark_from_static_string ("tracker-ignore-file");
	priv->quark_check_existence = g_quark_from_static_string ("tracker-check-existence");
	priv->quark_attribute_updated = g_quark_from_static_string ("tracker-attribute-updated");

	priv->iri_cache = g_hash_table_new_full (g_file_hash,
	                                         (GEqualFunc) g_file_equal,
	                                         (GDestroyNotify) g_object_unref,
	                                         (GDestroyNotify) g_free);

	priv->check_removed = g_hash_table_new_full (g_file_hash,
	                                             (GEqualFunc) g_file_equal,
	                                             (GDestroyNotify) g_object_unref,
	                                             NULL);

	priv->mtime_checking = TRUE;
	priv->initial_crawling = TRUE;
	priv->dirs_without_parent = NULL;
}

static void
fs_finalize (GObject *object)
{
	TrackerMinerFSPrivate *priv;

	priv = TRACKER_MINER_FS_GET_PRIVATE (object);

	if (priv->timer) {
		g_timer_destroy (priv->timer);
	}

	if (priv->item_queues_handler_id) {
		g_source_remove (priv->item_queues_handler_id);
		priv->item_queues_handler_id = 0;
	}

	crawl_directories_stop (TRACKER_MINER_FS (object));

	g_object_unref (priv->crawler);
	g_object_unref (priv->monitor);

	g_free (priv->current_iri_cache_parent_urn);
	if (priv->current_iri_cache_parent)
		g_object_unref (priv->current_iri_cache_parent);
	if (priv->current_mtime_cache_parent)
		g_object_unref (priv->current_mtime_cache_parent);

	if (priv->directories) {
		g_list_foreach (priv->directories, (GFunc) directory_data_unref, NULL);
		g_list_free (priv->directories);
	}

	if (priv->config_directories) {
		g_list_foreach (priv->config_directories, (GFunc) directory_data_unref, NULL);
		g_list_free (priv->config_directories);
	}

	g_queue_foreach (priv->crawled_directories, (GFunc) crawled_directory_data_free, NULL);
	g_queue_free (priv->crawled_directories);

	tracker_processing_pool_free (priv->processing_pool);

	g_queue_foreach (priv->items_moved, (GFunc) item_moved_data_free, NULL);
	g_queue_free (priv->items_moved);

	g_queue_foreach (priv->items_deleted, (GFunc) g_object_unref, NULL);
	g_queue_free (priv->items_deleted);

	g_queue_foreach (priv->items_updated, (GFunc) g_object_unref, NULL);
	g_queue_free (priv->items_updated);

	g_queue_foreach (priv->items_created, (GFunc) g_object_unref, NULL);
	g_queue_free (priv->items_created);

	g_list_foreach (priv->dirs_without_parent, (GFunc) g_object_unref, NULL);
	g_list_free (priv->dirs_without_parent);

	g_hash_table_unref (priv->items_ignore_next_update);

	if (priv->mtime_cache) {
		g_hash_table_unref (priv->mtime_cache);
	}

	if (priv->iri_cache) {
		g_hash_table_unref (priv->iri_cache);
	}

	if (priv->check_removed) {
		g_hash_table_unref (priv->check_removed);
	}

#ifdef EVENT_QUEUE_ENABLE_TRACE
	if (priv->queue_status_timeout_id)
		g_source_remove (priv->queue_status_timeout_id);
#endif /* PROCESSING_POOL_ENABLE_TRACE */

	G_OBJECT_CLASS (tracker_miner_fs_parent_class)->finalize (object);
}

static void
fs_set_property (GObject      *object,
                 guint         prop_id,
                 const GValue *value,
                 GParamSpec   *pspec)
{
	TrackerMinerFS *fs = TRACKER_MINER_FS (object);

	switch (prop_id) {
	case PROP_THROTTLE:
		tracker_miner_fs_set_throttle (TRACKER_MINER_FS (object),
		                               g_value_get_double (value));
		break;
	case PROP_WAIT_POOL_LIMIT:
		tracker_processing_pool_set_wait_limit (fs->private->processing_pool,
		                                        g_value_get_uint (value));
		break;
	case PROP_READY_POOL_LIMIT:
		tracker_processing_pool_set_ready_limit (fs->private->processing_pool,
		                                         g_value_get_uint (value));
		break;
	case PROP_N_REQUESTS_POOL_LIMIT:
		tracker_processing_pool_set_n_requests_limit (fs->private->processing_pool,
		                                              g_value_get_uint (value));
		break;
	case PROP_MTIME_CHECKING:
		fs->private->mtime_checking = g_value_get_boolean (value);
		break;
	case PROP_INITIAL_CRAWLING:
		fs->private->initial_crawling = g_value_get_boolean (value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static void
fs_get_property (GObject    *object,
                 guint       prop_id,
                 GValue     *value,
                 GParamSpec *pspec)
{
	TrackerMinerFS *fs;

	fs = TRACKER_MINER_FS (object);

	switch (prop_id) {
	case PROP_THROTTLE:
		g_value_set_double (value, fs->private->throttle);
		break;
	case PROP_WAIT_POOL_LIMIT:
		g_value_set_uint (value,
		                  tracker_processing_pool_get_wait_limit (fs->private->processing_pool));
		break;
	case PROP_READY_POOL_LIMIT:
		g_value_set_uint (value,
		                  tracker_processing_pool_get_ready_limit (fs->private->processing_pool));
		break;
	case PROP_N_REQUESTS_POOL_LIMIT:
		g_value_set_uint (value,
		                  tracker_processing_pool_get_n_requests_limit (fs->private->processing_pool));
		break;
	case PROP_MTIME_CHECKING:
		g_value_set_boolean (value, fs->private->mtime_checking);
		break;
	case PROP_INITIAL_CRAWLING:
		g_value_set_boolean (value, fs->private->initial_crawling);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}

static gboolean
fs_defaults (TrackerMinerFS *fs,
             GFile          *file)
{
	return TRUE;
}

static gboolean
fs_contents_defaults (TrackerMinerFS *fs,
                      GFile          *parent,
                      GList          *children)
{
	return TRUE;
}

static void
miner_started (TrackerMiner *miner)
{
	TrackerMinerFS *fs;

	fs = TRACKER_MINER_FS (miner);

	fs->private->been_started = TRUE;

	tracker_info ("Initializing");

	g_object_set (miner,
	              "progress", 0.0,
	              "status", "Initializing",
	              NULL);

	crawl_directories_start (fs);
}

static void
miner_stopped (TrackerMiner *miner)
{
	tracker_info ("Idle");

	g_object_set (miner,
	              "progress", 1.0,
	              "status", "Idle",
	              NULL);
}

static void
miner_paused (TrackerMiner *miner)
{
	TrackerMinerFS *fs;

	fs = TRACKER_MINER_FS (miner);

	fs->private->is_paused = TRUE;

	tracker_crawler_pause (fs->private->crawler);

	if (fs->private->item_queues_handler_id) {
		g_source_remove (fs->private->item_queues_handler_id);
		fs->private->item_queues_handler_id = 0;
	}
}

static void
miner_resumed (TrackerMiner *miner)
{
	TrackerMinerFS *fs;

	fs = TRACKER_MINER_FS (miner);

	fs->private->is_paused = FALSE;

	tracker_crawler_resume (fs->private->crawler);

	/* Only set up queue handler if we have items waiting to be
	 * processed.
	 */
	if (g_queue_get_length (fs->private->items_deleted) > 0 ||
	    g_queue_get_length (fs->private->items_created) > 0 ||
	    g_queue_get_length (fs->private->items_updated) > 0 ||
	    g_queue_get_length (fs->private->items_moved) > 0) {
		item_queue_handlers_set_up (fs);
	}
}


static void
miner_ignore_next_update (TrackerMiner *miner, const GStrv urls)
{
	TrackerMinerFS *fs;
	guint n;

	fs = TRACKER_MINER_FS (miner);

	for (n = 0; urls[n] != NULL; n++) {
		g_hash_table_insert (fs->private->items_ignore_next_update,
		                     g_strdup (urls[n]),
		                     GINT_TO_POINTER (TRUE));
	}

	item_queue_handlers_set_up (fs);
}


static DirectoryData *
directory_data_new (GFile    *file,
                    gboolean  recurse)
{
	DirectoryData *dd;

	dd = g_slice_new (DirectoryData);

	dd->file = g_object_ref (file);
	dd->recurse = recurse;
	dd->ref_count = 1;

	return dd;
}

static DirectoryData *
directory_data_ref (DirectoryData *dd)
{
	dd->ref_count++;
	return dd;
}

static void
directory_data_unref (DirectoryData *dd)
{
	if (!dd) {
		return;
	}

	dd->ref_count--;

	if (dd->ref_count == 0) {
		g_object_unref (dd->file);
		g_slice_free (DirectoryData, dd);
	}
}

static void
process_print_stats (TrackerMinerFS *fs)
{
	/* Only do this the first time, otherwise the results are
	 * likely to be inaccurate. Devices can be added or removed so
	 * we can't assume stats are correct.
	 */
	if (!fs->private->shown_totals) {
		fs->private->shown_totals = TRUE;

		tracker_info ("--------------------------------------------------");
		tracker_info ("Total directories : %d (%d ignored)",
		              fs->private->total_directories_found,
		              fs->private->total_directories_ignored);
		tracker_info ("Total files       : %d (%d ignored)",
		              fs->private->total_files_found,
		              fs->private->total_files_ignored);
		tracker_info ("Total monitors    : %d",
		              tracker_monitor_get_count (fs->private->monitor));
		tracker_info ("Total processed   : %d (%d notified, %d with error)",
		              fs->private->total_files_processed,
		              fs->private->total_files_notified,
		              fs->private->total_files_notified_error);
		tracker_info ("--------------------------------------------------\n");
	}
}

static void
process_stop (TrackerMinerFS *fs)
{
	/* Now we have finished crawling, print stats and enable monitor events */
	process_print_stats (fs);

	tracker_info ("Idle");

	g_object_set (fs,
	              "progress", 1.0,
	              "status", "Idle",
	              NULL);

	g_signal_emit (fs, signals[FINISHED], 0,
	               fs->private->timer ? g_timer_elapsed (fs->private->timer, NULL) : 0.0,
	               fs->private->total_directories_found,
	               fs->private->total_directories_ignored,
	               fs->private->total_files_found,
	               fs->private->total_files_ignored);

	if (fs->private->timer) {
		g_timer_destroy (fs->private->timer);
		fs->private->timer = NULL;
	}

	fs->private->total_directories_found = 0;
	fs->private->total_directories_ignored = 0;
	fs->private->total_files_found = 0;
	fs->private->total_files_ignored = 0;

	fs->private->been_crawled = TRUE;
}

static ItemMovedData *
item_moved_data_new (GFile *file,
                     GFile *source_file)
{
	ItemMovedData *data;

	data = g_slice_new (ItemMovedData);
	data->file = g_object_ref (file);
	data->source_file = g_object_ref (source_file);

	return data;
}

static void
item_moved_data_free (ItemMovedData *data)
{
	g_object_unref (data->file);
	g_object_unref (data->source_file);
	g_slice_free (ItemMovedData, data);
}

static void
processing_pool_task_finished_cb (TrackerProcessingTask *task,
                                  gpointer               user_data,
                                  const GError          *error)
{
	TrackerMinerFS *fs;
	TrackerMinerFSPrivate *priv;

	fs = user_data;
	priv = fs->private;

	if (error) {
		g_critical ("Could not execute sparql: %s", error->message);
		priv->total_files_notified_error++;
	} else {
		if (fs->private->current_iri_cache_parent) {
			GFile *parent;
			GFile *task_file;

			task_file = tracker_processing_task_get_file (task);

			/* Note: parent may be NULL if the file represents
			 * the root directory of the file system (applies to
			 * .gvfs mounts also!) */
			parent = g_file_get_parent (task_file);

			if (parent) {
				if (g_file_equal (parent, fs->private->current_iri_cache_parent) &&
				    g_hash_table_lookup (fs->private->iri_cache, task_file) == NULL) {
					/* Item is processed, add an empty element for the processed GFile,
					 * in case it is again processed before the cache expires
					 */
					g_hash_table_insert (fs->private->iri_cache,
					                     g_object_ref (task_file),
					                     NULL);
				}

				g_object_unref (parent);
			}
		}
	}

	item_queue_handlers_set_up (fs);
}

static void
item_query_exists_cb (GObject      *object,
                      GAsyncResult *result,
                      gpointer      user_data)
{
	ItemQueryExistsData *data = user_data;
	TrackerSparqlCursor *cursor;
	GError *error = NULL;
	guint n_results;

	cursor = tracker_sparql_connection_query_finish (TRACKER_SPARQL_CONNECTION (object), result, &error);

	g_main_loop_quit (data->main_loop);

	if (error) {
		g_critical ("Could not execute sparql query: %s", error->message);
		g_error_free (error);
		if (cursor) {
			g_object_unref (cursor);
		}
		return;
	}

	if (!tracker_sparql_cursor_next (cursor, NULL, NULL)) {
#ifdef IRI_CACHE_ENABLE_TRACE
		g_debug ("Querying for resources with uri '%s', NONE found",
		         data->uri);
#endif /* IRI_CACHE_ENABLE_TRACE */

		g_object_unref (cursor);
		return;
	}

	n_results = 1;
	data->iri = g_strdup (tracker_sparql_cursor_get_string (cursor, 0, NULL));
	if (data->get_mime)
		data->mime = g_strdup (tracker_sparql_cursor_get_string (cursor, 1, NULL));

#ifdef IRI_CACHE_ENABLE_TRACE
	g_debug ("Querying for resources with uri '%s', ONE found: '%s'",
	         data->uri,
	         data->iri);
#endif /* IRI_CACHE_ENABLE_TRACE */

	/* Any additional result must be logged as critical */
	while (tracker_sparql_cursor_next (cursor, NULL, NULL)) {
		if (n_results == 1) {
			/* If first duplicate found, log initial critical */
			g_critical ("More than one URNs have been found for uri \"%s\"...",
			            data->uri);
			g_critical ("  (1) urn:'%s', mime:'%s'",
			            data->iri,
			            data->get_mime ? data->mime : "unneeded");
		}
		n_results++;
		g_critical ("  (%d) urn:'%s', mime:'%s'",
		            n_results,
		            tracker_sparql_cursor_get_string (cursor, 0, NULL),
		            data->get_mime ? tracker_sparql_cursor_get_string (cursor, 1, NULL) : "unneeded");
	}

	g_object_unref (cursor);
}

static gboolean
item_query_exists (TrackerMinerFS  *miner,
                   GFile           *file,
                   gchar          **iri,
                   gchar          **mime)
{
	gboolean   result;
	gchar     *sparql, *uri;
	ItemQueryExistsData data = { 0 };

	data.get_mime = (mime != NULL);

	uri = g_file_get_uri (file);

	if (data.get_mime) {
		sparql = g_strdup_printf ("SELECT ?s nie:mimeType(?s) WHERE { ?s nie:url \"%s\" }", uri);
	} else {
		sparql = g_strdup_printf ("SELECT ?s WHERE { ?s nie:url \"%s\" }", uri);
	}

	data.main_loop = g_main_loop_new (NULL, FALSE);
	data.uri = uri;

	tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (miner)),
	                                       sparql,
	                                       NULL,
	                                       item_query_exists_cb,
	                                       &data);

	g_main_loop_run (data.main_loop);
	result = (data.iri != NULL);

	g_main_loop_unref (data.main_loop);

	if (iri) {
		*iri = data.iri;
	} else {
		g_free (data.iri);
	}

	if (mime) {
		*mime = data.mime;
	} else {
		g_free (data.mime);
	}

	g_free (sparql);
	g_free (uri);

	return result;
}

static void
cache_query_cb (GObject	     *object,
                GAsyncResult *result,
                gpointer      user_data)
{
	TrackerSparqlCursor *cursor;
	CacheQueryData *data;
	GError *error = NULL;

	data = user_data;
	cursor = tracker_sparql_connection_query_finish (TRACKER_SPARQL_CONNECTION (object), result, &error);

	g_main_loop_quit (data->main_loop);

	if (G_UNLIKELY (error)) {
		g_critical ("Could not execute cache query: %s", error->message);
		g_error_free (error);
		if (cursor) {
			g_object_unref (cursor);
		}
		return;
	}

	while (tracker_sparql_cursor_next (cursor, NULL, NULL)) {
		GFile *file;

		file = g_file_new_for_uri (tracker_sparql_cursor_get_string (cursor, 0, NULL));

		g_hash_table_insert (data->values,
		                     file,
		                     g_strdup (tracker_sparql_cursor_get_string (cursor, 1, NULL)));
	}

	g_object_unref (cursor);
}

static gboolean
file_is_crawl_directory (TrackerMinerFS *fs,
                         GFile          *file)
{
	GList *dirs;

	/* Check whether file is a crawl directory itself */
	dirs = fs->private->config_directories;

	while (dirs) {
		DirectoryData *data;

		data = dirs->data;
		dirs = dirs->next;

		if (g_file_equal (data->file, file)) {
			return TRUE;
		}
	}

	return FALSE;
}

static DirectoryData *
find_config_directory (TrackerMinerFS *fs,
                       GFile          *file)
{
	GList *dirs;

	dirs = fs->private->config_directories;

	while (dirs) {
		DirectoryData *data;

		data = dirs->data;
		dirs = dirs->next;

		if (g_file_equal (data->file, file) ||
		    (data->recurse && (g_file_has_prefix (file, data->file)))) {
			return data;
		}
	}

	return NULL;
}


static void
ensure_iri_cache (TrackerMinerFS *fs,
                  GFile          *file)
{
	gchar *query, *uri;
	CacheQueryData data;
	GFile *parent;
	guint cache_size;

	g_hash_table_remove_all (fs->private->iri_cache);

	/* Note: parent may be NULL if the file represents
	 * the root directory of the file system (applies to
	 * .gvfs mounts also!) */
	parent = g_file_get_parent (file);

	if (!parent) {
		return;
	}

	uri = g_file_get_uri (parent);

	g_debug ("Generating children cache for URI '%s'", uri);

	query = g_strdup_printf ("SELECT ?url ?u { "
	                         "  ?u nfo:belongsToContainer ?p ; "
	                         "     nie:url ?url . "
	                         "  ?p nie:url \"%s\" "
	                         "}",
	                         uri);

	data.main_loop = g_main_loop_new (NULL, FALSE);
	data.values = g_hash_table_ref (fs->private->iri_cache);

	tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
	                                       query,
	                                       NULL,
	                                       cache_query_cb,
	                                       &data);
	g_free (query);

	g_main_loop_run (data.main_loop);

	g_main_loop_unref (data.main_loop);
	g_hash_table_unref (data.values);

	cache_size = g_hash_table_size (fs->private->iri_cache);

	if (cache_size == 0) {
		if (file_is_crawl_directory (fs, file)) {
			gchar *query_iri;

			if (item_query_exists (fs, file, &query_iri, NULL)) {
				g_hash_table_insert (data.values,
				                     g_object_ref (file), query_iri);
				cache_size++;
			}
		} else if (miner_fs_has_children_without_parent (fs, parent)) {
			/* Quite ugly hack: If mtime_cache is found EMPTY after the query, still, we
			 * may have a nfo:Folder where nfo:belogsToContainer was not yet set (when
			 * generating the dummy nfo:Folder for mount points). In this case, make a
			 * new query not using nfo:belongsToContainer, and using fn:starts-with
			 * instead. Any better solution is highly appreciated */

			/* Initialize data contents */
			data.main_loop = g_main_loop_new (NULL, FALSE);
			data.values = g_hash_table_ref (fs->private->iri_cache);

			g_debug ("Generating children cache for URI '%s' (fn:starts-with)",
			         uri);

			/* Note the last '/' in the url passed in the FILTER: We want to look for
			 * directory contents, not the directory itself */
			query = g_strdup_printf ("SELECT ?url ?u "
			                         "WHERE { ?u a nfo:Folder ; "
			                         "           nie:url ?url . "
			                         "        FILTER (fn:starts-with (?url,\"%s/\"))"
			                         "}",
			                         uri);

			tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
			                                       query,
			                                       NULL,
			                                       cache_query_cb,
			                                       &data);
			g_free (query);

			g_main_loop_run (data.main_loop);
			g_main_loop_unref (data.main_loop);
			g_hash_table_unref (data.values);

			/* Note that in this case, the cache may be actually populated with items
			 * which are not direct children of this parent, but doesn't seem a big
			 * issue right now. In the best case, the dummy item that we created will
			 * be there with a proper mtime set. */
			cache_size = g_hash_table_size (fs->private->iri_cache);
		}
	}

#ifdef IRI_CACHE_ENABLE_TRACE
	g_debug ("Populated IRI cache with '%u' items", cache_size);
	if (cache_size > 0) {
		GHashTableIter iter;
		gpointer key, value;

		g_hash_table_iter_init (&iter, fs->private->iri_cache);
		while (g_hash_table_iter_next (&iter, &key, &value)) {
			gchar *fileuri;

			fileuri = g_file_get_uri (key);
			g_debug ("  In IRI cache: '%s','%s'", fileuri, (gchar *) value);
			g_free (fileuri);
		}
	}
#endif /* IRI_CACHE_ENABLE_TRACE */

	g_object_unref (parent);
	g_free (uri);
}

static const gchar *
iri_cache_lookup (TrackerMinerFS *fs,
                  GFile          *file,
                  gboolean        force_direct_iri_query)
{
	gpointer in_cache_value;
	gboolean in_cache;
	gchar *query_iri;

	/* Look for item in IRI cache */
	in_cache = g_hash_table_lookup_extended (fs->private->iri_cache,
	                                         file,
	                                         NULL,
	                                         &in_cache_value);

	/* Item found with a proper value. If value is NULL, we need
	 * to do a direct IRI query as it was a cache miss (item was added
	 * after the last iri cache update) */
	if (in_cache && in_cache_value)
		return (const gchar *) in_cache_value;

	/* Item doesn't exist in cache. If we don't need to force iri query,
	 * just return. */
	if (!in_cache && !force_direct_iri_query)
		return NULL;

	/* Independent direct IRI query */
	if (item_query_exists (fs, file, &query_iri, NULL)) {
		/* Replace! as we may already have an item in the cache with
		 * NULL value! */
		g_hash_table_replace (fs->private->iri_cache,
		                      g_object_ref (file),
		                      query_iri);
		/* Set iri to return */
		return query_iri;
	}

	/* Not in store, remove item from cache if any */
	if (in_cache)
		g_hash_table_remove (fs->private->iri_cache, file);

	return NULL;
}

static void
iri_cache_invalidate (TrackerMinerFS *fs,
                      GFile          *file)
{
	g_hash_table_remove (fs->private->iri_cache, file);
}

static UpdateProcessingTaskContext *
update_processing_task_context_new (TrackerMiner         *miner,
                                    const gchar          *urn,
                                    const gchar          *parent_urn,
                                    GCancellable         *cancellable,
                                    TrackerSparqlBuilder *builder)
{
	UpdateProcessingTaskContext *ctxt;

	ctxt = g_slice_new0 (UpdateProcessingTaskContext);
	ctxt->miner = miner;
	ctxt->urn = g_strdup (urn);
	ctxt->parent_urn = g_strdup (parent_urn);

	if (cancellable) {
		ctxt->cancellable = g_object_ref (cancellable);
	}

	if (builder) {
		ctxt->builder = g_object_ref (builder);
	}

	return ctxt;
}

static void
update_processing_task_context_free (UpdateProcessingTaskContext *ctxt)
{
	g_free (ctxt->urn);
	g_free (ctxt->parent_urn);

	if (ctxt->cancellable) {
		g_object_unref (ctxt->cancellable);
	}

	if (ctxt->builder) {
		g_object_unref (ctxt->builder);
	}

	g_slice_free (UpdateProcessingTaskContext, ctxt);
}

static gboolean
do_process_file (TrackerMinerFS        *fs,
                 TrackerProcessingTask *task)
{
	TrackerMinerFSPrivate *priv;
	gboolean processing;
	gboolean attribute_update_only;
	gchar *uri;
	GFile *task_file;
	UpdateProcessingTaskContext *ctxt;

	ctxt = tracker_processing_task_get_context (task);
	task_file = tracker_processing_task_get_file (task);
	uri = g_file_get_uri (task_file);
	priv = fs->private;

	attribute_update_only = GPOINTER_TO_INT (g_object_get_qdata (G_OBJECT (task_file),
	                                                             priv->quark_attribute_updated));

	if (!attribute_update_only) {
		g_debug ("Processing file '%s'...", uri);
		g_signal_emit (fs, signals[PROCESS_FILE], 0,
		               task_file,
		               ctxt->builder,
		               ctxt->cancellable,
		               &processing);
	} else {
		g_debug ("Processing attributes in file '%s'...", uri);
		g_signal_emit (fs, signals[PROCESS_FILE_ATTRIBUTES], 0,
		               task_file,
		               ctxt->builder,
		               ctxt->cancellable,
		               &processing);
	}

	if (!processing) {
		/* Re-fetch data, since it might have been
		 * removed in broken implementations
		 */
		task = tracker_processing_pool_find_task (priv->processing_pool, task_file, FALSE);

		g_message ("%s refused to process '%s'", G_OBJECT_TYPE_NAME (fs), uri);

		if (!task) {
			g_critical ("%s has returned FALSE in ::process-file for '%s', "
			            "but it seems that this file has been processed through "
			            "tracker_miner_fs_file_notify(), this is an "
			            "implementation error", G_OBJECT_TYPE_NAME (fs), uri);
		} else {
			tracker_processing_pool_remove_task (priv->processing_pool, task);
			tracker_processing_task_free (task);
		}
	}

	g_free (uri);

	return processing;
}

static void
item_add_or_update_cb (TrackerMinerFS        *fs,
                       TrackerProcessingTask *task,
                       const GError          *error)
{
	UpdateProcessingTaskContext *ctxt;
	GFile *task_file;
	gchar *uri;

	ctxt = tracker_processing_task_get_context (task);
	task_file = tracker_processing_task_get_file (task);
	uri = g_file_get_uri (task_file);

	if (error) {
		TrackerProcessingTask *first_item_task;

		first_item_task = tracker_processing_pool_get_last_wait (fs->private->processing_pool);

		/* Perhaps this is too specific to TrackerMinerFiles, if the extractor
		 * is choking on some file, the miner will get a timeout for all files
		 * being currently processed, but the one that is actually causing it
		 * is the first one that was added to the processing pool, so we retry
		 * the others.
		 */
		if (task != first_item_task &&
		    (error->code == G_DBUS_ERROR_NO_REPLY ||
		     error->code == G_DBUS_ERROR_TIMEOUT ||
		     error->code == G_DBUS_ERROR_TIMED_OUT)) {
			g_debug ("  Got DBus timeout error on '%s', but it could not be caused by it. Retrying file.", uri);

			/* Reset the TrackerSparqlBuilder */
			g_object_unref (ctxt->builder);
			ctxt->builder = tracker_sparql_builder_new_update ();

			do_process_file (fs, task);
			g_free (uri);

			return;

		} else {
			fs->private->total_files_notified_error++;
			g_message ("Could not process '%s': %s",
			            uri,
			            error->message ? error->message : "No error given");
		}
	}

	if (ctxt->urn) {
		gboolean attribute_update_only;

		attribute_update_only = GPOINTER_TO_INT (g_object_steal_qdata (G_OBJECT (task_file),
		                                                               fs->private->quark_attribute_updated));
		g_debug ("Updating item%s%s%s '%s' with urn '%s'%s",
		         error != NULL ? " (which had extractor error '" : "",
		         error != NULL ? (error->message ? error->message : "No error given") : "",
		         error != NULL ? "')" : "",
		         uri,
		         ctxt->urn,
		         attribute_update_only ? " (attributes only)" : "");

		if (!attribute_update_only) {
			gchar *full_sparql;

			/* update, delete all statements inserted by miner
			 * except for rdf:type statements as they could cause implicit deletion of user data */
			full_sparql = g_strdup_printf ("DELETE {"
			                               "  GRAPH <%s> {"
			                               "    <%s> ?p ?o"
			                               "  } "
			                               "} "
			                               "WHERE {"
			                               "  GRAPH <%s> {"
			                               "    <%s> ?p ?o"
			                               "    FILTER (?p != rdf:type && ?p != nie:contentCreated)"
			                               "  } "
			                               "} "
			                               "%s",
			                               TRACKER_MINER_FS_GRAPH_URN,
			                               ctxt->urn,
			                               TRACKER_MINER_FS_GRAPH_URN,
			                               ctxt->urn,
			                               tracker_sparql_builder_get_result (ctxt->builder));

			/* Note that set_sparql_string() takes ownership of the passed string */
			tracker_processing_task_set_sparql_string (task, full_sparql);
		} else {
			/* Do not drop graph if only updating attributes, the SPARQL builder
			 * will already contain the necessary DELETE statements for the properties
			 * being updated */
			tracker_processing_task_set_sparql (task, ctxt->builder);
		}
	} else {
		if (error != NULL) {
			g_debug ("Creating minimal info for new item '%s' which had error: '%s'",
			         uri,
			         error->message ? error->message : "No error given");
		} else {
			g_debug ("Creating new item '%s'", uri);
		}

		tracker_processing_task_set_sparql (task, ctxt->builder);
	}

	/* If push_ready_task() returns FALSE, it means the actual db update was delayed,
	 * and in this case we need to setup queue handlers again */
	if (!tracker_processing_pool_push_ready_task (fs->private->processing_pool,
	                                              task,
	                                              processing_pool_task_finished_cb,
	                                              fs)) {
		item_queue_handlers_set_up (fs);
	}

	g_free (uri);
}

static gboolean
item_add_or_update (TrackerMinerFS *fs,
                    GFile          *file)
{
	TrackerMinerFSPrivate *priv;
	TrackerSparqlBuilder *sparql;
	GCancellable *cancellable;
	gboolean retval;
	TrackerProcessingTask *task;
	GFile *parent;
	const gchar *urn;
	const gchar *parent_urn = NULL;
	UpdateProcessingTaskContext *ctxt;

	priv = fs->private;
	retval = TRUE;

	cancellable = g_cancellable_new ();
	sparql = tracker_sparql_builder_new_update ();
	g_object_ref (file);

	parent = g_file_get_parent (file);

	if (parent) {
		if (!fs->private->current_iri_cache_parent ||
		    !g_file_equal (parent, fs->private->current_iri_cache_parent)) {
			/* Cache the URN for the new current parent, processing
			 * order guarantees that all contents for a folder are
			 * inspected together, and that the parent folder info
			 * is already in tracker-store. So this should only
			 * happen on folder switch.
			 */
			if (fs->private->current_iri_cache_parent)
				g_object_unref (fs->private->current_iri_cache_parent);

			g_free (fs->private->current_iri_cache_parent_urn);

			fs->private->current_iri_cache_parent = g_object_ref (parent);

			if (!item_query_exists (fs,
			                        parent,
			                        &fs->private->current_iri_cache_parent_urn,
			                        NULL)) {
				fs->private->current_iri_cache_parent_urn = NULL;
			}

			ensure_iri_cache (fs, file);
		}

		parent_urn = fs->private->current_iri_cache_parent_urn;
		g_object_unref (parent);
	}

	/* Force a direct URN query if not found in the cache. This is to handle
	 * situations where an application inserted items in the store after we
	 * updated the cache, or without a proper nfo:belongsToContainer */
	urn = iri_cache_lookup (fs, file, TRUE);

	/* Create task and add it to the pool as a WAIT task (we need to extract
	 * the file metadata and such) */
	task = tracker_processing_task_new (file);
	ctxt = update_processing_task_context_new (TRACKER_MINER (fs),
	                                           urn,
	                                           parent_urn,
	                                           cancellable,
	                                           sparql);
	tracker_processing_task_set_context (task,
	                                     ctxt,
	                                     (GFreeFunc) update_processing_task_context_free);
	tracker_processing_pool_push_wait_task (priv->processing_pool, task);

	if (do_process_file (fs, task)) {
		fs->private->total_files_processed++;

		if (tracker_processing_pool_wait_limit_reached (priv->processing_pool)) {
			retval = FALSE;
		}
	}

	g_object_unref (file);
	g_object_unref (cancellable);
	g_object_unref (sparql);

	return retval;
}

static gboolean
item_remove (TrackerMinerFS *fs,
             GFile          *file)
{
	gchar *uri;
	gchar *mime = NULL;
	TrackerProcessingTask *task;

	iri_cache_invalidate (fs, file);
	uri = g_file_get_uri (file);

	g_debug ("Removing item: '%s' (Deleted from filesystem or no longer monitored)",
	         uri);

	if (!item_query_exists (fs, file, NULL, &mime)) {
		g_debug ("  File does not exist anyway (uri '%s')", uri);
		g_free (uri);
		g_free (mime);
		return TRUE;
	}

	tracker_thumbnailer_remove_add (uri, mime);

	g_free (mime);

	/* FIRST:
	 * Remove tracker:available for the resources we're going to remove.
	 * This is done so that unavailability of the resources is marked as soon
	 * as possible, as the actual delete may take reaaaally a long time
	 * (removing resources for 30GB of files takes even 30minutes in a 1-CPU
	 * device). */

	/* Add new task to processing pool */
	task = tracker_processing_task_new (file);
	tracker_processing_task_set_bulk_operation (task,
	                                            "DELETE { "
	                                            "  ?f tracker:available true "
	                                            "}",
	                                            TRACKER_BULK_MATCH_EQUALS |
	                                            TRACKER_BULK_MATCH_CHILDREN);

	/* If push_ready_task() returns FALSE, it means the actual db update was delayed,
	 * and in this case we need to setup queue handlers again */
	if (!tracker_processing_pool_push_ready_task (fs->private->processing_pool,
	                                              task,
	                                              processing_pool_task_finished_cb,
	                                              fs)) {
		item_queue_handlers_set_up (fs);
	}

	/* SECOND:
	 * Actually remove all resources. This operation is the one which may take
	 * a long time.
	 */

	/* Add new task to processing pool */
	task = tracker_processing_task_new (file);
	tracker_processing_task_set_bulk_operation (task,
	                                            "DELETE { "
	                                            "  ?f a rdfs:Resource "
	                                            "}",
	                                            TRACKER_BULK_MATCH_EQUALS |
	                                            TRACKER_BULK_MATCH_CHILDREN);

	/* If push_ready_task() returns FALSE, it means the actual db update was delayed,
	 * and in this case we need to setup queue handlers again */
	if (!tracker_processing_pool_push_ready_task (fs->private->processing_pool,
	                                              task,
	                                              processing_pool_task_finished_cb,
	                                              fs)) {
		item_queue_handlers_set_up (fs);
	}

	g_free (uri);

	return TRUE;
}

static gboolean
item_ignore_next_update (TrackerMinerFS *fs,
                         GFile          *file,
                         GFile          *source_file)
{
	TrackerSparqlBuilder *sparql;
	gchar *uri;
	gboolean success = FALSE;
	GCancellable *cancellable;
	GFile *working_file;

	/* While we are in ignore-on-next-update:
	 * o. We always ignore deletes because it's never the final operation
	 *    of a write. We have a delete when both are null.
	 * o. A create means the write used rename(). This is the final
	 *    operation of a write and thus we make the update query.
	 *    We have a create when file == null and source_file != null
	 * o. A move means the write used rename(). This is the final
	 *    operation of a write and thus we make the update query.
	 *    We have a move when both file and source_file aren't null.
	 * o. A update means the write didn't use rename(). This is the
	 *    final operation of a write and thus we make the update query.
	 *    An update means that file != null and source_file == null. */

	/* Happens on delete while in write */
	if (!file && !source_file) {
		return TRUE;
	}

	/* Create or update, we are the final one so we make the update query */

	if (!file && source_file) {
		/* Happens on create while in write */
		working_file = source_file;
	} else {
		/* Happens on update while in write */
		working_file = file;
	}

	uri = g_file_get_uri (working_file);

	g_debug ("Updating item: '%s' (IgnoreNextUpdate event)", uri);

	cancellable = g_cancellable_new ();
	sparql = tracker_sparql_builder_new_update ();
	g_object_ref (working_file);

	/* IgnoreNextUpdate */
	g_signal_emit (fs, signals[IGNORE_NEXT_UPDATE_FILE], 0,
	               working_file, sparql, cancellable, &success);

	if (success) {
		gchar *query;

		/* Perhaps we should move the DELETE to tracker-miner-files.c?
		 * Or we add support for DELETE to TrackerSparqlBuilder ofcrs */

		query = g_strdup_printf ("DELETE { GRAPH <%s> { "
		                         "  ?u nfo:fileSize ?unknown1 ; "
		                         "     nfo:fileLastModified ?unknown2 ; "
		                         "     nfo:fileLastAccessed ?unknown3 ; "
		                         "     nie:mimeType ?unknown4 } "
		                         "} WHERE { GRAPH <%s> { "
		                         "  ?u nfo:fileSize ?unknown1 ; "
		                         "     nfo:fileLastModified ?unknown2 ; "
		                         "     nfo:fileLastAccessed ?unknown3 ; "
		                         "     nie:mimeType ?unknown4 ; "
		                         "     nie:url \"%s\" } "
		                         "} %s", TRACKER_MINER_FS_GRAPH_URN,
		                         TRACKER_MINER_FS_GRAPH_URN, uri,
		                         tracker_sparql_builder_get_result (sparql));

		tracker_sparql_connection_update_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
		                                        query,
		                                        G_PRIORITY_DEFAULT,
		                                        NULL,
		                                        NULL,
		                                        NULL);

		g_free (query);
	}

	g_hash_table_remove (fs->private->items_ignore_next_update, uri);

	g_object_unref (sparql);
	g_object_unref (working_file);
	g_object_unref (cancellable);

	g_free (uri);

	return FALSE;
}

static void
item_update_children_uri_cb (GObject      *object,
                             GAsyncResult *result,
                             gpointer      user_data)
{
	RecursiveMoveData *data = user_data;
	GError *error = NULL;

	TrackerSparqlCursor *cursor = tracker_sparql_connection_query_finish (TRACKER_SPARQL_CONNECTION (object), result, &error);

	if (error) {
		g_critical ("Could not query children: %s", error->message);
		g_error_free (error);
		if (cursor) {
			g_object_unref (cursor);
		}
	} else {
		while (tracker_sparql_cursor_next (cursor, NULL, NULL)) {
			const gchar *child_source_uri, *child_mime, *child_urn;
			gchar *child_uri;

			child_urn = tracker_sparql_cursor_get_string (cursor, 0, NULL);
			child_source_uri = tracker_sparql_cursor_get_string (cursor, 1, NULL);
			child_mime = tracker_sparql_cursor_get_string (cursor, 2, NULL);

			if (!g_str_has_prefix (child_source_uri, data->source_uri)) {
				g_warning ("Child URI '%s' does not start with parent URI '%s'",
				           child_source_uri,
				           data->source_uri);
				continue;
			}

			child_uri = g_strdup_printf ("%s%s", data->uri, child_source_uri + strlen (data->source_uri));

			g_string_append_printf (data->sparql,
			                        "DELETE { "
			                        "  <%s> nie:url ?u "
			                        "} WHERE { "
			                        "  <%s> nie:url ?u "
			                        "} ",
			                        child_urn, child_urn);

			g_string_append_printf (data->sparql,
			                        "INSERT INTO <%s> {"
			                        "  <%s> nie:url \"%s\" "
			                        "} ",
			                        child_urn, child_urn, child_uri);

			tracker_thumbnailer_move_add (child_source_uri, child_mime, child_uri);

			g_free (child_uri);
		}
	}

	g_object_unref (cursor);

	g_main_loop_quit (data->main_loop);
}

static void
item_update_children_uri (TrackerMinerFS    *fs,
                          RecursiveMoveData *move_data,
                          const gchar       *source_uri,
                          const gchar       *uri)
{
	gchar *sparql;

	sparql = g_strdup_printf ("SELECT ?child ?url nie:mimeType(?child) WHERE { "
	                          "  ?child nie:url ?url . "
	                          "  FILTER (tracker:uri-is-descendant (\"%s\", ?url)) "
	                          "}",
	                          source_uri);

	tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
	                                       sparql,
	                                       NULL,
	                                       item_update_children_uri_cb,
	                                       move_data);

	g_free (sparql);
}

static gboolean
item_move (TrackerMinerFS *fs,
           GFile          *file,
           GFile          *source_file)
{
	gchar     *uri, *source_uri;
	GFileInfo *file_info;
	GString   *sparql;
	RecursiveMoveData move_data;
	TrackerProcessingTask *task;
	gchar *source_iri;
	gchar *display_name;
	gboolean source_exists;
	GFile *new_parent;
	gchar *new_parent_iri;

	iri_cache_invalidate (fs, file);
	iri_cache_invalidate (fs, source_file);

	uri = g_file_get_uri (file);
	source_uri = g_file_get_uri (source_file);

	file_info = g_file_query_info (file,
	                               G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME ","
	                               G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE ","
	                               G_FILE_ATTRIBUTE_STANDARD_TYPE,
	                               G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
	                               NULL, NULL);

	/* Get 'source' ID */
	source_exists = item_query_exists (fs, source_file, &source_iri, NULL);

	if (!file_info) {
		gboolean retval;

		if (source_exists) {
			/* Destination file has gone away, ignore dest file and remove source if any */
			retval = item_remove (fs, source_file);
		} else {
			/* Destination file went away, and source wasn't indexed either */
			retval = TRUE;
		}

		g_free (source_iri);
		g_free (source_uri);
		g_free (uri);

		return retval;
	} else if (file_info && !source_exists) {
		gboolean retval;
		GFileType file_type;

		g_message ("Source file '%s' not found in store to move, indexing '%s' from scratch", source_uri, uri);

		file_type = g_file_info_get_file_type (file_info);

		if (file_type == G_FILE_TYPE_DIRECTORY &&
		    should_recurse_for_directory (fs, file)) {
			/* We're dealing with a recursive directory */
			tracker_miner_fs_directory_add_internal (fs, file);
			retval = TRUE;
		} else {
			retval = item_add_or_update (fs, file);
		}

		g_free (source_uri);
		g_free (uri);
		g_object_unref (file_info);

		return retval;
	}

	g_debug ("Moving item from '%s' to '%s'",
	         source_uri,
	         uri);

	tracker_thumbnailer_move_add (source_uri,
	                              g_file_info_get_content_type (file_info),
	                              uri);

	sparql = g_string_new ("");

	/* Delete destination item from store if any */
	g_string_append_printf (sparql,
	                        "DELETE { "
	                        "  ?urn a rdfs:Resource "
	                        "} WHERE {"
	                        "  ?urn nie:url \"%s\" "
	                        "}",
	                        uri);

	g_string_append_printf (sparql,
	                        "DELETE { "
	                        "  <%s> nfo:fileName ?f ; "
	                        "       nie:url ?u ; "
	                        "       nie:isStoredAs ?s ; "
	                        "       nfo:belongsToContainer ?b"
	                        "} WHERE { "
	                        "  <%s> nfo:fileName ?f ; "
	                        "       nie:url ?u ; "
	                        "       nie:isStoredAs ?s ; "
	                        "       nfo:belongsToContainer ?b"
	                        "} ",
	                        source_iri, source_iri);

	display_name = tracker_sparql_escape_string (g_file_info_get_display_name (file_info));

	/* Get new parent information */
	new_parent = g_file_get_parent (file);

	if (new_parent &&
	    item_query_exists (fs, new_parent, &new_parent_iri, NULL)) {
		g_string_append_printf (sparql,
		                        "INSERT INTO <%s> {"
		                        "  <%s> nfo:fileName \"%s\" ; "
		                        "       nie:url \"%s\" ; "
		                        "       nie:isStoredAs <%s> ; "
		                        "       nfo:belongsToContainer \"%s\""
		                        "}"   ,
		                        source_iri, source_iri,
		                        display_name, uri,
		                        source_iri,
		                        new_parent_iri);
		g_free (new_parent_iri);
	} else {
		g_warning ("Adding moved item '%s' without nfo:belongsToContainer (new_parent: %p)",
		           uri, new_parent);
		g_string_append_printf (sparql,
		                        "INSERT INTO <%s> {"
		                        "  <%s> nfo:fileName \"%s\" ; "
		                        "       nie:url \"%s\" ; "
		                        "       nie:isStoredAs <%s>"
		                        "} ",
		                        source_iri, source_iri,
		                        display_name, uri,
		                        source_iri);
	}

	if (new_parent)
		g_object_unref (new_parent);
	g_free (display_name);

	move_data.main_loop = g_main_loop_new (NULL, FALSE);
	move_data.sparql = sparql;
	move_data.source_uri = source_uri;
	move_data.uri = uri;

	item_update_children_uri (fs, &move_data, source_uri, uri);

	g_main_loop_run (move_data.main_loop);

	g_main_loop_unref (move_data.main_loop);

	/* Add new task to processing pool */
	task = tracker_processing_task_new (file);
	/* Note that set_sparql_string() takes ownership of the passed string */
	tracker_processing_task_set_sparql_string (task,
	                                           g_string_free (sparql, FALSE));
	/* If push_ready_task() returns FALSE, it means the actual db update was delayed,
	 * and in this case we need to setup queue handlers again */
	if (!tracker_processing_pool_push_ready_task (fs->private->processing_pool,
	                                              task,
	                                              processing_pool_task_finished_cb,
	                                              fs)) {
		item_queue_handlers_set_up (fs);
	}

	g_free (uri);
	g_free (source_uri);
	g_object_unref (file_info);
	g_free (source_iri);

	return TRUE;
}

static gboolean
check_ignore_next_update (TrackerMinerFS *fs, GFile *queue_file)
{
	gchar *uri = g_file_get_uri (queue_file);
	if (g_hash_table_lookup (fs->private->items_ignore_next_update, uri)) {
		g_free (uri);
		return TRUE;
	}
	g_free (uri);
	return FALSE;
}

static void
fill_in_items_created_queue (TrackerMinerFS *fs)
{
	CrawledDirectoryData *dir_data;
	GList *l, *post_nodes = NULL;
	GFile *file;
	GNode *node;

	dir_data = g_queue_peek_head (fs->private->crawled_directories);

	if (g_queue_is_empty (dir_data->nodes)) {
		/* Special case, append the root directory for the tree */
		node = dir_data->tree;
		file = node->data;
		dir_data->n_items_processed++;

		g_queue_push_tail (dir_data->nodes, node);

		if (!g_object_get_qdata (G_OBJECT (file), fs->private->quark_ignore_file)) {
			trace_eq_push_tail ("CREATED", file, "Root directory of tree");
			g_queue_push_tail (fs->private->items_created, g_object_ref (file));
			return;
		}
	}

	node = g_queue_pop_head (dir_data->nodes);

	/* There are nodes in the middle of processing. Append
	 * items to the queue, an add directories to post_nodes,
	 * so they can be processed later on.
	 */
	while (node) {
		GNode *children;

#ifdef EVENT_QUEUE_ENABLE_TRACE
		gchar *uri;

		uri = g_file_get_uri (node->data);
		g_message ("Adding files from directory '%s' into the processing queue", uri);
		g_free (uri);
#endif /* EVENT_QUEUE_ENABLE_TRACE */

		children = node->children;

		while (children) {
			file = children->data;
			dir_data->n_items_processed++;

			if (!g_object_get_qdata (G_OBJECT (file), fs->private->quark_ignore_file)) {
				trace_eq_push_tail ("CREATED", file, NULL);
				g_queue_push_tail (fs->private->items_created, g_object_ref (file));
			}

			if (children->children) {
				post_nodes = g_list_prepend (post_nodes, children);
			}

			children = children->next;
		}

		node = g_queue_pop_head (dir_data->nodes);
	}

	/* Children collected in post_nodes will be
	 * the ones processed on the next iteration
	 */
	for (l = post_nodes; l; l = l->next) {
		g_queue_push_tail (dir_data->nodes, l->data);
	}

	g_list_free (post_nodes);

	if (g_queue_is_empty (dir_data->nodes)) {
		/* There's no more data to process, move on to the next one */
		g_queue_pop_head (fs->private->crawled_directories);
		crawled_directory_data_free (dir_data);
	}
}


static gboolean
should_wait (TrackerMinerFS *fs,
             GFile          *file)
{
	GFile *parent;

	/* Is the item already being processed? */
	if (tracker_processing_pool_find_task (fs->private->processing_pool,
	                                       file,
	                                       TRUE)) {
		/* Yes, a previous event on same item currently
		 * being processed */
		return TRUE;
	}

	/* Is the item's parent being processed right now? */
	parent = g_file_get_parent (file);
	if (parent) {
		if (tracker_processing_pool_find_task (fs->private->processing_pool,
		                                       parent,
		                                       TRUE)) {
			/* Yes, a previous event on the parent of this item
			 * currently being processed */
			g_object_unref (parent);
			return TRUE;
		}

		g_object_unref (parent);
	}
	return FALSE;
}

static QueueState
item_queue_get_next_file (TrackerMinerFS  *fs,
                          GFile          **file,
                          GFile          **source_file)
{
	ItemMovedData *data;
	GFile *queue_file;

	/* Deleted items first */
	queue_file = g_queue_pop_head (fs->private->items_deleted);
	if (queue_file) {
		*source_file = NULL;

		trace_eq_pop_head ("DELETED", queue_file);

		/* Do not ignore DELETED event even if file is marked as
		   IgnoreNextUpdate. We should never see DELETED on update
		   (atomic rename or in-place update) but we may see DELETED
		   due to actual file deletion right after update. */

		/* If the same item OR its first parent is currently being processed,
		 * we need to wait for this event */
		if (should_wait (fs, queue_file)) {
			*file = NULL;

			trace_eq_push_head ("DELETED", queue_file, "Should wait");

			/* Need to postpone event... */
			g_queue_push_head (fs->private->items_deleted,
			                   queue_file);
			return QUEUE_WAIT;
		}

		*file = queue_file;
		return QUEUE_DELETED;
	}

	if (g_queue_is_empty (fs->private->items_created) &&
	    !g_queue_is_empty (fs->private->crawled_directories)) {

		trace_eq ("Created items queue empty, but still crawling (%d tasks in WAIT state)",
		          tracker_processing_pool_get_wait_task_count (fs->private->processing_pool));

		/* The items_created queue is empty, but there are pending
		 * items from the crawler to be processed. We feed the queue
		 * in this manner so it's ensured that the parent directory
		 * info is inserted to the store before the children are
		 * inspected.
		 */
		if (tracker_processing_pool_get_wait_task_count (fs->private->processing_pool) > 0) {
			/* Items still being processed */
			*file = NULL;
			*source_file = NULL;
			return QUEUE_WAIT;
		} else {
			/* Iterate through all directory hierarchies until
			 * one of these return something for the miner to do,
			 * or no data is left to process.
			 */
			while (g_queue_is_empty (fs->private->items_created) &&
			       !g_queue_is_empty (fs->private->crawled_directories)) {
				fill_in_items_created_queue (fs);
			}
		}
	}

	/* Created items next */
	queue_file = g_queue_pop_head (fs->private->items_created);
	if (queue_file) {
		*source_file = NULL;

		trace_eq_pop_head ("CREATED", queue_file);

		/* Note:
		 * We won't be considering an IgnoreNextUpdate request if
		 * the event being processed is a CREATED event. This is a fast
		 * and dirty solution for the situation where tracker-writeback
		 * sends an IgnoreNextUpdate event and miner-fs had never indexed
		 * the file before.
		 */
		if (check_ignore_next_update (fs, queue_file)) {
			gchar *uri;

			/* Just remove the IgnoreNextUpdate request */
			uri = g_file_get_uri (queue_file);
			g_debug ("Skipping the IgnoreNextUpdate request on CREATED event for '%s'",
			         uri);
			g_hash_table_remove (fs->private->items_ignore_next_update, uri);
			g_free (uri);
		}

		/* If the same item OR its first parent is currently being processed,
		 * we need to wait for this event */
		if (should_wait (fs, queue_file)) {
			*file = NULL;

			trace_eq_push_head ("CREATED", queue_file, "Should wait");

			/* Need to postpone event... */
			g_queue_push_head (fs->private->items_created,
			                   queue_file);
			return QUEUE_WAIT;
		}

		*file = queue_file;
		return QUEUE_CREATED;
	}

	/* Updated items next */
	queue_file = g_queue_pop_head (fs->private->items_updated);
	if (queue_file) {
		*file = queue_file;
		*source_file = NULL;

		trace_eq_pop_head ("UPDATED", queue_file);

		if (check_ignore_next_update (fs, queue_file)) {
			gchar *uri;

			uri = g_file_get_uri (queue_file);
			g_debug ("UPDATED event ignored on file '%s', "
			         " processing as IgnoreNextUpdate...",
			         uri);
			g_free (uri);

			return QUEUE_IGNORE_NEXT_UPDATE;
		}

		/* If the same item OR its first parent is currently being processed,
		 * we need to wait for this event */
		if (should_wait (fs, queue_file)) {
			*file = NULL;

			trace_eq_push_head ("UPDATED", queue_file, "Should wait");

			/* Need to postpone event... */
			g_queue_push_head (fs->private->items_updated,
			                   queue_file);
			return QUEUE_WAIT;
		}

		return QUEUE_UPDATED;
	}

	/* Moved items next */
	data = g_queue_pop_head (fs->private->items_moved);
	if (data) {
		trace_eq_pop_head_2 ("MOVED", data->file, data->source_file);

		if (check_ignore_next_update (fs, data->file)) {
			gchar *uri;
			gchar *source_uri;

			uri = g_file_get_uri (queue_file);
			source_uri = g_file_get_uri (data->source_file);
			g_debug ("MOVED event ignored on files '%s->%s', "
			         " processing as IgnoreNextUpdate on '%s'",
			         source_uri, uri, uri);
			g_free (uri);
			g_free (source_uri);

			*file = g_object_ref (data->file);
			*source_file = g_object_ref (data->source_file);
			item_moved_data_free (data);
			return QUEUE_IGNORE_NEXT_UPDATE;
		}

		/* If the same item OR its first parent is currently being processed,
		 * we need to wait for this event */
		if (should_wait (fs, data->file) ||
		    should_wait (fs, data->source_file)) {
			*file = NULL;
			*source_file = NULL;

			trace_eq_push_head_2 ("MOVED", data->source_file, data->file, "Should wait");

			/* Need to postpone event... */
			g_queue_push_head (fs->private->items_moved,
			                   data); /* no need to create again */
			return QUEUE_WAIT;
		}

		*file = g_object_ref (data->file);
		*source_file = g_object_ref (data->source_file);
		item_moved_data_free (data);
		return QUEUE_MOVED;
	}

	*file = NULL;
	*source_file = NULL;

	return QUEUE_NONE;
}

static void
get_tree_progress_foreach (CrawledDirectoryData *data,
                           gint                 *items_to_process)
{
	*items_to_process += data->n_items - data->n_items_processed;
}

static gdouble
item_queue_get_progress (TrackerMinerFS *fs,
                         guint          *n_items_processed,
                         guint          *n_items_remaining)
{
	guint items_to_process = 0;
	guint items_total = 0;

	items_to_process += g_queue_get_length (fs->private->items_deleted);
	items_to_process += g_queue_get_length (fs->private->items_created);
	items_to_process += g_queue_get_length (fs->private->items_updated);
	items_to_process += g_queue_get_length (fs->private->items_moved);

	g_queue_foreach (fs->private->crawled_directories,
	                 (GFunc) get_tree_progress_foreach,
	                 &items_to_process);

	items_total += fs->private->total_directories_found;
	items_total += fs->private->total_files_found;

	if (n_items_processed) {
		*n_items_processed = ((items_total >= items_to_process) ?
		                      (items_total - items_to_process) : 0);
	}

	if (n_items_remaining) {
		*n_items_remaining = items_to_process;
	}

	if (items_to_process == 0 && items_total > 0) {
		return 0.0;
	}

	if (items_total == 0 || items_to_process > items_total) {
		return 1.0;
	}

	return (gdouble) (items_total - items_to_process) / items_total;
}

static gboolean
item_queue_handlers_cb (gpointer user_data)
{
	TrackerMinerFS *fs;
	GFile *file = NULL;
	GFile *source_file = NULL;
	QueueState queue;
	GTimeVal time_now;
	static GTimeVal time_last = { 0 };
	gboolean keep_processing = TRUE;

	fs = user_data;
	queue = item_queue_get_next_file (fs, &file, &source_file);

	if (queue == QUEUE_WAIT) {
		/* Items are still being processed, and there is pending
		 * data in priv->crawled_directories, so wait until
		 * the processing pool is cleared before starting with
		 * the next directories batch.
		 */
		fs->private->item_queues_handler_id = 0;

		/* We should flush the processing pool buffer here, because
		 * if there was a previous task on the same file we want to
		 * process now, we want it to get finished before we can go
		 * on with the queues... */
		tracker_processing_pool_buffer_flush (fs->private->processing_pool,
		                                      "Queue handlers WAIT");

		return FALSE;
	}

	if (file && queue != QUEUE_DELETED &&
	    tracker_file_is_locked (file)) {
		gchar *uri;

		/* File is locked, ignore any updates on it */

		uri = g_file_get_uri (file);
		g_debug ("File '%s' is currently locked, ignoring updates on it",
		         uri);
		g_free (uri);

		g_object_unref (file);

		if (source_file) {
			g_object_unref (source_file);
		}

		return TRUE;
	}

	if (!fs->private->timer) {
		fs->private->timer = g_timer_new ();
	}

	/* Update progress, but don't spam it. */
	g_get_current_time (&time_now);

	if ((time_now.tv_sec - time_last.tv_sec) >= 1) {
		guint items_processed, items_remaining;
		gdouble progress_now;
		static gdouble progress_last = 0.0;
		static gint info_last = 0;

		time_last = time_now;

		/* Update progress? */
		progress_now = item_queue_get_progress (fs,
		                                        &items_processed,
		                                        &items_remaining);

		if (!fs->private->is_crawling) {
			gchar *status;

			g_object_get (fs, "status", &status, NULL);

			if (g_strcmp0 (status, "Processing…") != 0) {
				/* Don't spam this */
				tracker_info ("Processing…");
				g_object_set (fs,
				              "status", "Processing…",
				              "progress", progress_now,
				              NULL);
			} else {
				g_object_set (fs,
				              "progress", progress_now,
				              NULL);
			}

			g_free (status);
		}

		if (++info_last >= 5 &&
		    (gint) (progress_last * 100) != (gint) (progress_now * 100)) {
			gchar *str1, *str2;
			gdouble seconds_elapsed;

			info_last = 0;
			progress_last = progress_now;

			/* Log estimated remaining time */
			seconds_elapsed = g_timer_elapsed (fs->private->timer, NULL);
			str1 = tracker_seconds_estimate_to_string (seconds_elapsed,
			                                           TRUE,
			                                           items_processed,
			                                           items_remaining);
			str2 = tracker_seconds_to_string (seconds_elapsed, TRUE);

			tracker_info ("Processed %u/%u, estimated %s left, %s elapsed",
			              items_processed,
			              items_processed + items_remaining,
			              str1,
			              str2);

			g_free (str2);
			g_free (str1);
		}
	}

	/* Handle queues */
	switch (queue) {
	case QUEUE_NONE:
		/* Print stats and signal finished */
		if (!fs->private->is_crawling &&
		    tracker_processing_pool_get_total_task_count (fs->private->processing_pool) == 0) {
			process_stop (fs);
		}

		/* Flush any possible pending update here */
		tracker_processing_pool_buffer_flush (fs->private->processing_pool,
		                                      "Queue handlers NONE");

		tracker_thumbnailer_send ();
		/* No more files left to process */
		keep_processing = FALSE;
		break;
	case QUEUE_MOVED:
		keep_processing = item_move (fs, file, source_file);
		break;
	case QUEUE_DELETED:
		keep_processing = item_remove (fs, file);
		break;
	case QUEUE_CREATED:
		/* Check existence before processing, if requested to do so. */
		if (g_object_get_qdata (G_OBJECT (file),
		                        fs->private->quark_check_existence)) {
			/* Clear the qdata */
			g_object_set_qdata (G_OBJECT (file),
			                    fs->private->quark_check_existence,
			                    GINT_TO_POINTER (FALSE));

			/* Avoid adding items that already exist, when processing
			 * a CREATED task (as those generated when crawling) */
			if (!item_query_exists (fs, file, NULL, NULL)) {
				keep_processing = item_add_or_update (fs, file);
				break;
			}

			/* If already in store, skip processing the CREATED task */
			keep_processing = TRUE;
			break;
		}
		/* Else, fall down and treat as QUEUE_UPDATED */
	case QUEUE_UPDATED:
		keep_processing = item_add_or_update (fs, file);
		break;
	case QUEUE_IGNORE_NEXT_UPDATE:
		keep_processing = item_ignore_next_update (fs, file, source_file);
		break;
	default:
		g_assert_not_reached ();
	}

	if (file) {
		g_object_unref (file);
	}

	if (source_file) {
		g_object_unref (source_file);
	}

	if (!keep_processing) {
		fs->private->item_queues_handler_id = 0;
		return FALSE;
	} else {
		return TRUE;
	}
}

static guint
_tracker_idle_add (TrackerMinerFS *fs,
                   GSourceFunc     func,
                   gpointer        user_data)
{
	guint interval;

	interval = MAX_TIMEOUT_INTERVAL * fs->private->throttle;

	if (interval == 0) {
		return g_idle_add (func, user_data);
	} else {
		return g_timeout_add (interval, func, user_data);
	}
}

static void
item_queue_handlers_set_up (TrackerMinerFS *fs)
{
	if (fs->private->item_queues_handler_id != 0) {
		return;
	}

	if (fs->private->is_paused) {
		return;
	}

	/* Already sent max number of tasks to tracker-extract? */
	if (tracker_processing_pool_wait_limit_reached (fs->private->processing_pool)) {
		return;
	}

	/* Already sent max number of requests to tracker-store?
	 * In this case, we also slow down the processing of items, as we don't
	 * want to keep on extracting if the communication with tracker-store is
	 * very busy. Note that this is not very likely to happen, as the bottleneck
	 * during extraction is not the communication with tracker-store.
	 */
	if (tracker_processing_pool_n_requests_limit_reached (fs->private->processing_pool)) {
		return;
	}

	if (!fs->private->is_crawling) {
		gchar *status;

		g_object_get (fs, "status", &status, NULL);

		if (g_strcmp0 (status, "Processing…") != 0) {
			/* Don't spam this */
			tracker_info ("Processing…");
			g_object_set (fs, "status", "Processing…", NULL);
		}

		g_free (status);
	}

	fs->private->item_queues_handler_id =
		_tracker_idle_add (fs,
		                   item_queue_handlers_cb,
		                   fs);
}

static gboolean
remove_unexisting_file_cb (gpointer key,
                           gpointer value,
                           gpointer user_data)
{
	TrackerMinerFS *fs = user_data;
	GFile *file = key;

	/* If file no longer exists, remove it from the store */
	if (!g_file_query_exists (file, NULL)) {
		gchar *uri;

		uri = g_file_get_uri (file);
		g_debug ("  Marking file which no longer exists in FS for removal: %s", uri);
		g_free (uri);

		trace_eq_push_tail ("DELETED", file, "No longer exists");
		g_queue_push_tail (fs->private->items_deleted,
		                   g_object_ref (file));

		item_queue_handlers_set_up (fs);
	}

	return TRUE;
}

static void
check_if_files_removed (TrackerMinerFS *fs)
{
	g_debug ("Checking if any file was removed...");
	g_hash_table_foreach_remove (fs->private->check_removed,
	                             remove_unexisting_file_cb,
	                             fs);
}

static void
add_to_check_removed_cb (gpointer key,
                         gpointer value,
                         gpointer user_data)
{
	TrackerMinerFS *fs = user_data;
	GFile *file = key;

	/* Not adding any data to the value, we just want
	 * fast search for key availability */
	g_hash_table_insert (fs->private->check_removed,
	                     g_object_ref (file),
	                     NULL);
}

static void
ensure_mtime_cache (TrackerMinerFS *fs,
                    GFile          *file,
                    gboolean        is_parent)
{
	gchar *query, *uri;
	GFile *parent;
	guint cache_size;

	if (G_UNLIKELY (!fs->private->mtime_cache)) {
		fs->private->mtime_cache = g_hash_table_new_full (g_file_hash,
		                                                  (GEqualFunc) g_file_equal,
		                                                  (GDestroyNotify) g_object_unref,
		                                                  (GDestroyNotify) g_free);
	}

	/* Is input file already the parent dir? */
	if (is_parent) {
		g_return_if_fail (file != NULL);
		parent = g_object_ref (file);
	} else {
		/* Note: parent may be NULL if the file represents
		 * the root directory of the file system (applies to
		 * .gvfs mounts also!) */
		parent = g_file_get_parent (file);
	}
	query = NULL;

	if (fs->private->current_mtime_cache_parent) {
		if (parent &&
		    g_file_equal (parent, fs->private->current_mtime_cache_parent)) {
			/* Cache is still valid */
			g_object_unref (parent);
			return;
		}

		g_object_unref (fs->private->current_mtime_cache_parent);
	}

	fs->private->current_mtime_cache_parent = parent;

	g_hash_table_remove_all (fs->private->mtime_cache);

	/* Hack hack alert!
	 * Before querying the store, check if the parent directory is scheduled to
	 * be added, and if so, leave the mtime cache empty.
	 */
	if (g_queue_find_custom (fs->private->items_created,
	                         parent,
	                         (GCompareFunc)tracker_file_cmp) != NULL) {
		uri = g_file_get_uri (file);
		g_debug ("Empty mtime cache for URI '%s' "
		         "(parent scheduled to be created)",
		         uri);
		g_free (uri);
		return;
	}

	if (!parent || (!is_parent && file_is_crawl_directory (fs, file))) {
		/* File is a crawl directory itself, query its mtime directly */
		uri = g_file_get_uri (file);

		g_debug ("Generating mtime cache for URI '%s' (config location)", uri);

		query = g_strdup_printf ("SELECT ?url ?last "
		                         "WHERE { "
		                         "  ?u nfo:fileLastModified ?last ; "
		                         "     nie:url ?url ; "
		                         "     nie:url \"%s\" "
		                         "}",
		                         uri);
		g_free (uri);
	} else if (parent) {
		uri = g_file_get_uri (parent);

		g_debug ("Generating mtime cache for URI '%s'", uri);

		query = g_strdup_printf ("SELECT ?url IF(bound(?damaged) && ?damaged, '0001-01-01T00:00:00Z', STR(?last)) { "
		                         "?u nfo:belongsToContainer ?p ; "
		                         "   nie:url ?url ; "
		                         "   nfo:fileLastModified ?last . "
		                         "?p nie:url \"%s\" . "
		                         "OPTIONAL { ?u tracker:damaged ?damaged }}", uri);

		g_free (uri);
	}

	if (query) {
		CacheQueryData data;

		/* Initialize data contents */
		data.main_loop = g_main_loop_new (NULL, FALSE);
		data.values = g_hash_table_ref (fs->private->mtime_cache);

		tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
		                                       query,
		                                       NULL,
		                                       cache_query_cb,
		                                       &data);
		g_free (query);
		g_main_loop_run (data.main_loop);
		g_main_loop_unref (data.main_loop);
		g_hash_table_unref (data.values);
	}

	cache_size = g_hash_table_size (fs->private->mtime_cache);

	/* Quite ugly hack: If mtime_cache is found EMPTY after the query, still, we
	 * may have a nfo:Folder where nfo:belogsToContainer was not yet set (when
	 * generating the dummy nfo:Folder for mount points). In this case, make a
	 * new query not using nfo:belongsToContainer, and using fn:starts-with
	 * instead. Any better solution is highly appreciated */
	if (parent &&
	    cache_size == 0 &&
	    miner_fs_has_children_without_parent (fs, parent)) {
		CacheQueryData data;

		/* Initialize data contents */
		data.main_loop = g_main_loop_new (NULL, FALSE);
		data.values = g_hash_table_ref (fs->private->mtime_cache);
		uri = g_file_get_uri (parent);

		g_debug ("Generating mtime cache for URI '%s' (fn:starts-with)", uri);

		/* Note the last '/' in the url passed in the FILTER: We want to look for
		 * directory contents, not the directory itself */
		query = g_strdup_printf ("SELECT ?url ?last "
		                         "WHERE { ?u a nfo:Folder ; "
		                         "           nie:url ?url ; "
		                         "           nfo:fileLastModified ?last . "
		                         "        FILTER (fn:starts-with (?url,\"%s/\"))"
		                         "}",
		                         uri);
		g_free (uri);

		tracker_sparql_connection_query_async (tracker_miner_get_connection (TRACKER_MINER (fs)),
		                                       query,
		                                       NULL,
		                                       cache_query_cb,
		                                       &data);
		g_free (query);
		g_main_loop_run (data.main_loop);
		g_main_loop_unref (data.main_loop);
		g_hash_table_unref (data.values);

		/* Note that in this case, the cache may be actually populated with items
		 * which are not direct children of this parent, but doesn't seem a big
		 * issue right now. In the best case, the dummy item that we created will
		 * be there with a proper mtime set. */
		cache_size = g_hash_table_size (fs->private->mtime_cache);
	}

#ifdef MTIME_CACHE_ENABLE_TRACE
	g_debug ("Populated mtime cache with '%u' items", cache_size);
	if (cache_size > 0) {
		GHashTableIter iter;
		gpointer key, value;

		g_hash_table_iter_init (&iter, fs->private->mtime_cache);
		while (g_hash_table_iter_next (&iter, &key, &value)) {
			gchar *fileuri;

			fileuri = g_file_get_uri (key);
			g_debug ("  In mtime cache: '%s','%s'", fileuri, (gchar *) value);
			g_free (fileuri);
		}
	}
#endif /* MTIME_CACHE_ENABLE_TRACE */

	/* Iterate repopulated HT and add all to the check_removed HT */
	g_hash_table_foreach (fs->private->mtime_cache,
	                      add_to_check_removed_cb,
	                      fs);
}

static gboolean
should_change_index_for_file (TrackerMinerFS *fs,
                              GFile          *file)
{
	GFileInfo          *file_info;
	guint64             time;
	time_t              mtime;
	struct tm           t;
	gchar              *time_str, *lookup_time;

	/* Make sure mtime cache contains the mtimes of all files in the
	 * same directory as the given file
	 */
	ensure_mtime_cache (fs, file, FALSE);

	/* Remove the file from the list of files to be checked if removed */
	g_hash_table_remove (fs->private->check_removed, file);

	/* If the file is NOT found in the cache, it means its a new
	 * file the store doesn't know about, so just report it to be
	 * re-indexed.
	 */
	lookup_time = g_hash_table_lookup (fs->private->mtime_cache, file);
	if (!lookup_time) {
		return TRUE;
	}

	file_info = g_file_query_info (file,
	                               G_FILE_ATTRIBUTE_TIME_MODIFIED,
	                               G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
	                               NULL,
	                               NULL);
	if (!file_info) {
		/* NOTE: We return TRUE here because we want to update the DB
		 * about this file, not because we want to index it.
		 */
		return TRUE;
	}

	time = g_file_info_get_attribute_uint64 (file_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);
	mtime = (time_t) time;
	g_object_unref (file_info);

	gmtime_r (&mtime, &t);

	time_str = g_strdup_printf ("%04d-%02d-%02dT%02d:%02d:%02dZ",
	                            t.tm_year + 1900,
	                            t.tm_mon + 1,
	                            t.tm_mday,
	                            t.tm_hour,
	                            t.tm_min,
	                            t.tm_sec);

	if (strcmp (time_str, lookup_time) == 0) {
		/* File already up-to-date in the database */
		g_free (time_str);
		return FALSE;
	}

	g_free (time_str);

	/* File either not yet in the database or mtime is different
	 * Update in database required
	 */
	return TRUE;
}

static gboolean
should_check_file (TrackerMinerFS *fs,
                   GFile          *file,
                   gboolean        is_dir)
{
	gboolean should_check;

	if (is_dir) {
		g_signal_emit (fs, signals[CHECK_DIRECTORY], 0, file, &should_check);
	} else {
		g_signal_emit (fs, signals[CHECK_FILE], 0, file, &should_check);
	}

	return should_check;
}

static gboolean
should_process_file (TrackerMinerFS *fs,
                     GFile          *file,
                     gboolean        is_dir)
{
	if (!should_check_file (fs, file, is_dir)) {
		ensure_mtime_cache (fs, file, FALSE);

		if (g_hash_table_lookup (fs->private->mtime_cache, file) != NULL) {
			/* File is told not to be checked, but exists
			 * in the store, put in deleted queue.
			 */
			trace_eq_push_tail ("DELETED", file, "No longer to be indexed");
			g_queue_push_tail (fs->private->items_deleted,
			                   g_object_ref (file));
		}
		return FALSE;
	}

	/* Check whether file is up-to-date in tracker-store */
	return should_change_index_for_file (fs, file);
}

static void
monitor_item_created_cb (TrackerMonitor *monitor,
                         GFile          *file,
                         gboolean        is_directory,
                         gpointer        user_data)
{
	TrackerMinerFS *fs;
	gboolean should_process = TRUE;
	gchar *uri;

	fs = user_data;
	should_process = should_check_file (fs, file, is_directory);

	uri = g_file_get_uri (file);

	g_debug ("%s:'%s' (%s) (create monitor event or user request)",
	         should_process ? "Found " : "Ignored",
	         uri,
	         is_directory ? "DIR" : "FILE");

	if (should_process) {
		if (is_directory &&
		    should_recurse_for_directory (fs, file)) {
			tracker_miner_fs_directory_add_internal (fs, file);
		} else {
			trace_eq_push_tail ("CREATED", file, "On monitor event");
			g_queue_push_tail (fs->private->items_created,
			                   g_object_ref (file));

			item_queue_handlers_set_up (fs);
		}
	}

	g_free (uri);
}

static void
monitor_item_updated_cb (TrackerMonitor *monitor,
                         GFile          *file,
                         gboolean        is_directory,
                         gpointer        user_data)
{
	TrackerMinerFS *fs;
	gboolean should_process;
	gchar *uri;

	fs = user_data;
	should_process = should_check_file (fs, file, is_directory);

	uri = g_file_get_uri (file);

	g_debug ("%s:'%s' (%s) (update monitor event or user request)",
	         should_process ? "Found " : "Ignored",
	         uri,
	         is_directory ? "DIR" : "FILE");

	if (should_process) {
		trace_eq_push_tail ("UPDATED", file, "On monitor event");
		g_queue_push_tail (fs->private->items_updated,
		                   g_object_ref (file));

		item_queue_handlers_set_up (fs);
	}

	g_free (uri);
}

static void
monitor_item_attribute_updated_cb (TrackerMonitor *monitor,
                                   GFile          *file,
                                   gboolean        is_directory,
                                   gpointer        user_data)
{
	TrackerMinerFS *fs;
	gboolean should_process;
	gchar *uri;

	fs = user_data;
	should_process = should_check_file (fs, file, is_directory);

	uri = g_file_get_uri (file);

	g_debug ("%s:'%s' (%s) (attribute update monitor event or user request)",
	         should_process ? "Found " : "Ignored",
	         uri,
	         is_directory ? "DIR" : "FILE");

	if (should_process) {
		/* Set the Quark specifying that ONLY attributes were
		 * modified */
		g_object_set_qdata (G_OBJECT (file),
		                    fs->private->quark_attribute_updated,
		                    GINT_TO_POINTER (TRUE));
		trace_eq_push_tail ("UPDATED", file, "On monitor event (attributes)");
		g_queue_push_tail (fs->private->items_updated,
		                   g_object_ref (file));

		item_queue_handlers_set_up (fs);
	}

	g_free (uri);
}

static void
monitor_item_deleted_cb (TrackerMonitor *monitor,
                         GFile          *file,
                         gboolean        is_directory,
                         gpointer        user_data)
{
	TrackerMinerFS *fs;
	gboolean should_process;
	gchar *uri;

	fs = user_data;
	should_process = should_check_file (fs, file, is_directory);

	uri = g_file_get_uri (file);

	g_debug ("%s:'%s' (%s) (delete monitor event or user request)",
	         should_process ? "Found " : "Ignored",
	         uri,
	         is_directory ? "DIR" : "FILE");

	if (should_process) {
		trace_eq_push_tail ("DELETED", file, "On monitor event");
		g_queue_push_tail (fs->private->items_deleted,
		                   g_object_ref (file));

		item_queue_handlers_set_up (fs);
	}

#if 0
	/* FIXME: Should we do this for MOVE events too? */

	/* Remove directory from list of directories we are going to
	 * iterate if it is in there.
	 */
	l = g_list_find_custom (fs->private->directories,
	                        path,
	                        (GCompareFunc) g_strcmp0);

	/* Make sure we don't remove the current device we are
	 * processing, this is because we do this same clean up later
	 * in process_device_next()
	 */
	if (l && l != fs->private->current_directory) {
		directory_data_unref (l->data);
		fs->private->directories =
			g_list_delete_link (fs->private->directories, l);
	}
#endif

	g_free (uri);
}

static void
monitor_item_moved_cb (TrackerMonitor *monitor,
                       GFile          *file,
                       GFile          *other_file,
                       gboolean        is_directory,
                       gboolean        is_source_monitored,
                       gpointer        user_data)
{
	TrackerMinerFS *fs;

	fs = user_data;

	if (!is_source_monitored) {
		if (is_directory) {
			/* Remove monitors if any */
			tracker_monitor_remove_recursively (fs->private->monitor,
			                                    file);
			if (should_recurse_for_directory (fs, other_file)) {
				gchar *uri;

				uri = g_file_get_uri (other_file);
				g_debug ("Not in store:'?'->'%s' (DIR) "
				         "(move monitor event, source unknown)",
				         uri);
				/* If the source is not monitored, we need to crawl it. */
				tracker_miner_fs_directory_add_internal (fs, other_file);
				g_free (uri);
			}
		}
		/* else, file, do nothing */
	} else {
		gchar *uri;
		gchar *other_uri;
		gboolean source_stored, should_process_other;

		uri = g_file_get_uri (file);
		other_uri = g_file_get_uri (other_file);

		source_stored = item_query_exists (fs, file, NULL, NULL);
		should_process_other = should_check_file (fs, other_file, is_directory);

		g_debug ("%s:'%s'->'%s':%s (%s) (move monitor event or user request)",
		         source_stored ? "In store" : "Not in store",
		         uri,
		         other_uri,
		         should_process_other ? "Found " : "Ignored",
		         is_directory ? "DIR" : "FILE");

		/* FIXME: Guessing this soon the queue the event should pertain
		 *        to could introduce race conditions if events from other
		 *        queues for the same files are processed before items_moved,
		 *        Most of these decisions should be taken when the event is
		 *        actually being processed.
		 */
		if (!source_stored) {
			/* Remove monitors if any */
			if (is_directory) {
				tracker_monitor_remove_recursively (fs->private->monitor,
				                                    file);
			}

			if (should_process_other) {
				/* Source file was not stored, check dest file as new */
				if (!is_directory ||
				    !should_recurse_for_directory (fs, other_file)) {
					trace_eq_push_tail ("CREATED", other_file, "On move monitor event");
					g_queue_push_tail (fs->private->items_created,
					                   g_object_ref (other_file));

					item_queue_handlers_set_up (fs);
				} else {
					tracker_miner_fs_directory_add_internal (fs, other_file);
				}
			}
			/* Else, do nothing else */
		} else if (!should_process_other) {
			/* Remove monitors if any */
			if (is_directory) {
				tracker_monitor_remove_recursively (fs->private->monitor,
				                                    file);
			}

			/* Delete old file */
			trace_eq_push_tail ("DELETED", file, "On move monitor event");
			g_queue_push_tail (fs->private->items_deleted,
			                   g_object_ref (file));
			item_queue_handlers_set_up (fs);
		} else {
			/* Move monitors to the new place */
			if (is_directory) {
				tracker_monitor_move (fs->private->monitor,
				                      file,
				                      other_file);
			}
			/* Move old file to new file */
			trace_eq_push_tail_2 ("MOVED", file, other_file, "On monitor event");
			g_queue_push_tail (fs->private->items_moved,
			                   item_moved_data_new (other_file, file));
			item_queue_handlers_set_up (fs);
		}

		g_free (other_uri);
		g_free (uri);
	}
}

static gboolean
crawler_check_file_cb (TrackerCrawler *crawler,
                       GFile          *file,
                       gpointer        user_data)
{
	TrackerMinerFS *fs = user_data;

	if (!fs->private->been_crawled &&
	    (!fs->private->mtime_checking ||
	     !fs->private->initial_crawling)) {
		return FALSE;
	}

	return should_process_file (fs, file, FALSE);
}

static gboolean
crawler_check_directory_cb (TrackerCrawler *crawler,
                            GFile          *file,
                            gpointer        user_data)
{
	TrackerMinerFS *fs = user_data;
	gboolean should_check;

	should_check = should_check_file (fs, file, TRUE);

	if (!should_check) {
		/* Remove monitors if any */
		tracker_monitor_remove (fs->private->monitor, file);

		/* Put item in deleted queue if it existed in the store */
		ensure_mtime_cache (fs, file, FALSE);

		if (g_hash_table_lookup (fs->private->mtime_cache, file) != NULL) {
			/* File is told not to be checked, but exists
			 * in the store, put in deleted queue.
			 */
			trace_eq_push_tail ("DELETED", file, "while crawling directory");
			g_queue_push_tail (fs->private->items_deleted,
			                   g_object_ref (file));
		}
	} else {
		gboolean should_change_index;

		if (!fs->private->been_crawled &&
		    (!fs->private->mtime_checking ||
		     !fs->private->initial_crawling)) {
			should_change_index = FALSE;
		} else {
			should_change_index = should_change_index_for_file (fs, file);
		}

		if (!should_change_index) {
			/* Mark the file as ignored, we still want the crawler
			 * to iterate over its contents, but the directory hasn't
			 * actually changed, hence this flag.
			 */
			g_object_set_qdata (G_OBJECT (file),
			                    fs->private->quark_ignore_file,
			                    GINT_TO_POINTER (TRUE));
		}
	}

	/* We _HAVE_ to check ALL directories because mtime updates
	 * are not guaranteed on parents on Windows AND we on Linux
	 * only the immediate parent directory mtime is updated, this
	 * is not done recursively.
	 *
	 * As such, we only use the "check" rules here, we don't do
	 * any database comparison with mtime.
	 */
	return should_check;
}

static gboolean
crawler_check_directory_contents_cb (TrackerCrawler *crawler,
                                     GFile          *parent,
                                     GList          *children,
                                     gpointer        user_data)
{
	TrackerMinerFS *fs = user_data;
	gboolean add_monitor = FALSE;
	gboolean process;

	g_signal_emit (fs, signals[CHECK_DIRECTORY_CONTENTS], 0, parent, children, &process);

	if (process) {
		g_signal_emit (fs, signals[MONITOR_DIRECTORY], 0, parent, &add_monitor);

		/* If the directory crawled doesn't have ANY file, we need to
		 * force a mtime cache reload using the given directory as input
		 * parent to mtime_cache. This is done in order to track any
		 * possible deleted file on the directory if the directory is fully
		 * empty (if there is at least one file in the directory,
		 * ensure_mtime_cache() will be called using the given file as input,
		 * and thus reloading the mtime cache at least once). */
		if (!children) {
			ensure_mtime_cache (fs, parent, TRUE);
		}
	}

	/* FIXME: Should we add here or when we process the queue in
	 * the finished sig?
	 */
	if (add_monitor) {
		/* Only if:
		 * -First crawl has already been done OR
		 * -mtime_checking is TRUE.
		 */
		if (fs->private->been_crawled || fs->private->mtime_checking) {
			/* Set quark so that before trying to add the item we first
			 * check for its existence. */
			g_object_set_qdata (G_OBJECT (parent),
			                    fs->private->quark_check_existence,
			                    GINT_TO_POINTER (TRUE));

			/* Before adding the monitor, start notifying the store
			 * about the new directory, so that if any file event comes
			 * afterwards, the directory is already in store. */
			trace_eq_push_tail ("CREATED", parent, "while crawling directory, parent");
			g_queue_push_tail (fs->private->items_created,
			                   g_object_ref (parent));
			item_queue_handlers_set_up (fs);

			/* As we already added here, specify that it shouldn't be added
			 * any more */
			g_object_set_qdata (G_OBJECT (parent),
			                    fs->private->quark_ignore_file,
			                    GINT_TO_POINTER (TRUE));
		}

		tracker_monitor_add (fs->private->monitor, parent);
	} else {
		tracker_monitor_remove (fs->private->monitor, parent);
	}

	return process;
}

#ifdef CRAWLED_TREE_ENABLE_TRACE

static gboolean
print_file_tree (GNode    *node,
                 gpointer  user_data)
{
	gchar *name;
	gint i;

	name = g_file_get_basename (node->data);

	/* Indentation */
	for (i = g_node_depth (node) - 1; i > 0; i--) {
		g_print ("  ");
	}

	g_print ("%s\n", name);
	g_free (name);

	return FALSE;
}

#endif /* CRAWLED_TREE_ENABLE_TRACE */

static CrawledDirectoryData *
crawled_directory_data_new (GNode *tree)
{
	CrawledDirectoryData *data;

	data = g_slice_new (CrawledDirectoryData);
	data->tree = g_node_copy_deep (tree, (GCopyFunc) g_object_ref, NULL);
	data->nodes = g_queue_new ();

	data->n_items = g_node_n_nodes (data->tree, G_TRAVERSE_ALL);
	data->n_items_processed = 0;

	return data;
}

static gboolean
crawled_directory_data_free_foreach (GNode    *node,
                                     gpointer  user_data)
{
	g_object_unref (node->data);
	return FALSE;
}

static void
crawled_directory_data_free (CrawledDirectoryData *data)
{
	g_node_traverse (data->tree,
	                 G_POST_ORDER,
	                 G_TRAVERSE_ALL,
	                 -1,
	                 crawled_directory_data_free_foreach,
	                 NULL);
	g_node_destroy (data->tree);

	g_queue_free (data->nodes);

	g_slice_free (CrawledDirectoryData, data);
}

static void
crawler_directory_crawled_cb (TrackerCrawler *crawler,
                              GFile          *directory,
                              GNode          *tree,
                              guint           directories_found,
                              guint           directories_ignored,
                              guint           files_found,
                              guint           files_ignored,
                              gpointer        user_data)
{
	TrackerMinerFS *fs = user_data;
	CrawledDirectoryData *dir_data;

#ifdef CRAWLED_TREE_ENABLE_TRACE
	/* Debug printing of the directory tree */
	g_node_traverse (tree, G_PRE_ORDER, G_TRAVERSE_ALL, -1,
	                 print_file_tree, NULL);
#endif /* CRAWLED_TREE_ENABLE_TRACE */

	/* Add tree to the crawled directories queue, this queue
	 * will be used to fill priv->items_created in when no
	 * further data is left there.
	 */
	dir_data = crawled_directory_data_new (tree);
	g_queue_push_tail (fs->private->crawled_directories, dir_data);

	/* Update stats */
	fs->private->directories_found += directories_found;
	fs->private->directories_ignored += directories_ignored;
	fs->private->files_found += files_found;
	fs->private->files_ignored += files_ignored;

	fs->private->total_directories_found += directories_found;
	fs->private->total_directories_ignored += directories_ignored;
	fs->private->total_files_found += files_found;
	fs->private->total_files_ignored += files_ignored;

	g_message ("  Found %d directories, ignored %d directories",
	           directories_found,
	           directories_ignored);
	g_message ("  Found %d files, ignored %d files",
	           files_found,
	           files_ignored);
}

static void
crawler_finished_cb (TrackerCrawler *crawler,
                     gboolean        was_interrupted,
                     gpointer        user_data)
{
	TrackerMinerFS *fs = user_data;

	fs->private->is_crawling = FALSE;

	tracker_info ("%s crawling files after %2.2f seconds",
	              was_interrupted ? "Stopped" : "Finished",
	              g_timer_elapsed (fs->private->timer, NULL));

	directory_data_unref (fs->private->current_directory);
	fs->private->current_directory = NULL;

	/* Check if any file was left after whole crawling */
	check_if_files_removed (fs);

	/* Proceed to next thing to process */
	crawl_directories_start (fs);
}

static gboolean
crawl_directories_cb (gpointer user_data)
{
	TrackerMinerFS *fs = user_data;
	gchar *path, *path_utf8;
	gchar *str;

	if (fs->private->current_directory) {
		g_critical ("One directory is already being processed, bailing out");
		fs->private->crawl_directories_id = 0;
		return FALSE;
	}

	if (!fs->private->directories) {
		if (fs->private->current_iri_cache_parent) {
			/* Unset parent folder so caches are regenerated */
			g_object_unref (fs->private->current_iri_cache_parent);
			fs->private->current_iri_cache_parent = NULL;
		}

		if (fs->private->current_mtime_cache_parent) {
			/* Unset parent folder so caches are regenerated */
			g_object_unref (fs->private->current_mtime_cache_parent);
			fs->private->current_mtime_cache_parent = NULL;
		}

		/* Now we handle the queue */
		item_queue_handlers_set_up (fs);
		crawl_directories_stop (fs);

		fs->private->crawl_directories_id = 0;
		return FALSE;
	}

	if (!fs->private->timer) {
		fs->private->timer = g_timer_new ();
	}

	fs->private->current_directory = fs->private->directories->data;
	fs->private->directories = g_list_remove (fs->private->directories,
	                                          fs->private->current_directory);

	path = g_file_get_path (fs->private->current_directory->file);
	path_utf8 = g_filename_to_utf8 (path, -1, NULL, NULL, NULL);
	g_free (path);

	if (fs->private->current_directory->recurse) {
		str = g_strdup_printf ("Crawling recursively directory '%s'", path_utf8);
	} else {
		str = g_strdup_printf ("Crawling single directory '%s'", path_utf8);
	}
	g_free (path_utf8);

	tracker_info ("%s", str);

	/* Always set the progress here to at least 1% */
	g_object_set (fs,
	              "progress", 0.01,
	              "status", str,
	              NULL);
	g_free (str);

	if (tracker_crawler_start (fs->private->crawler,
	                           fs->private->current_directory->file,
	                           fs->private->current_directory->recurse)) {
		/* Crawler when restart the idle function when done */
		fs->private->is_crawling = TRUE;
		fs->private->crawl_directories_id = 0;
		return FALSE;
	}

	/* Directory couldn't be processed */
	directory_data_unref (fs->private->current_directory);
	fs->private->current_directory = NULL;

	return TRUE;
}

static void
crawl_directories_start (TrackerMinerFS *fs)
{
	if (!fs->private->initial_crawling) {
		/* Do not perform initial crawling */
		g_message ("Crawling is disabled, waiting for DBus events");
		process_stop (fs);
		return;
	}

	if (fs->private->crawl_directories_id != 0 ||
	    fs->private->current_directory) {
		/* Processing ALREADY going on */
		return;
	}

	if (!fs->private->been_started) {
		/* Miner has not been started yet */
		return;
	}

	if (!fs->private->timer) {
		fs->private->timer = g_timer_new ();
	}

	fs->private->directories_found = 0;
	fs->private->directories_ignored = 0;
	fs->private->files_found = 0;
	fs->private->files_ignored = 0;

	fs->private->crawl_directories_id = _tracker_idle_add (fs, crawl_directories_cb, fs);
}

static void
crawl_directories_stop (TrackerMinerFS *fs)
{
	if (fs->private->crawl_directories_id == 0) {
		/* No processing going on, nothing to stop */
		return;
	}

	if (fs->private->current_directory) {
		tracker_crawler_stop (fs->private->crawler);
	}

	/* Is this the right time to emit FINISHED? What about
	 * monitor events left to handle? Should they matter
	 * here?
	 */
	if (fs->private->crawl_directories_id != 0) {
		g_source_remove (fs->private->crawl_directories_id);
		fs->private->crawl_directories_id = 0;
	}
}

static gboolean
should_recurse_for_directory (TrackerMinerFS *fs,
                              GFile          *file)
{
	gboolean recurse = FALSE;
	GList *dirs;

	for (dirs = fs->private->config_directories; dirs; dirs = dirs->next) {
		DirectoryData *data;

		data = dirs->data;

		if (data->recurse &&
		    (g_file_equal (file, data->file) ||
		     g_file_has_prefix (file, data->file))) {
			/* File is inside a recursive dir */
			recurse = TRUE;
			break;
		}
	}

	return recurse;
}


/* Returns 0 if 'a' and 'b' point to the same diretory, OR if
 *  'b' is contained inside directory 'a' and 'a' is recursively
 *  indexed. */
static gint
directory_compare_cb (gconstpointer a,
                      gconstpointer b)
{
	DirectoryData *dda = (DirectoryData *) a;
	DirectoryData *ddb = (DirectoryData *) b;

	return (g_file_equal (dda->file, ddb->file) ||
	        (dda->recurse &&
	         g_file_has_prefix (ddb->file, dda->file))) ? 0 : -1;
}


/* This function is for internal use, adds the file to the processing
 * queue with the same directory settings than the corresponding
 * config directory.
 */
static void
tracker_miner_fs_directory_add_internal (TrackerMinerFS *fs,
                                         GFile          *file)
{
	DirectoryData *data;
	gboolean recurse;

	recurse = should_recurse_for_directory (fs, file);
	data = directory_data_new (file, recurse);

	/* Only add if not already there */
	if (!g_list_find_custom (fs->private->directories,
	                         data,
	                         directory_compare_cb)) {
		fs->private->directories =
			g_list_append (fs->private->directories,
			               directory_data_ref (data));

		crawl_directories_start (fs);
	}

	directory_data_unref (data);
}

/**
 * tracker_miner_fs_directory_add:
 * @fs: a #TrackerMinerFS
 * @file: #GFile for the directory to inspect
 * @recurse: whether the directory should be inspected recursively
 *
 * Tells the filesystem miner to inspect a directory.
 *
 * Since: 0.8
 **/
void
tracker_miner_fs_directory_add (TrackerMinerFS *fs,
                                GFile          *file,
                                gboolean        recurse)
{
	DirectoryData *dir_data;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));
	g_return_if_fail (G_IS_FILE (file));

	dir_data = directory_data_new (file, recurse);

	/* New directory to add in config_directories? */
	if (!g_list_find_custom (fs->private->config_directories,
	                         dir_data,
	                         directory_compare_cb)) {
		fs->private->config_directories =
			g_list_append (fs->private->config_directories,
			               directory_data_ref (dir_data));
	}

	/* If not already in the list to process, add it */
	if (!g_list_find_custom (fs->private->directories,
	                         dir_data,
	                         directory_compare_cb)) {
		fs->private->directories =
			g_list_append (fs->private->directories,
			               directory_data_ref (dir_data));

		crawl_directories_start (fs);
	}

	directory_data_unref (dir_data);
}

static void
check_files_removal (GQueue *queue,
                     GFile  *parent)
{
	GList *l;

	l = queue->head;

	while (l) {
		GFile *file = l->data;
		GList *link = l;

		l = l->next;

		if (g_file_equal (file, parent) ||
		    g_file_has_prefix (file, parent)) {
			g_queue_delete_link (queue, link);
			g_object_unref (file);
		}
	}
}

static void
processing_pool_cancel_foreach (gpointer data,
                                gpointer user_data)
{
	TrackerProcessingTask *task = data;
	GFile *file = user_data;
	GFile *task_file;
	UpdateProcessingTaskContext *ctxt;

	task_file = tracker_processing_task_get_file (task);
	ctxt = tracker_processing_task_get_context (task);

	if (ctxt &&
	    ctxt->cancellable &&
	    (g_file_equal (task_file, file) ||
	     g_file_has_prefix (task_file, file))) {
		g_cancellable_cancel (ctxt->cancellable);
	}
}

/**
 * tracker_miner_fs_directory_remove:
 * @fs: a #TrackerMinerFS
 * @file: #GFile for the directory to be removed
 *
 * Removes a directory from being inspected by @fs. Note that only directory
 *  watches are removed.
 *
 * Returns: %TRUE if the directory was successfully removed.
 *
 * Since: 0.8
 **/
gboolean
tracker_miner_fs_directory_remove (TrackerMinerFS *fs,
                                   GFile          *file)
{
	TrackerMinerFSPrivate *priv;
	gboolean return_val = FALSE;
	GList *dirs;

	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), FALSE);
	g_return_val_if_fail (G_IS_FILE (file), FALSE);

	priv = fs->private;

	if (fs->private->current_directory) {
		GFile *current_file;

		current_file = fs->private->current_directory->file;

		if (g_file_equal (file, current_file) ||
		    g_file_has_prefix (file, current_file)) {
			/* Dir is being processed currently, cancel crawler */
			tracker_crawler_stop (fs->private->crawler);
			return_val = TRUE;
		}
	}

	dirs = fs->private->directories;

	while (dirs) {
		DirectoryData *data = dirs->data;
		GList *link = dirs;

		dirs = dirs->next;

		if (g_file_equal (file, data->file) ||
		    g_file_has_prefix (file, data->file)) {
			directory_data_unref (data);
			fs->private->directories = g_list_delete_link (fs->private->directories, link);
			return_val = TRUE;
		}
	}

	dirs = fs->private->config_directories;

	while (dirs) {
		DirectoryData *data = dirs->data;
		GList *link = dirs;

		dirs = dirs->next;

		if (g_file_equal (file, data->file)) {
			directory_data_unref (data);
			fs->private->config_directories = g_list_delete_link (fs->private->config_directories, link);
			return_val = TRUE;
		}
	}

	/* Remove anything contained in the removed directory
	 * from all relevant processing queues.
	 */
	check_files_removal (priv->items_updated, file);
	check_files_removal (priv->items_created, file);

	/* Cancel all pending tasks on files inside the path given by file */
	tracker_processing_pool_foreach (fs->private->processing_pool,
	                                 processing_pool_cancel_foreach,
	                                 file);

	/* Remove all monitors */
	tracker_monitor_remove_recursively (fs->private->monitor, file);

	return return_val;
}


/**
 * tracker_miner_fs_directory_remove_full:
 * @fs: a #TrackerMinerFS
 * @file: #GFile for the directory to be removed
 *
 * Removes a directory from being inspected by @fs, and removes all
 * associated metadata of the directory (and its contents) from the
 * store.
 *
 * Returns: %TRUE if the directory was successfully removed.
 *
 * Since: 0.10
 **/
gboolean
tracker_miner_fs_directory_remove_full (TrackerMinerFS *fs,
                                        GFile          *file)
{
	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), FALSE);
	g_return_val_if_fail (G_IS_FILE (file), FALSE);

	/* Tell miner not to keep on inspecting the directory... */
	if (tracker_miner_fs_directory_remove (fs, file)) {
		/* And remove all info about the directory (recursively)
		 * from the store... */
		trace_eq_push_tail ("DELETED", file, "on remove full");
		g_queue_push_tail (fs->private->items_deleted,
		                   g_object_ref (file));
		item_queue_handlers_set_up (fs);

		return TRUE;
	}

	return FALSE;
}

static gboolean
check_file_parents (TrackerMinerFS *fs,
                    GFile          *file)
{
	DirectoryData *data;
	GFile *parent;
	GList *parents = NULL, *p;

	parent = g_file_get_parent (file);

	if (!parent) {
		return FALSE;
	}

	data = find_config_directory (fs, parent);

	if (!data) {
		return FALSE;
	}

	/* Add parent directories until we're past the config dir */
	while (parent &&
	       !g_file_has_prefix (data->file, parent)) {
		parents = g_list_prepend (parents, parent);
		parent = g_file_get_parent (parent);
	}

	/* Last parent fetched is not added to the list */
	if (parent) {
		g_object_unref (parent);
	}

	for (p = parents; p; p = p->next) {
		trace_eq_push_tail ("UPDATED", p->data, "checking file parents");
		g_queue_push_tail (fs->private->items_updated, p->data);
	}

	g_list_free (parents);

	return TRUE;
}

/**
 * tracker_miner_fs_check_file:
 * @fs: a #TrackerMinerFS
 * @file: #GFile for the file to check
 * @check_parents: whether to check parents and eligibility or not
 *
 * Tells the filesystem miner to check and index a file,
 * this file must be part of the usual crawling directories
 * of #TrackerMinerFS. See tracker_miner_fs_directory_add().
 *
 * Since: 0.10
 **/
void
tracker_miner_fs_check_file (TrackerMinerFS *fs,
                             GFile          *file,
                             gboolean        check_parents)
{
	gboolean should_process = TRUE;
	gchar *path;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));
	g_return_if_fail (G_IS_FILE (file));

	if (check_parents) {
		should_process = should_check_file (fs, file, FALSE);
	}

	path = g_file_get_path (file);

	g_debug ("%s:'%s' (FILE) (requested by application)",
	         should_process ? "Found " : "Ignored",
	         path);

	if (should_process) {
		if (check_parents && !check_file_parents (fs, file)) {
			return;
		}

		trace_eq_push_tail ("UPDATED", file, "Requested by application");
		g_queue_push_tail (fs->private->items_updated,
		                   g_object_ref (file));

		item_queue_handlers_set_up (fs);
	}

	g_free (path);
}

/**
 * tracker_miner_fs_check_directory:
 * @fs: a #TrackerMinerFS
 * @file: #GFile for the directory to check
 * @check_parents: whether to check parents and eligibility or not
 *
 * Tells the filesystem miner to check and index a directory,
 * this file must be part of the usual crawling directories
 * of #TrackerMinerFS. See tracker_miner_fs_directory_add().
 *
 * Since: 0.10
 **/
void
tracker_miner_fs_check_directory (TrackerMinerFS *fs,
                                  GFile          *file,
                                  gboolean        check_parents)
{
	gboolean should_process = TRUE;
	gchar *path;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));
	g_return_if_fail (G_IS_FILE (file));

	if (check_parents) {
		should_process = should_check_file (fs, file, TRUE);
	}

	path = g_file_get_path (file);

	g_debug ("%s:'%s' (DIR) (requested by application)",
	         should_process ? "Found " : "Ignored",
	         path);

	if (should_process) {
		if (check_parents && !check_file_parents (fs, file)) {
			return;
		}

		tracker_miner_fs_directory_add_internal (fs, file);
	}

	g_free (path);
}

/**
 * tracker_miner_fs_file_notify:
 * @fs: a #TrackerMinerFS
 * @file: a #GFile
 * @error: a #GError with the error that happened during processing, or %NULL.
 *
 * Notifies @fs that all processing on @file has been finished, if any error
 * happened during file data processing, it should be passed in @error, else
 * that parameter will contain %NULL to reflect success.
 *
 * Since: 0.8
 **/
void
tracker_miner_fs_file_notify (TrackerMinerFS *fs,
                              GFile          *file,
                              const GError   *error)
{
	TrackerProcessingTask *task;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));
	g_return_if_fail (G_IS_FILE (file));

	fs->private->total_files_notified++;

	task = tracker_processing_pool_find_task (fs->private->processing_pool,
	                                          file,
	                                          FALSE);

	if (!task) {
		gchar *uri;

		uri = g_file_get_uri (file);
		g_critical ("%s has notified that file '%s' has been processed, "
		            "but that file was not in the processing queue. "
		            "This is an implementation error, please ensure that "
		            "tracker_miner_fs_file_notify() is called on the same "
		            "GFile that is passed in ::process-file, and that this"
		            "signal didn't return FALSE for it",
		            G_OBJECT_TYPE_NAME (fs), uri);
		g_free (uri);

		return;
	}

	item_add_or_update_cb (fs, task, error);
}

/**
 * tracker_miner_fs_set_throttle:
 * @fs: a #TrackerMinerFS
 * @throttle: throttle value, between 0 and 1
 *
 * Tells the filesystem miner to throttle its operations.
 * a value of 0 means no throttling at all, so the miner
 * will perform operations at full speed, 1 is the slowest
 * value.
 *
 * Since: 0.8
 **/
void
tracker_miner_fs_set_throttle (TrackerMinerFS *fs,
                               gdouble         throttle)
{
	g_return_if_fail (TRACKER_IS_MINER_FS (fs));

	throttle = CLAMP (throttle, 0, 1);

	if (fs->private->throttle == throttle) {
		return;
	}

	fs->private->throttle = throttle;

	/* Update timeouts */
	if (fs->private->item_queues_handler_id != 0) {
		g_source_remove (fs->private->item_queues_handler_id);

		fs->private->item_queues_handler_id =
			_tracker_idle_add (fs,
			                   item_queue_handlers_cb,
			                   fs);
	}

	if (fs->private->crawl_directories_id) {
		g_source_remove (fs->private->crawl_directories_id);

		fs->private->crawl_directories_id =
			_tracker_idle_add (fs, crawl_directories_cb, fs);
	}
}

/**
 * tracker_miner_fs_get_throttle:
 * @fs: a #TrackerMinerFS
 *
 * Gets the current throttle value. see tracker_miner_fs_set_throttle().
 *
 * Returns: current throttle value.
 *
 * Since: 0.8
 **/
gdouble
tracker_miner_fs_get_throttle (TrackerMinerFS *fs)
{
	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), 0);

	return fs->private->throttle;
}

/**
 * tracker_miner_fs_get_urn:
 * @fs: a #TrackerMinerFS
 * @file: a #GFile obtained in #TrackerMinerFS::process-file
 *
 * If the item exists in the store, this function retrieves
 * the URN for a #GFile being currently processed.

 * If @file is not being currently processed by @fs, or doesn't
 * exist in the store yet, %NULL will be returned.
 *
 * Returns: (transfer none): The URN containing the data associated to @file,
 *          or %NULL.
 *
 * Since: 0.8
 **/
G_CONST_RETURN gchar *
tracker_miner_fs_get_urn (TrackerMinerFS *fs,
                          GFile          *file)
{
	TrackerProcessingTask *task;

	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), NULL);
	g_return_val_if_fail (G_IS_FILE (file), NULL);

	/* Check if found in currently processed data */
	task = tracker_processing_pool_find_task (fs->private->processing_pool,
	                                          file,
	                                          FALSE);

	if (!task) {
		gchar *uri;

		uri = g_file_get_uri (file);

		g_critical ("File '%s' is not being currently processed, "
		            "so the URN cannot be retrieved.", uri);
		g_free (uri);

		return NULL;
	} else {
		UpdateProcessingTaskContext *ctxt;

		/* We are only storing the URN in the created/updated tasks */
		ctxt = tracker_processing_task_get_context (task);
		if (!ctxt) {
			gchar *uri;

			uri = g_file_get_uri (file);
			g_critical ("File '%s' is being processed, but not as a "
			            "CREATED/UPDATED task, so cannot get URN",
			            uri);
			g_free (uri);
			return NULL;
		}

		return ctxt->urn;
	}
}

/**
 * tracker_miner_fs_query_urn:
 * @fs: a #TrackerMinerFS
 * @file: a #GFile
 *
 * If the item exists in the store, this function retrieves
 * the URN of the given #GFile

 * If @file doesn't exist in the store yet, %NULL will be returned.
 *
 * Returns: (transfer full): A newly allocated string with the URN containing the data associated
 *          to @file, or %NULL.
 *
 * Since: 0.10
 **/
gchar *
tracker_miner_fs_query_urn (TrackerMinerFS *fs,
                            GFile          *file)
{
	gchar *iri = NULL;

	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), NULL);
	g_return_val_if_fail (G_IS_FILE (file), NULL);

	/* We don't really need to check the return value here, just
	 * looking at the output iri is enough. */
	item_query_exists (fs, file, &iri, NULL);

	return iri;
}

/**
 * tracker_miner_fs_get_parent_urn:
 * @fs: a #TrackerMinerFS
 * @file: a #GFile obtained in #TrackerMinerFS::process-file
 *
 * If @file is currently being processed by @fs, this function
 * will return the parent folder URN if any. This function is
 * useful to set the nie:belongsToContainer relationship. The
 * processing order of #TrackerMinerFS guarantees that a folder
 * has been already fully processed for indexing before any
 * children is processed, so most usually this function should
 * return non-%NULL.
 *
 * Returns: (transfer none): The parent folder URN, or %NULL.
 *
 * Since: 0.8
 **/
G_CONST_RETURN gchar *
tracker_miner_fs_get_parent_urn (TrackerMinerFS *fs,
                                 GFile          *file)
{
	TrackerProcessingTask *task;

	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), NULL);
	g_return_val_if_fail (G_IS_FILE (file), NULL);

	/* Check if found in currently processed data */
	task = tracker_processing_pool_find_task (fs->private->processing_pool,
	                                          file,
	                                          FALSE);

	if (!task) {
		gchar *uri;

		uri = g_file_get_uri (file);

		g_critical ("File '%s' is not being currently processed, "
		            "so the parent URN cannot be retrieved.", uri);
		g_free (uri);

		return NULL;
	} else {
		UpdateProcessingTaskContext *ctxt;

		/* We are only storing the URN in the created/updated tasks */
		ctxt = tracker_processing_task_get_context (task);
		if (!ctxt) {
			gchar *uri;

			uri = g_file_get_uri (file);
			g_critical ("File '%s' is being processed, but not as a "
			            "CREATED/UPDATED task, so cannot get parent "
			            "URN",
			            uri);
			g_free (uri);
			return NULL;
		}

		return ctxt->parent_urn;
	}
}

void
tracker_miner_fs_force_recheck (TrackerMinerFS *fs)
{
	GList *directories;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));

	g_message ("Forcing re-check on all index directories");

	directories = g_list_copy (fs->private->config_directories);
	g_list_foreach (directories, (GFunc) directory_data_ref, NULL);

	fs->private->directories = g_list_concat (fs->private->directories, directories);

	crawl_directories_start (fs);
}

/**
 * tracker_miner_fs_set_mtime_checking:
 * @fs: a #TrackerMinerFS
 * @mtime_checking: a #gboolean
 *
 * Tells the miner-fs that during the crawling phase, directory mtime
 * checks should or shouldn't be performed against the database to
 * make sure we have the most up to date version of the file being
 * checked at the time. Setting this to #FALSE can dramatically
 * improve the start up the crawling of the @fs.
 *
 * The down side is that using this consistently means that some files
 * on the disk may be out of date with files in the database.
 *
 * The main purpose of this function is for systems where a @fs is
 * running the entire time and where it is very unlikely that a file
 * could be changed outside between startup and shutdown of the
 * process using this API.
 *
 * The default if not set directly is that @mtime_checking is #TRUE.
 *
 * Since: 0.10
 **/
void
tracker_miner_fs_set_mtime_checking (TrackerMinerFS *fs,
                                     gboolean        mtime_checking)
{
	g_return_if_fail (TRACKER_IS_MINER_FS (fs));

	fs->private->mtime_checking = mtime_checking;
}

/**
 * tracker_miner_fs_get_mtime_checking:
 * @fs: a #TrackerMinerFS
 *
 * Returns: #TRUE if mtime checks for directories against the database
 * are done when @fs crawls the file system, otherwise #FALSE.
 *
 * Since: 0.10
 **/
gboolean
tracker_miner_fs_get_mtime_checking (TrackerMinerFS *fs)
{
	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), FALSE);

	return fs->private->mtime_checking;
}

void
tracker_miner_fs_set_initial_crawling (TrackerMinerFS *fs,
                                       gboolean        do_initial_crawling)
{
	g_return_if_fail (TRACKER_IS_MINER_FS (fs));

	fs->private->initial_crawling = do_initial_crawling;
}

gboolean
tracker_miner_fs_get_initial_crawling (TrackerMinerFS *fs)
{
	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), FALSE);

	return fs->private->initial_crawling;
}

/**
 * tracker_miner_fs_has_items_to_process:
 * @fs: a #TrackerMinerFS
 *
 * Returns: #TRUE if there are items to process in the internal
 * queues, otherwise #FALSE.
 *
 * Since: 0.10
 **/
gboolean
tracker_miner_fs_has_items_to_process (TrackerMinerFS *fs)
{
	g_return_val_if_fail (TRACKER_IS_MINER_FS (fs), FALSE);

	if (g_queue_get_length (fs->private->items_deleted) > 0 ||
	    g_queue_get_length (fs->private->items_created) > 0 ||
	    g_queue_get_length (fs->private->items_updated) > 0 ||
	    g_queue_get_length (fs->private->items_moved) > 0) {
		return TRUE;
	}

	return FALSE;
}

/**
 * tracker_miner_fs_add_directory_without_parent:
 * @fs: a #TrackerMinerFS
 * @file: a #GFile
 *
 * Tells the miner-fs that the given #GFile corresponds to a
 * directory which was created in the store without a specific
 * parent object. In this case, when regenerating internal
 * caches, an extra query will be done so that these elements
 * are taken into account.
 *
 * Since: 0.10
 **/
void
tracker_miner_fs_add_directory_without_parent (TrackerMinerFS *fs,
                                               GFile          *file)
{
	GFile *parent;
	GList *l;

	g_return_if_fail (TRACKER_IS_MINER_FS (fs));
	g_return_if_fail (G_IS_FILE (file));

	/* Get parent of the input file, IF ANY! */
	parent = g_file_get_parent (file);
	if (!parent) {
		return;
	}

	for (l = fs->private->dirs_without_parent;
	     l;
	     l = g_list_next (l)) {
		if (g_file_equal (l->data, parent)) {
			/* If parent already in the list, return */
			g_object_unref (parent);
			return;
		}
	}

	/* We add the parent of the input file */
	fs->private->dirs_without_parent = g_list_prepend (fs->private->dirs_without_parent,
	                                                   parent);
}

/* Returns TRUE if the given GFile is actually the REAL parent
 * of a GFile without parent notified before */
static gboolean
miner_fs_has_children_without_parent (TrackerMinerFS *fs,
                                      GFile          *file)
{
	GList *l;

	for (l = fs->private->dirs_without_parent;
	     l;
	     l = g_list_next (l)) {
		if (g_file_equal (l->data, file)) {
			/* If already found, return */
			return TRUE;
		}
	}
	return FALSE;
}

#ifdef EVENT_QUEUE_ENABLE_TRACE

static void
miner_fs_trace_queue_with_files (TrackerMinerFS *fs,
                                 const gchar    *queue_name,
                                 GQueue         *queue)
{
	GList *li;

	trace_eq ("(%s) Queue '%s' has %u elements:",
	          G_OBJECT_TYPE_NAME (fs),
	          queue_name,
	          queue->length);

	for (li = queue->head; li; li = g_list_next (li)) {
		gchar *uri;

		uri = g_file_get_uri (li->data);
		trace_eq ("(%s)     Queue '%s' has '%s'",
		          G_OBJECT_TYPE_NAME (fs),
		          queue_name,
		          uri);
		g_free (uri);
	}
}

static void
miner_fs_trace_queue_with_data (TrackerMinerFS *fs,
                                const gchar    *queue_name,
                                GQueue         *queue)
{
	GList *li;

	trace_eq ("(%s) Queue '%s' has %u elements:",
	          G_OBJECT_TYPE_NAME (fs),
	          queue_name,
	          queue->length);

	for (li = queue->head; li; li = g_list_next (li)) {
		ItemMovedData *data = li->data;
		gchar *source_uri;
		gchar *dest_uri;

		source_uri = g_file_get_uri (data->source_file);
		dest_uri = g_file_get_uri (data->file);
		trace_eq ("(%s)     Queue '%s' has '%s->%s'",
		          G_OBJECT_TYPE_NAME (fs),
		          queue_name,
		          source_uri,
		          dest_uri);
		g_free (source_uri);
		g_free (dest_uri);
	}
}

static gboolean
miner_fs_queues_status_trace_timeout_cb (gpointer data)
{
	TrackerMinerFS *fs = data;

	trace_eq ("(%s) ------------", G_OBJECT_TYPE_NAME (fs));
	miner_fs_trace_queue_with_files (fs, "CREATED", fs->private->items_created);
	miner_fs_trace_queue_with_files (fs, "UPDATED", fs->private->items_updated);
	miner_fs_trace_queue_with_files (fs, "DELETED", fs->private->items_deleted);
	miner_fs_trace_queue_with_data  (fs, "MOVED",   fs->private->items_moved);

	return TRUE;
}

#endif /* EVENT_QUEUE_ENABLE_TRACE */
