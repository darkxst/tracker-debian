/* Copyright (C) 2006, Mr Jamie McCracken (jamiemcc@gnome.org)
 * Copyright (C) 2008, Nokia

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

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#if defined(__linux__)
#include <linux/sched.h>
#endif
#include <sched.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n.h>

#include <libtracker-common/tracker-config.h>
#include <libtracker-common/tracker-ioprio.h>
#include <libtracker-common/tracker-log.h>
#include <libtracker-common/tracker-ontology.h>
#include <libtracker-common/tracker-module-config.h>
#include <libtracker-common/tracker-file-utils.h>
#include <libtracker-common/tracker-thumbnailer.h>

#include <libtracker-db/tracker-db-manager.h>
#include <libtracker-db/tracker-db-index-manager.h>

#include <libtracker-data/tracker-data-update.h>
#include <libtracker-data/tracker-turtle.h>
#include <libtracker-data/tracker-data-manager.h>

#include "tracker-dbus.h"
#include "tracker-indexer.h"
#include "tracker-push.h"
#include "tracker-indexer-glue.h"

#define ABOUT								  \
	"Tracker " PACKAGE_VERSION "\n"

#define LICENSE								  \
	"This program is free software and comes without any warranty.\n" \
	"It is licensed under version 2 or later of the General Public "  \
	"License which can be viewed at:\n"				  \
	"\n"								  \
	"  http://www.gnu.org/licenses/gpl.txt\n"

#define QUIT_TIMEOUT 300 /* 5 minutes worth of seconds */

static GMainLoop    *main_loop;
static guint	     quit_timeout_id;

static gboolean      version;
static gint	     verbosity = -1;
static gboolean      process_all;
static gboolean      run_forever;
static gchar       **modules;

static GOptionEntry  entries[] = {
	{ "version", 'V', 0,
	  G_OPTION_ARG_NONE, &version,
	  N_("Displays version information"),
	  NULL },
	{ "verbosity", 'v', 0,
	  G_OPTION_ARG_INT, &verbosity,
	  N_("Logging, 0 = errors only, "
	     "1 = minimal, 2 = detailed and 3 = debug (default = 0)"),
	  NULL },
	{ "process-all", 'p', 0,
	  G_OPTION_ARG_NONE, &process_all,
	  N_("Index data from all enabled modules"),
	  NULL },
	{ "run-forever", 'f', 0,
	  G_OPTION_ARG_NONE, &run_forever,
	  N_("Run forever, only interesting for debugging purposes"),
	  NULL },
        { "modules", 'm', 0,
          G_OPTION_ARG_STRING_ARRAY, &modules,
          N_("Modules to be used when processing data"),
          NULL },

	{ NULL }
};

static void
sanity_check_option_values (TrackerConfig *config)
{
	g_message ("General options:");
	g_message ("  Verbosity  ............................  %d",
		   tracker_config_get_verbosity (config));
	g_message ("  Low memory mode  ......................  %s",
		   tracker_config_get_low_memory_mode (config) ? "yes" : "no");

	g_message ("Indexer options:");
	g_message ("  Throttle level  .......................  %d",
		   tracker_config_get_throttle (config));
	g_message ("  File content indexing enabled  ........  %s",
		   tracker_config_get_enable_content_indexing (config) ? "yes" : "no");
	g_message ("  Thumbnail indexing enabled  ...........  %s",
		   tracker_config_get_enable_thumbnails (config) ? "yes" : "no");
	g_message ("  Indexer language code  ................  %s",
		   tracker_config_get_language (config));
	g_message ("  Stemmer enabled  ......................  %s",
		   tracker_config_get_enable_stemmer (config) ? "yes" : "no");
	g_message ("  Fast merges enabled  ..................  %s",
		   tracker_config_get_fast_merges (config) ? "yes" : "no");
	g_message ("  Disable indexing on battery  ..........  %s (initially = %s)",
		   tracker_config_get_disable_indexing_on_battery (config) ? "yes" : "no",
		   tracker_config_get_disable_indexing_on_battery_init (config) ? "yes" : "no");

	if (tracker_config_get_low_disk_space_limit (config) == -1) {
		g_message ("  Low disk space limit  .................  Disabled");
	} else {
		g_message ("  Low disk space limit  .................  %d%%",
			   tracker_config_get_low_disk_space_limit (config));
	}

	g_message ("  Minimum index word length  ............  %d",
		   tracker_config_get_min_word_length (config));
	g_message ("  Maximum index word length  ............  %d",
		   tracker_config_get_max_word_length (config));
	g_message ("  Maximum text to index  ................  %d",
		   tracker_config_get_max_text_to_index (config));
	g_message ("  Maximum words to index  ...............  %d",
		   tracker_config_get_max_words_to_index (config));
	g_message ("  Maximum bucket count  .................  %d",
		   tracker_config_get_max_bucket_count (config));
	g_message ("  Minimum bucket count  .................  %d",
		   tracker_config_get_min_bucket_count (config));
}

