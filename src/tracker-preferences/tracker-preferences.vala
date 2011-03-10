/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009, Nokia
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
 *
 * Author: Philip Van Hoof <philip@codeminded.be>
 */

using Gtk;
using GLib;
using Tracker;

public static Config config = null;

public const string HOME_STRING = "$HOME";

public static Window window;
public static CheckButton checkbutton_enable_index_on_battery_first_time;
public static CheckButton checkbutton_enable_index_on_battery;
public static SpinButton spinbutton_delay;
public static CheckButton checkbutton_enable_monitoring;
public static CheckButton checkbutton_index_removable_media;
public static CheckButton checkbutton_index_optical_discs;
public static Scale hscale_disk_space_limit;
public static Scale hscale_throttle;
public static Scale hscale_drop_device_threshold;
public static ListStore liststore_index_recursively;
public static ListStore liststore_index_single;
public static ListStore liststore_ignored_directories;
public static ListStore liststore_ignored_files;
public static ListStore liststore_gnored_directories_with_content;
public static TreeView treeview_index_recursively;
public static TreeView treeview_index_single;
public static TreeView treeview_ignored_directories;
public static TreeView treeview_ignored_directories_with_content;
public static TreeView treeview_ignored_files;
public static ToggleButton togglebutton_home;
public static Notebook notebook;
public static RadioButton radiobutton_display_never;
public static RadioButton radiobutton_display_active;
public static RadioButton radiobutton_display_always;

public static void spinbutton_delay_value_changed_cb (SpinButton source) {
	config.initial_sleep = source.get_value_as_int ();
}

public static void checkbutton_enable_monitoring_toggled_cb (CheckButton source) {
	config.enable_monitors = source.active;
}

public static void checkbutton_enable_index_on_battery_toggled_cb (CheckButton source) {
	config.index_on_battery = source.active;
	checkbutton_enable_index_on_battery_first_time.set_sensitive (!source.active);
}

public static void checkbutton_enable_index_on_battery_first_time_toggled_cb (CheckButton source) {
	config.index_on_battery_first_time = source.active;
}

public static void checkbutton_index_removable_media_toggled_cb (CheckButton source) {
	config.index_removable_devices = source.active;
	checkbutton_index_optical_discs.set_sensitive (source.active);
}

public static void checkbutton_index_optical_discs_toggled_cb (CheckButton source) {
	config.index_optical_discs = source.active;
}

public static string hscale_disk_space_limit_format_value_cb (Scale source, double value) {
	if (((int) value) == -1) {
		return _("Disabled");
	}

	return _("%d%%").printf ((int) value);
}

public static string hscale_throttle_format_value_cb (Scale source, double value) {
	return _("%d/20").printf ((int) value);
}

public static string hscale_drop_device_threshold_format_value_cb (Scale source, double value) {
	if (((int) value) == 0) {
		return _("Disabled");
	}

	return _("%d").printf ((int) value);
}

public static void add_freevalue (ListStore model) {
	Dialog dialog;
	Entry entry;
	Container content_area;

	dialog = new Dialog.with_buttons (_("Enter value"),
	                                  window,
	                                  DialogFlags.DESTROY_WITH_PARENT,
	                                  Stock.CANCEL, ResponseType.CANCEL,
	                                  Stock.OK, ResponseType.ACCEPT);

	dialog.set_default_response(ResponseType.ACCEPT);
	content_area = (Container) dialog.get_content_area ();
	entry = new Entry ();
	entry.set_activates_default (true);
	entry.show ();
	content_area.add (entry);

	if (dialog.run () == ResponseType.ACCEPT) {
		string text = entry.get_text ();

		if (text != null && text != "") {
			TreeIter iter;
			model.append (out iter);
			var v = Value (typeof (string));
			v.set_string (text);
			model.set_value (iter, 0, v);
		}
	}

	dialog.destroy ();
}


public static void add_dir (ListStore model)
{
	FileChooserDialog dialog = new FileChooserDialog (_("Select directory"), window,
	                                              FileChooserAction.SELECT_FOLDER,
	                                              Stock.CANCEL, ResponseType.CANCEL,
	                                              Stock.OK, ResponseType.ACCEPT);

	if (dialog.run () == ResponseType.ACCEPT) {
		TreeIter iter;
		File dir;

		dir = dialog.get_file ();

		model.append (out iter);
		var v = Value (typeof (string));
		v.set_string (dir.get_path());
		model.set_value (iter, 0, v);
	}

	dialog.destroy ();
}

