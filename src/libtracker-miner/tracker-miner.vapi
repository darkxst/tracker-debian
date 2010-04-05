/*
 * Copyright (C) 2010, Adrien Bustany (abustany@gnome.org)
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

[CCode (cprefix = "Tracker", lower_case_cprefix = "tracker_")]
namespace Tracker {
	[CCode (cheader_filename = "libtracker-miner/tracker-miner-object.h")]
	public class Miner : GLib.Object {
		public async void commit (GLib.Cancellable? cancellable = null) throws GLib.Error;
		public static GLib.Quark error_quark ();
		public async void execute_batch_update (string sparql, GLib.Cancellable? cancellable = null) throws GLib.Error;
		public async unowned GLib.PtrArray execute_sparql (string sparql, GLib.Cancellable? cancellable = null) throws GLib.Error;
		public async void execute_update (string sparql, GLib.Cancellable? cancellable = null) throws GLib.Error;
		public void ignore_next_update (string[] urls);
		public bool is_started ();
		public int pause (string reason) throws GLib.Error;
		public virtual void paused ();
		public bool resume (int cookie) throws GLib.Error;
		public virtual void resumed ();
		public void start ();
		public virtual void started ();
		public void stop ();
		public virtual void stopped ();
		public signal void error (GLib.Error e);
	}
	[CCode (ref_function = "tracker_miner_fs_ref", unref_function = "tracker_miner_fs_unref", cheader_filename = "libtracker-miner/tracker-miner-fs.h")]
	public class MinerFS {
		public virtual bool check_directory (GLib.File directory);
		public virtual bool check_directory_contents (GLib.File directory, GLib.List<GLib.File> children);
		public virtual bool check_file (GLib.File file);
		public void directory_add (GLib.File file, bool recurse);
		public bool directory_remove (GLib.File file);
		public void file_add (GLib.File file);
		public void file_notify (GLib.File file, GLib.Error error);
		public unowned string get_parent_urn (GLib.File file);
		public double get_throttle ();
		public unowned string get_urn (GLib.File file);
		public virtual bool monitor_directory (GLib.File directory);
		public void set_throttle (double throttle);
		public signal void finished (double elapsed, uint directories_found, uint directories_ignored, uint files_found, uint files_ignored);
	}
	[CCode (cheader_filename = "libtracker-miner/tracker-miner-manager.h")]
	public class MinerManager {
		[CCode (has_construct_function = false)]
		public MinerManager ();
		public unowned GLib.SList get_available ();
		public unowned string get_description (string miner);
		public unowned string get_display_name (string miner);
		public unowned GLib.SList get_running ();
		public bool get_status (string miner, string status, double progress);
		public bool ignore_next_update (string miner, string urls);
		public bool is_active (string miner);
		public bool is_paused (string miner, string[] applications, string[] reasons);
		public bool pause (string miner, string reason, uint32 cookie);
		public bool resume (string miner, uint32 cookie);
		public virtual void miner_activated (string miner_name);
		public virtual void miner_deactivated (string miner_name);
		public virtual void miner_paused (string miner_name);
		public virtual void miner_resumed (string miner_name);
		public virtual void miner_progress (string miner_name, string status, double progress);
	}
	[CCode (cheader_filename = "libtracker-miner/tracker-miner-web.h")]
	public class MinerWeb : Tracker.Miner {
		public virtual void associate (GLib.HashTable association_data) throws Tracker.MinerWebError;
		public virtual void authenticate () throws Tracker.MinerWebError;
		public virtual void dissociate () throws Tracker.MinerWebError;
		public static GLib.Quark error_quark ();
		public virtual GLib.HashTable get_association_data () throws Tracker.MinerWebError;
	}
	[CCode (cheader_filename = "libtracker-miner/tracker-password-provider.h")]
	public interface PasswordProvider : GLib.Object {
		public void forget_password (string service) throws GLib.Error;
		public static unowned Tracker.PasswordProvider @get ();
		public unowned string get_name ();
		public string get_password (string service, out string username) throws GLib.Error;
		public void store_password (string service, string description, string username, string password) throws GLib.Error;
	}
	[CCode (cprefix = "TRACKER_MINER_WEB_ERROR_", cheader_filename = "libtracker-miner/tracker-miner-web.h")]
	public errordomain MinerWebError {
		WRONG_CREDENTIALS,
		TOKEN_EXPIRED,
		NO_CREDENTIALS,
		KEYRING,
		SERVICE,
		TRACKER,
	}
	[CCode (cprefix = "TRACKER_PASSWORD_PROVIDER_ERROR_", cheader_filename = "libtracker-miner/tracker-password-provider.h")]
	public errordomain PasswordProviderError {
		SERVICE,
		NOTFOUND,
	}
	[CCode (cheader_filename = "libtracker-miner/tracker-miner-object.h")]
	public const string MINER_WEB_DBUS_INTERFACE;
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static bool thumbnailer_cleanup (string uri_prefix);
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static bool thumbnailer_init ();
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static bool thumbnailer_move_add (string from_uri, string mime_type, string to_uri);
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static bool thumbnailer_remove_add (string uri, string mime_type);
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static void thumbnailer_send ();
	[CCode (cheader_filename = "libtracker-miner/tracker-thumbnailer.h")]
	public static void thumbnailer_shutdown ();
}