static void
signal_handler (int signo)
{
	static gboolean in_loop = FALSE;

	/* Die if we get re-entrant signals handler calls */
	if (in_loop) {
		_exit (EXIT_FAILURE);
	}

	switch (signo) {
	case SIGTERM:
	case SIGINT:
		in_loop = TRUE;
		g_main_loop_quit (main_loop);

	default:
		if (g_strsignal (signo)) {
			g_print ("\n");
			g_print ("Received signal:%d->'%s'",
                                 signo,
                                 g_strsignal (signo));
		}
		break;
	}
}

static void
initialize_signal_handler (void)
{
#ifndef G_OS_WIN32
	struct sigaction act;
	sigset_t	 empty_mask;

	sigemptyset (&empty_mask);
	act.sa_handler = signal_handler;
	act.sa_mask    = empty_mask;
	act.sa_flags   = 0;

	sigaction (SIGTERM, &act, NULL);
	sigaction (SIGINT,  &act, NULL);
	sigaction (SIGHUP,  &act, NULL);
#endif /* G_OS_WIN32 */
}

static void
initialize_priority (void)
{
	/* Set disk IO priority and scheduling */
	tracker_ioprio_init ();

	/* Set process priority:
	 * The nice() function uses attribute "warn_unused_result" and
	 * so complains if we do not check its returned value. But it
	 * seems that since glibc 2.2.4, nice() can return -1 on a
	 * successful call so we have to check value of errno too.
	 * Stupid... 
	 */
	g_message ("Setting process priority");

	if (nice (19) == -1) {
		const gchar *str = g_strerror (errno);

		g_message ("Couldn't set nice value to 19, %s",
			   str ? str : "no error given");
	}
}

static gboolean
quit_timeout_cb (gpointer user_data)
{
	TrackerIndexer *indexer;

	indexer = TRACKER_INDEXER (user_data);

	if (tracker_indexer_get_stoppable (indexer)) {
		g_message ("Indexer is still not running after %d seconds, quitting...",
			   QUIT_TIMEOUT);
		g_main_loop_quit (main_loop);
		quit_timeout_id = 0;
	} else {
		g_message ("Indexer is now running, staying alive until finished...");
	}

	return FALSE;
}

static void
indexer_finished_cb (TrackerIndexer *indexer,
		     gdouble	     seconds_elapsed,
		     guint           items_processed,
		     guint	     items_indexed,
		     gboolean	     interrupted,
		     gpointer	     user_data)
{
	g_message ("Finished indexing sent items");

	if (interrupted && !run_forever) {
		g_message ("Indexer was told to shutdown");
		g_main_loop_quit (main_loop);
		return;
	}

	if (quit_timeout_id) {
		g_message ("Cancelling previous quit timeout");
		g_source_remove (quit_timeout_id);
	}

	if (!run_forever) {
		g_message ("Waiting another %d seconds for more items before quitting...",
			   QUIT_TIMEOUT);

		quit_timeout_id = g_timeout_add_seconds_full (G_PRIORITY_DEFAULT,
							      QUIT_TIMEOUT,
							      quit_timeout_cb,
							      g_object_ref (indexer),
							      (GDestroyNotify) g_object_unref);
	}
}

static void
daemon_availability_changed_cb (const gchar *name,
                                gboolean     available,
                                gpointer     user_data)
{
        if (!available) {
                tracker_indexer_stop (TRACKER_INDEXER (user_data));
        }
}