public static void del_dir (TreeView view)
{
	List<TreePath> list;
	ListStore store;
	TreeModel model;

	TreeSelection selection = view.get_selection ();
	list= selection.get_selected_rows (out model);

	store = (ListStore) model;

	foreach (TreePath path in list) {
		TreeIter iter;
		if (model.get_iter (out iter, path)) {
			store.remove (iter);
		}
	}
}

public static void button_index_recursively_add_clicked_cb (Button source) {
	add_dir (liststore_index_recursively);
}

public static void button_index_recursively_remove_clicked_cb (Button source) {
	del_dir (treeview_index_recursively);
}

public static void button_index_single_remove_clicked_cb (Button source) {
	del_dir (treeview_index_single);
}

public static void button_index_single_add_clicked_cb (Button source) {
	add_dir (liststore_index_single);
}

public static void button_ignored_directories_globs_add_clicked_cb (Button source) {
	add_freevalue (liststore_ignored_directories);
}

public static void button_ignored_directories_add_clicked_cb (Button source) {
	add_dir (liststore_ignored_directories);
}

public static void button_ignored_directories_remove_clicked_cb (Button source) {
	del_dir (treeview_ignored_directories);
}

public static void button_ignored_directories_with_content_add_clicked_cb (Button source) {
	add_freevalue (liststore_gnored_directories_with_content);
}

public static void button_ignored_directories_with_content_remove_clicked_cb (Button source) {
	del_dir (treeview_ignored_directories_with_content);
}

public static void button_ignored_files_add_clicked_cb (Button source) {
	add_freevalue (liststore_ignored_files);
}

public static void button_ignored_files_remove_clicked_cb (Button source) {
	del_dir (treeview_ignored_files);
}

public static SList<string> model_to_slist (ListStore model)
{
	bool valid;
	SList<string> list = new SList<string>();
	TreeIter iter;

	valid = model.get_iter_first (out iter);
	while (valid) {
		Value value;
		model.get_value (iter, 0, out value);
		list.append (value.get_string ());
		valid = model.iter_next (ref iter);
	}

	return list;
}

public bool model_contains (TreeModel model, string needle) {
	bool valid;
	TreeIter iter;

	valid = model.get_iter_first (out iter);
	while (valid) {
		Value value;
		model.get_value (iter, 0, out value);
		if (value.get_string () == needle) {
			return true;
		}
		valid = model.iter_next (ref iter);
	}
	return false;
}

public static void button_apply_clicked_cb (Button source) {

	config.index_single_directories = model_to_slist (liststore_index_single);
	config.ignored_directories = model_to_slist (liststore_ignored_directories);
	config.ignored_files = model_to_slist (liststore_ignored_files);
	config.ignored_directories_with_content = model_to_slist (liststore_gnored_directories_with_content);
	config.index_recursive_directories = model_to_slist (liststore_index_recursively);

	config.low_disk_space_limit = (int) hscale_disk_space_limit.get_value ();
	config.throttle = (int) hscale_throttle.get_value ();
	config.removable_days_threshold = (int) hscale_drop_device_threshold.get_value ();

	config.save ();

	/* TODO: restart the Application and Files miner (no idea how to cleanly do this atm) */
}

public static void button_close_clicked_cb  (Button source) {
	Gtk.main_quit ();
}

public static void togglebutton_home_toggled_cb (ToggleButton source) {
	if (source.active && !model_contains (liststore_index_recursively, HOME_STRING)) {
		TreeIter iter;
		liststore_index_recursively.append (out iter);
		var v = Value (typeof (string));
		v.set_string (HOME_STRING);
		liststore_index_recursively.set_value (iter, 0, v);
	}

	if (!source.active && model_contains (liststore_index_recursively, HOME_STRING)) {
		bool valid;
		TreeIter iter;

		valid = liststore_index_recursively.get_iter_first (out iter);
		while (valid) {
			Value value;
			liststore_index_recursively.get_value (iter, 0, out value);
			if (value.get_string () == HOME_STRING) {
				liststore_index_recursively.remove (iter);
				valid = liststore_index_recursively.get_iter_first (out iter);
			} else {
				valid = liststore_index_recursively.iter_next (ref iter);
			}
		}
	}
}

static void
fill_in_model (ListStore model, SList<string> list)
{
	int position = 0;
	foreach (string str in list) {
		try {
			model.insert_with_values (null,
			                          position++,
			                          0,
			                          Filename.to_utf8 (str,
			                                            -1,
			                                            null,
			                                            null));
		} catch (GLib.ConvertError e) {
			print ("%s", e.message);
		}
	}
}

static void
setup_standard_treeview (TreeView view, string title)
{
	TreeViewColumn column = new TreeViewColumn.with_attributes (title,
	                                                            new CellRendererText (),
	                                                            "text", 0);
	view.append_column (column);
}