gint
main (gint argc, gchar *argv[])
{
	TrackerConfig *config;
	TrackerLanguage *language;
	TrackerIndexer *indexer;
	TrackerDBManagerFlags flags = TRACKER_DB_MANAGER_FORCE_NO_REINDEX;
	GOptionContext *context;
	GError *error = NULL;
	gchar *filename;
	TrackerDBIndex *file_index;
	TrackerDBIndex *email_index;

	g_type_init ();

	if (!g_thread_supported ()) {
		g_thread_init (NULL);
	}

	setlocale (LC_ALL, "");

	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	/* Set timezone info */
	tzset ();

	/* Translators: this messagge will apper immediately after the
	 * usage string - Usage: COMMAND <THIS_MESSAGE>
	 */
	context = g_option_context_new (_("- start the tracker indexer"));

	g_option_context_add_main_entries (context, entries, NULL);
	g_option_context_parse (context, &argc, &argv, &error);
	g_option_context_free (context);

        if (version) {
                g_print ("\n" ABOUT "\n" LICENSE "\n");
                return EXIT_SUCCESS;
        }

	g_print ("Initializing tracker-indexer...\n");

	initialize_signal_handler ();

	/* Check XDG spec locations XDG_DATA_HOME _MUST_ be writable. */
	if (!tracker_env_check_xdg_dirs ()) {
		return EXIT_FAILURE;
	}

	/* This makes sure we don't steal all the system's resources */
	initialize_priority ();

	/* Initialize logging */
	config = tracker_config_new ();
	language = tracker_language_new (config);

	if (verbosity > -1) {
		tracker_config_set_verbosity (config, verbosity);
	}
	/* Make sure we initialize DBus, this shows we are started
	 * successfully when called upon from the daemon.
	 */
	if (!tracker_dbus_init ()) {
		return EXIT_FAILURE;
	}

	filename = g_build_filename (g_get_user_data_dir (),
				     "tracker",
				     "tracker-indexer.log",
				     NULL);

	tracker_log_init (filename, tracker_config_get_verbosity (config));
	g_print ("Starting log:\n  File:'%s'\n", filename);
	g_free (filename);

	sanity_check_option_values (config);

	/* Initialize database manager */
	if (tracker_config_get_low_memory_mode (config)) {
		flags |= TRACKER_DB_MANAGER_LOW_MEMORY_MODE;
	}

	if (!tracker_db_manager_init (flags, NULL, FALSE, NULL)) {
		return EXIT_FAILURE;
        }

	if (!tracker_db_index_manager_init (0,
					    tracker_config_get_min_bucket_count (config),
					    tracker_config_get_max_bucket_count (config))) {
		return EXIT_FAILURE;
	}

	tracker_module_config_init ();

	/* Set IO priority */
	tracker_ioprio_init ();

	/* nice() uses attribute "warn_unused_result" and so complains
	 * if we do not check its returned value. But it seems that
	 * since glibc 2.2.4, nice() can return -1 on a successful
	 * call so we have to check value of errno too. Stupid...
	 */
	if (nice (19) == -1 && errno) {
		const gchar *str;

		str = g_strerror (errno);
		g_message ("Couldn't set nice value to 19, %s",
			   str ? str : "no error given");
	}

	indexer = tracker_indexer_new ();

	/* Make Tracker available for introspection */
	if (!tracker_dbus_register_object (G_OBJECT (indexer),
                                           &dbus_glib_tracker_indexer_object_info,
                                           TRACKER_INDEXER_PATH)) {
		return EXIT_FAILURE;
	}

        /* Listen for changes in trackerd availability */
        tracker_dbus_add_name_monitor (TRACKER_DAEMON_SERVICE,
                                       daemon_availability_changed_cb,
                                       indexer, NULL);

	/* Create the indexer and run the main loop */
	g_signal_connect (indexer, "finished",
			  G_CALLBACK (indexer_finished_cb),
			  NULL);

	/* Set up connections to the thumbnailer if supported */
	tracker_thumbnailer_init (config);

	if (process_all) {
		/* Tell the indexer to process all configured modules */
		tracker_indexer_process_all (indexer);
	} else if (modules) {
                tracker_indexer_process_modules (indexer, modules);
        }

	file_index = tracker_db_index_manager_get_index (TRACKER_DB_INDEX_FILE);
	email_index = tracker_db_index_manager_get_index (TRACKER_DB_INDEX_EMAIL);

	tracker_data_manager_init (config, language, file_index, email_index);

	tracker_push_init (config, indexer);

	tracker_turtle_init ();

	g_message ("Starting...");

	main_loop = g_main_loop_new (NULL, FALSE);
	g_main_loop_run (main_loop);

	g_message ("Shutdown started");

	tracker_turtle_shutdown ();

	if (quit_timeout_id) {
		g_source_remove (quit_timeout_id);
	}

	g_main_loop_unref (main_loop);
	g_object_unref (indexer);

	g_object_unref (config);
	g_object_unref (language);

	tracker_data_manager_shutdown ();

	tracker_push_shutdown ();

	tracker_thumbnailer_shutdown ();
	tracker_dbus_shutdown ();
	tracker_db_index_manager_shutdown ();
	tracker_db_manager_shutdown ();
	tracker_module_config_shutdown ();
	tracker_log_shutdown ();

	g_print ("\nOK\n\n");

	return EXIT_SUCCESS;
}