static int main (string[] args) {
	Gtk.init (ref args);

	Intl.bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	Intl.bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	Intl.textdomain (GETTEXT_PACKAGE);

	try {
		config = new Config.with_domain ("tracker-miner-fs");

		var builder = new Builder ();
		builder.add_from_file (TRACKER_DATADIR + Path.DIR_SEPARATOR_S + "tracker-preferences.ui");

		window = builder.get_object ("tracker-preferences") as Window;
		checkbutton_enable_index_on_battery = builder.get_object ("checkbutton_enable_index_on_battery") as CheckButton;
		checkbutton_enable_index_on_battery_first_time = builder.get_object ("checkbutton_enable_index_on_battery_first_time") as CheckButton;
		checkbutton_enable_index_on_battery.active = config.index_on_battery;
		checkbutton_enable_index_on_battery_first_time.set_sensitive (!checkbutton_enable_index_on_battery.active);
		checkbutton_enable_index_on_battery_first_time.active = config.index_on_battery_first_time;
		spinbutton_delay = builder.get_object ("spinbutton_delay") as SpinButton;
		spinbutton_delay.set_increments (1, 1);
		spinbutton_delay.value = (double) config.initial_sleep;
		checkbutton_enable_monitoring = builder.get_object ("checkbutton_enable_monitoring") as CheckButton;
		checkbutton_enable_monitoring.active = config.enable_monitors;
		checkbutton_index_removable_media = builder.get_object ("checkbutton_index_removable_media") as CheckButton;
		checkbutton_index_removable_media.active = config.index_removable_devices;
		checkbutton_index_optical_discs = builder.get_object ("checkbutton_index_optical_discs") as CheckButton;
		checkbutton_index_optical_discs.set_sensitive (checkbutton_index_removable_media.active);
		checkbutton_index_optical_discs.active = config.index_optical_discs;
		hscale_disk_space_limit = builder.get_object ("hscale_disk_space_limit") as Scale;
		hscale_disk_space_limit.set_value ((double) config.low_disk_space_limit);
		hscale_throttle = builder.get_object ("hscale_throttle") as Scale;
		hscale_throttle.set_value ((double) config.throttle);
		hscale_drop_device_threshold = builder.get_object ("hscale_drop_device_threshold") as Scale;
		hscale_drop_device_threshold.set_value ((double) config.removable_days_threshold);
		togglebutton_home = builder.get_object ("togglebutton_home") as ToggleButton;

		notebook = builder.get_object ("notebook") as Notebook;

		// We hide this page because it contains the start up
		// delay which is not necessary to display for most people.
		notebook.remove_page (0);

		treeview_index_recursively = builder.get_object ("treeview_index_recursively") as TreeView;
		treeview_index_single = builder.get_object ("treeview_index_single") as TreeView;
		treeview_ignored_directories = builder.get_object ("treeview_ignored_directories") as TreeView;
		treeview_ignored_directories_with_content = builder.get_object ("treeview_ignored_directories_with_content") as TreeView;
		treeview_ignored_files = builder.get_object ("treeview_ignored_files") as TreeView;

		setup_standard_treeview (treeview_index_recursively, _("Directory"));
		setup_standard_treeview (treeview_index_single, _("Directory"));
		setup_standard_treeview (treeview_ignored_directories, _("Directory"));
		setup_standard_treeview (treeview_ignored_directories_with_content, _("Directory"));
		setup_standard_treeview (treeview_ignored_files, _("File"));

		liststore_index_recursively = builder.get_object ("liststore_index_recursively") as ListStore;
		fill_in_model (liststore_index_recursively, config.index_recursive_directories_unfiltered);

		togglebutton_home.active = model_contains (liststore_index_recursively, HOME_STRING);

		liststore_index_single = builder.get_object ("liststore_index_single") as ListStore;
		fill_in_model (liststore_index_single, config.index_single_directories_unfiltered);

		liststore_ignored_directories = builder.get_object ("liststore_ignored_directories") as ListStore;
		fill_in_model (liststore_ignored_directories, config.ignored_directories);

		liststore_ignored_files = builder.get_object ("liststore_ignored_files") as ListStore;
		fill_in_model (liststore_ignored_files, config.ignored_files);

		liststore_gnored_directories_with_content = builder.get_object ("liststore_gnored_directories_with_content") as ListStore;
		fill_in_model (liststore_gnored_directories_with_content, config.ignored_directories_with_content);

		builder.connect_signals (null);

		window.show_all ();

		Gtk.main ();
	} catch (Error e) {
		stderr.printf ("Could not load UI: %s\n", e.message);
		return 1;
	}

	return 0;
}
