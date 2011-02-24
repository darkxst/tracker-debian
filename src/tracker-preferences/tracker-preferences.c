/* tracker-preferences.c generated by valac 0.11.4, the Vala compiler
 * generated from tracker-preferences.vala, do not modify */

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

#include <glib.h>
#include <glib-object.h>
#include <miners/fs/tracker-config.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <float.h>
#include <math.h>
#include <glib/gi18n-lib.h>
#include <gio/gio.h>
#include <config.h>
#include <stdio.h>

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define __g_list_free__gtk_tree_path_free0_0(var) ((var == NULL) ? NULL : (var = (_g_list_free__gtk_tree_path_free0_ (var), NULL)))
#define _gtk_tree_path_free0(var) ((var == NULL) ? NULL : (var = (gtk_tree_path_free (var), NULL)))
#define __g_slist_free__g_free0_0(var) ((var == NULL) ? NULL : (var = (_g_slist_free__g_free0_ (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))


extern TrackerConfig* config;
TrackerConfig* config = NULL;
extern GtkWindow* window;
GtkWindow* window = NULL;
extern GtkCheckButton* checkbutton_enable_index_on_battery_first_time;
GtkCheckButton* checkbutton_enable_index_on_battery_first_time = NULL;
extern GtkCheckButton* checkbutton_enable_index_on_battery;
GtkCheckButton* checkbutton_enable_index_on_battery = NULL;
extern GtkSpinButton* spinbutton_delay;
GtkSpinButton* spinbutton_delay = NULL;
extern GtkCheckButton* checkbutton_enable_monitoring;
GtkCheckButton* checkbutton_enable_monitoring = NULL;
extern GtkCheckButton* checkbutton_index_removable_media;
GtkCheckButton* checkbutton_index_removable_media = NULL;
extern GtkCheckButton* checkbutton_index_optical_discs;
GtkCheckButton* checkbutton_index_optical_discs = NULL;
extern GtkScale* hscale_disk_space_limit;
GtkScale* hscale_disk_space_limit = NULL;
extern GtkScale* hscale_throttle;
GtkScale* hscale_throttle = NULL;
extern GtkScale* hscale_drop_device_threshold;
GtkScale* hscale_drop_device_threshold = NULL;
extern GtkListStore* liststore_index_recursively;
GtkListStore* liststore_index_recursively = NULL;
extern GtkListStore* liststore_index_single;
GtkListStore* liststore_index_single = NULL;
extern GtkListStore* liststore_ignored_directories;
GtkListStore* liststore_ignored_directories = NULL;
extern GtkListStore* liststore_ignored_files;
GtkListStore* liststore_ignored_files = NULL;
extern GtkListStore* liststore_gnored_directories_with_content;
GtkListStore* liststore_gnored_directories_with_content = NULL;
extern GtkTreeView* treeview_index_recursively;
GtkTreeView* treeview_index_recursively = NULL;
extern GtkTreeView* treeview_index_single;
GtkTreeView* treeview_index_single = NULL;
extern GtkTreeView* treeview_ignored_directories;
GtkTreeView* treeview_ignored_directories = NULL;
extern GtkTreeView* treeview_ignored_directories_with_content;
GtkTreeView* treeview_ignored_directories_with_content = NULL;
extern GtkTreeView* treeview_ignored_files;
GtkTreeView* treeview_ignored_files = NULL;
extern GtkToggleButton* togglebutton_home;
GtkToggleButton* togglebutton_home = NULL;
extern GtkNotebook* notebook;
GtkNotebook* notebook = NULL;
extern GtkRadioButton* radiobutton_display_never;
GtkRadioButton* radiobutton_display_never = NULL;
extern GtkRadioButton* radiobutton_display_active;
GtkRadioButton* radiobutton_display_active = NULL;
extern GtkRadioButton* radiobutton_display_always;
GtkRadioButton* radiobutton_display_always = NULL;

#define HOME_STRING "$HOME"
void spinbutton_delay_value_changed_cb (GtkSpinButton* source);
void checkbutton_enable_monitoring_toggled_cb (GtkCheckButton* source);
void checkbutton_enable_index_on_battery_toggled_cb (GtkCheckButton* source);
void checkbutton_enable_index_on_battery_first_time_toggled_cb (GtkCheckButton* source);
void checkbutton_index_removable_media_toggled_cb (GtkCheckButton* source);
void checkbutton_index_optical_discs_toggled_cb (GtkCheckButton* source);
gchar* hscale_disk_space_limit_format_value_cb (GtkScale* source, gdouble value);
gchar* hscale_throttle_format_value_cb (GtkScale* source, gdouble value);
gchar* hscale_drop_device_threshold_format_value_cb (GtkScale* source, gdouble value);
void add_freevalue (GtkListStore* model);
void add_dir (GtkListStore* model);
void del_dir (GtkTreeView* view);
static void _gtk_tree_path_free0_ (gpointer var);
static void _g_list_free__gtk_tree_path_free0_ (GList* self);
void button_index_recursively_add_clicked_cb (GtkButton* source);
void button_index_recursively_remove_clicked_cb (GtkButton* source);
void button_index_single_remove_clicked_cb (GtkButton* source);
void button_index_single_add_clicked_cb (GtkButton* source);
void button_ignored_directories_globs_add_clicked_cb (GtkButton* source);
void button_ignored_directories_add_clicked_cb (GtkButton* source);
void button_ignored_directories_remove_clicked_cb (GtkButton* source);
void button_ignored_directories_with_content_add_clicked_cb (GtkButton* source);
void button_ignored_directories_with_content_remove_clicked_cb (GtkButton* source);
void button_ignored_files_add_clicked_cb (GtkButton* source);
void button_ignored_files_remove_clicked_cb (GtkButton* source);
GSList* model_to_slist (GtkListStore* model);
static void _g_free0_ (gpointer var);
static void _g_slist_free__g_free0_ (GSList* self);
gboolean model_contains (GtkTreeModel* model, const gchar* needle);
void button_apply_clicked_cb (GtkButton* source);
void button_close_clicked_cb (GtkButton* source);
void togglebutton_home_toggled_cb (GtkToggleButton* source);
void fill_in_model (GtkListStore* model, GSList* list);
void setup_standard_treeview (GtkTreeView* view, const gchar* title);
gint _vala_main (gchar** args, int args_length1);


void spinbutton_delay_value_changed_cb (GtkSpinButton* source) {
	gint _tmp0_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_spin_button_get_value_as_int (source);
	tracker_config_set_initial_sleep (config, _tmp0_);
}


void checkbutton_enable_monitoring_toggled_cb (GtkCheckButton* source) {
	gboolean _tmp0_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	tracker_config_set_enable_monitors (config, _tmp0_);
}


void checkbutton_enable_index_on_battery_toggled_cb (GtkCheckButton* source) {
	gboolean _tmp0_;
	gboolean _tmp1_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	tracker_config_set_index_on_battery (config, _tmp0_);
	_tmp1_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	gtk_widget_set_sensitive ((GtkWidget*) checkbutton_enable_index_on_battery_first_time, !_tmp1_);
}


void checkbutton_enable_index_on_battery_first_time_toggled_cb (GtkCheckButton* source) {
	gboolean _tmp0_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	tracker_config_set_index_on_battery_first_time (config, _tmp0_);
}


void checkbutton_index_removable_media_toggled_cb (GtkCheckButton* source) {
	gboolean _tmp0_;
	gboolean _tmp1_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	tracker_config_set_index_removable_devices (config, _tmp0_);
	_tmp1_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	gtk_widget_set_sensitive ((GtkWidget*) checkbutton_index_optical_discs, _tmp1_);
}


void checkbutton_index_optical_discs_toggled_cb (GtkCheckButton* source) {
	gboolean _tmp0_;
	g_return_if_fail (source != NULL);
	_tmp0_ = gtk_toggle_button_get_active ((GtkToggleButton*) source);
	tracker_config_set_index_optical_discs (config, _tmp0_);
}


gchar* hscale_disk_space_limit_format_value_cb (GtkScale* source, gdouble value) {
	gchar* result = NULL;
	const gchar* _tmp2_ = NULL;
	gchar* _tmp3_ = NULL;
	g_return_val_if_fail (source != NULL, NULL);
	if (((gint) value) == (-1)) {
		const gchar* _tmp0_ = NULL;
		gchar* _tmp1_;
		_tmp0_ = _ ("Disabled");
		_tmp1_ = g_strdup (_tmp0_);
		result = _tmp1_;
		return result;
	}
	_tmp2_ = _ ("%d%%");
	_tmp3_ = g_strdup_printf (_tmp2_, (gint) value);
	result = _tmp3_;
	return result;
}


gchar* hscale_throttle_format_value_cb (GtkScale* source, gdouble value) {
	gchar* result = NULL;
	const gchar* _tmp0_ = NULL;
	gchar* _tmp1_ = NULL;
	g_return_val_if_fail (source != NULL, NULL);
	_tmp0_ = _ ("%d/20");
	_tmp1_ = g_strdup_printf (_tmp0_, (gint) value);
	result = _tmp1_;
	return result;
}


gchar* hscale_drop_device_threshold_format_value_cb (GtkScale* source, gdouble value) {
	gchar* result = NULL;
	const gchar* _tmp2_ = NULL;
	gchar* _tmp3_ = NULL;
	g_return_val_if_fail (source != NULL, NULL);
	if (((gint) value) == 0) {
		const gchar* _tmp0_ = NULL;
		gchar* _tmp1_;
		_tmp0_ = _ ("Disabled");
		_tmp1_ = g_strdup (_tmp0_);
		result = _tmp1_;
		return result;
	}
	_tmp2_ = _ ("%d");
	_tmp3_ = g_strdup_printf (_tmp2_, (gint) value);
	result = _tmp3_;
	return result;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


void add_freevalue (GtkListStore* model) {
	GtkDialog* dialog;
	GtkEntry* entry;
	GtkContainer* content_area;
	const gchar* _tmp0_ = NULL;
	GtkDialog* _tmp1_ = NULL;
	GtkDialog* _tmp2_;
	GtkWidget* _tmp3_ = NULL;
	GtkContainer* _tmp4_;
	GtkContainer* _tmp5_;
	GtkEntry* _tmp6_ = NULL;
	GtkEntry* _tmp7_;
	gint _tmp8_;
	g_return_if_fail (model != NULL);
	dialog = NULL;
	entry = NULL;
	content_area = NULL;
	_tmp0_ = _ ("Enter value");
	_tmp1_ = (GtkDialog*) gtk_dialog_new_with_buttons (_tmp0_, window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	_tmp2_ = g_object_ref_sink (_tmp1_);
	_g_object_unref0 (dialog);
	dialog = _tmp2_;
	gtk_dialog_set_default_response (dialog, (gint) GTK_RESPONSE_ACCEPT);
	_tmp3_ = gtk_dialog_get_content_area (dialog);
	_tmp4_ = _g_object_ref0 (GTK_CONTAINER (_tmp3_));
	_tmp5_ = _tmp4_;
	_g_object_unref0 (content_area);
	content_area = _tmp5_;
	_tmp6_ = (GtkEntry*) gtk_entry_new ();
	_tmp7_ = g_object_ref_sink (_tmp6_);
	_g_object_unref0 (entry);
	entry = _tmp7_;
	gtk_entry_set_activates_default (entry, TRUE);
	gtk_widget_show ((GtkWidget*) entry);
	gtk_container_add (content_area, (GtkWidget*) entry);
	_tmp8_ = gtk_dialog_run (dialog);
	if (_tmp8_ == GTK_RESPONSE_ACCEPT) {
		const gchar* _tmp9_ = NULL;
		gchar* _tmp10_;
		gchar* text;
		gboolean _tmp11_ = FALSE;
		_tmp9_ = gtk_entry_get_text (entry);
		_tmp10_ = g_strdup (_tmp9_);
		text = _tmp10_;
		if (text != NULL) {
			_tmp11_ = g_strcmp0 (text, "") != 0;
		} else {
			_tmp11_ = FALSE;
		}
		if (_tmp11_) {
			GtkTreeIter iter = {0};
			GtkTreeIter _tmp12_ = {0};
			GValue v = {0};
			gtk_list_store_append (model, &_tmp12_);
			iter = _tmp12_;
			g_value_init (&v, G_TYPE_STRING);
			g_value_set_string (&v, text);
			gtk_list_store_set_value (model, &iter, 0, &v);
			G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
		}
		_g_free0 (text);
	}
	gtk_object_destroy ((GtkObject*) dialog);
	_g_object_unref0 (content_area);
	_g_object_unref0 (entry);
	_g_object_unref0 (dialog);
}


void add_dir (GtkListStore* model) {
	const gchar* _tmp0_ = NULL;
	GtkFileChooserDialog* _tmp1_ = NULL;
	GtkFileChooserDialog* dialog;
	gint _tmp2_;
	g_return_if_fail (model != NULL);
	_tmp0_ = _ ("Select directory");
	_tmp1_ = (GtkFileChooserDialog*) gtk_file_chooser_dialog_new (_tmp0_, window, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	dialog = g_object_ref_sink (_tmp1_);
	_tmp2_ = gtk_dialog_run ((GtkDialog*) dialog);
	if (_tmp2_ == GTK_RESPONSE_ACCEPT) {
		GtkTreeIter iter = {0};
		GFile* dir;
		GFile* _tmp3_ = NULL;
		GFile* _tmp4_;
		GFile* _tmp5_;
		GtkTreeIter _tmp6_ = {0};
		GValue v = {0};
		gchar* _tmp7_ = NULL;
		gchar* _tmp8_;
		dir = NULL;
		_tmp3_ = gtk_file_chooser_get_file ((GtkFileChooser*) dialog);
		_tmp4_ = _g_object_ref0 (_tmp3_);
		_tmp5_ = _tmp4_;
		_g_object_unref0 (dir);
		dir = _tmp5_;
		gtk_list_store_append (model, &_tmp6_);
		iter = _tmp6_;
		g_value_init (&v, G_TYPE_STRING);
		_tmp7_ = g_file_get_path (dir);
		_tmp8_ = _tmp7_;
		g_value_set_string (&v, _tmp8_);
		_g_free0 (_tmp8_);
		gtk_list_store_set_value (model, &iter, 0, &v);
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
		_g_object_unref0 (dir);
	}
	gtk_object_destroy ((GtkObject*) dialog);
	_g_object_unref0 (dialog);
}


static void _gtk_tree_path_free0_ (gpointer var) {
	(var == NULL) ? NULL : (var = (gtk_tree_path_free (var), NULL));
}


static void _g_list_free__gtk_tree_path_free0_ (GList* self) {
	g_list_foreach (self, (GFunc) _gtk_tree_path_free0_, NULL);
	g_list_free (self);
}


static gpointer _gtk_tree_path_copy0 (gpointer self) {
	return self ? gtk_tree_path_copy (self) : NULL;
}


void del_dir (GtkTreeView* view) {
	GList* list;
	GtkListStore* store;
	GtkTreeModel* model;
	GtkTreeSelection* _tmp0_ = NULL;
	GtkTreeSelection* _tmp1_;
	GtkTreeSelection* selection;
	GtkTreeModel* _tmp2_ = NULL;
	GList* _tmp3_ = NULL;
	GtkTreeModel* _tmp4_;
	GList* _tmp5_;
	GtkListStore* _tmp6_;
	GtkListStore* _tmp7_;
	g_return_if_fail (view != NULL);
	list = NULL;
	store = NULL;
	model = NULL;
	_tmp0_ = gtk_tree_view_get_selection (view);
	_tmp1_ = _g_object_ref0 (_tmp0_);
	selection = _tmp1_;
	_tmp3_ = gtk_tree_selection_get_selected_rows (selection, &_tmp2_);
	_g_object_unref0 (model);
	_tmp4_ = _g_object_ref0 (_tmp2_);
	model = _tmp4_;
	_tmp5_ = _tmp3_;
	__g_list_free__gtk_tree_path_free0_0 (list);
	list = _tmp5_;
	_tmp6_ = _g_object_ref0 (GTK_LIST_STORE (model));
	_tmp7_ = _tmp6_;
	_g_object_unref0 (store);
	store = _tmp7_;
	{
		GList* path_collection;
		GList* path_it;
		path_collection = list;
		for (path_it = path_collection; path_it != NULL; path_it = path_it->next) {
			GtkTreePath* _tmp8_;
			GtkTreePath* path;
			_tmp8_ = _gtk_tree_path_copy0 ((GtkTreePath*) path_it->data);
			path = _tmp8_;
			{
				GtkTreeIter iter = {0};
				GtkTreeIter _tmp9_ = {0};
				gboolean _tmp10_;
				_tmp10_ = gtk_tree_model_get_iter (model, &_tmp9_, path);
				iter = _tmp9_;
				if (_tmp10_) {
					gtk_list_store_remove (store, &iter);
				}
				_gtk_tree_path_free0 (path);
			}
		}
	}
	_g_object_unref0 (selection);
	_g_object_unref0 (model);
	_g_object_unref0 (store);
	__g_list_free__gtk_tree_path_free0_0 (list);
}


void button_index_recursively_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_dir (liststore_index_recursively);
}


void button_index_recursively_remove_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	del_dir (treeview_index_recursively);
}


void button_index_single_remove_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	del_dir (treeview_index_single);
}


void button_index_single_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_dir (liststore_index_single);
}


void button_ignored_directories_globs_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_freevalue (liststore_ignored_directories);
}


void button_ignored_directories_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_dir (liststore_ignored_directories);
}


void button_ignored_directories_remove_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	del_dir (treeview_ignored_directories);
}


void button_ignored_directories_with_content_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_freevalue (liststore_gnored_directories_with_content);
}


void button_ignored_directories_with_content_remove_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	del_dir (treeview_ignored_directories_with_content);
}


void button_ignored_files_add_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	add_freevalue (liststore_ignored_files);
}


void button_ignored_files_remove_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	del_dir (treeview_ignored_files);
}


static void _g_free0_ (gpointer var) {
	var = (g_free (var), NULL);
}


static void _g_slist_free__g_free0_ (GSList* self) {
	g_slist_foreach (self, (GFunc) _g_free0_, NULL);
	g_slist_free (self);
}


GSList* model_to_slist (GtkListStore* model) {
	GSList* result = NULL;
	gboolean valid = FALSE;
	GSList* list;
	GtkTreeIter iter = {0};
	GtkTreeIter _tmp0_ = {0};
	gboolean _tmp1_;
	g_return_val_if_fail (model != NULL, NULL);
	list = NULL;
	_tmp1_ = gtk_tree_model_get_iter_first ((GtkTreeModel*) model, &_tmp0_);
	iter = _tmp0_;
	valid = _tmp1_;
	while (TRUE) {
		GValue value = {0};
		GValue _tmp2_ = {0};
		const gchar* _tmp3_ = NULL;
		gchar* _tmp4_;
		gboolean _tmp5_;
		if (!valid) {
			break;
		}
		gtk_tree_model_get_value ((GtkTreeModel*) model, &iter, 0, &_tmp2_);
		G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
		value = _tmp2_;
		_tmp3_ = g_value_get_string (&value);
		_tmp4_ = g_strdup (_tmp3_);
		list = g_slist_append (list, _tmp4_);
		_tmp5_ = gtk_tree_model_iter_next ((GtkTreeModel*) model, &iter);
		valid = _tmp5_;
		G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
	}
	result = list;
	return result;
}


gboolean model_contains (GtkTreeModel* model, const gchar* needle) {
	gboolean result = FALSE;
	gboolean valid = FALSE;
	GtkTreeIter iter = {0};
	GtkTreeIter _tmp0_ = {0};
	gboolean _tmp1_;
	g_return_val_if_fail (model != NULL, FALSE);
	g_return_val_if_fail (needle != NULL, FALSE);
	_tmp1_ = gtk_tree_model_get_iter_first (model, &_tmp0_);
	iter = _tmp0_;
	valid = _tmp1_;
	while (TRUE) {
		GValue value = {0};
		GValue _tmp2_ = {0};
		const gchar* _tmp3_ = NULL;
		gboolean _tmp4_;
		if (!valid) {
			break;
		}
		gtk_tree_model_get_value (model, &iter, 0, &_tmp2_);
		G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
		value = _tmp2_;
		_tmp3_ = g_value_get_string (&value);
		if (g_strcmp0 (_tmp3_, needle) == 0) {
			result = TRUE;
			G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
			return result;
		}
		_tmp4_ = gtk_tree_model_iter_next (model, &iter);
		valid = _tmp4_;
		G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
	}
	result = FALSE;
	return result;
}


void button_apply_clicked_cb (GtkButton* source) {
	GSList* _tmp0_ = NULL;
	GSList* _tmp1_;
	GSList* _tmp2_ = NULL;
	GSList* _tmp3_;
	GSList* _tmp4_ = NULL;
	GSList* _tmp5_;
	GSList* _tmp6_ = NULL;
	GSList* _tmp7_;
	GSList* _tmp8_ = NULL;
	GSList* _tmp9_;
	gdouble _tmp10_;
	gdouble _tmp11_;
	gdouble _tmp12_;
	g_return_if_fail (source != NULL);
	_tmp0_ = model_to_slist (liststore_index_single);
	_tmp1_ = _tmp0_;
	tracker_config_set_index_single_directories (config, _tmp1_);
	__g_slist_free__g_free0_0 (_tmp1_);
	_tmp2_ = model_to_slist (liststore_ignored_directories);
	_tmp3_ = _tmp2_;
	tracker_config_set_ignored_directories (config, _tmp3_);
	__g_slist_free__g_free0_0 (_tmp3_);
	_tmp4_ = model_to_slist (liststore_ignored_files);
	_tmp5_ = _tmp4_;
	tracker_config_set_ignored_files (config, _tmp5_);
	__g_slist_free__g_free0_0 (_tmp5_);
	_tmp6_ = model_to_slist (liststore_gnored_directories_with_content);
	_tmp7_ = _tmp6_;
	tracker_config_set_ignored_directories_with_content (config, _tmp7_);
	__g_slist_free__g_free0_0 (_tmp7_);
	_tmp8_ = model_to_slist (liststore_index_recursively);
	_tmp9_ = _tmp8_;
	tracker_config_set_index_recursive_directories (config, _tmp9_);
	__g_slist_free__g_free0_0 (_tmp9_);
	_tmp10_ = gtk_range_get_value ((GtkRange*) hscale_disk_space_limit);
	tracker_config_set_low_disk_space_limit (config, (gint) _tmp10_);
	_tmp11_ = gtk_range_get_value ((GtkRange*) hscale_throttle);
	tracker_config_set_throttle (config, (gint) _tmp11_);
	_tmp12_ = gtk_range_get_value ((GtkRange*) hscale_drop_device_threshold);
	tracker_config_set_removable_days_threshold (config, (gint) _tmp12_);
	tracker_config_save (config);
}


void button_close_clicked_cb (GtkButton* source) {
	g_return_if_fail (source != NULL);
	gtk_main_quit ();
}


void togglebutton_home_toggled_cb (GtkToggleButton* source) {
	gboolean _tmp0_ = FALSE;
	gboolean _tmp1_;
	gboolean _tmp4_ = FALSE;
	gboolean _tmp5_;
	g_return_if_fail (source != NULL);
	_tmp1_ = gtk_toggle_button_get_active (source);
	if (_tmp1_) {
		gboolean _tmp2_;
		_tmp2_ = model_contains ((GtkTreeModel*) liststore_index_recursively, HOME_STRING);
		_tmp0_ = !_tmp2_;
	} else {
		_tmp0_ = FALSE;
	}
	if (_tmp0_) {
		GtkTreeIter iter = {0};
		GtkTreeIter _tmp3_ = {0};
		GValue v = {0};
		gtk_list_store_append (liststore_index_recursively, &_tmp3_);
		iter = _tmp3_;
		g_value_init (&v, G_TYPE_STRING);
		g_value_set_string (&v, HOME_STRING);
		gtk_list_store_set_value (liststore_index_recursively, &iter, 0, &v);
		G_IS_VALUE (&v) ? (g_value_unset (&v), NULL) : NULL;
	}
	_tmp5_ = gtk_toggle_button_get_active (source);
	if (!_tmp5_) {
		gboolean _tmp6_;
		_tmp6_ = model_contains ((GtkTreeModel*) liststore_index_recursively, HOME_STRING);
		_tmp4_ = _tmp6_;
	} else {
		_tmp4_ = FALSE;
	}
	if (_tmp4_) {
		gboolean valid = FALSE;
		GtkTreeIter iter = {0};
		GtkTreeIter _tmp7_ = {0};
		gboolean _tmp8_;
		_tmp8_ = gtk_tree_model_get_iter_first ((GtkTreeModel*) liststore_index_recursively, &_tmp7_);
		iter = _tmp7_;
		valid = _tmp8_;
		while (TRUE) {
			GValue value = {0};
			GValue _tmp9_ = {0};
			const gchar* _tmp10_ = NULL;
			if (!valid) {
				break;
			}
			gtk_tree_model_get_value ((GtkTreeModel*) liststore_index_recursively, &iter, 0, &_tmp9_);
			G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
			value = _tmp9_;
			_tmp10_ = g_value_get_string (&value);
			if (g_strcmp0 (_tmp10_, HOME_STRING) == 0) {
				GtkTreeIter _tmp11_ = {0};
				gboolean _tmp12_;
				gtk_list_store_remove (liststore_index_recursively, &iter);
				_tmp12_ = gtk_tree_model_get_iter_first ((GtkTreeModel*) liststore_index_recursively, &_tmp11_);
				iter = _tmp11_;
				valid = _tmp12_;
			} else {
				gboolean _tmp13_;
				_tmp13_ = gtk_tree_model_iter_next ((GtkTreeModel*) liststore_index_recursively, &iter);
				valid = _tmp13_;
			}
			G_IS_VALUE (&value) ? (g_value_unset (&value), NULL) : NULL;
		}
	}
}


void fill_in_model (GtkListStore* model, GSList* list) {
	gint position;
	GError * _inner_error_ = NULL;
	g_return_if_fail (model != NULL);
	position = 0;
	{
		GSList* str_collection;
		GSList* str_it;
		str_collection = list;
		for (str_it = str_collection; str_it != NULL; str_it = str_it->next) {
			gchar* _tmp0_;
			gchar* str;
			_tmp0_ = g_strdup ((const gchar*) str_it->data);
			str = _tmp0_;
			{
				gchar* _tmp1_ = NULL;
				gchar* _tmp2_;
				gint _tmp3_;
				gchar* _tmp4_;
				_tmp1_ = g_filename_to_utf8 (str, (gssize) (-1), NULL, NULL, &_inner_error_);
				_tmp2_ = _tmp1_;
				if (_inner_error_ != NULL) {
					if (_inner_error_->domain == G_CONVERT_ERROR) {
						goto __catch0_g_convert_error;
					}
					_g_free0 (str);
					g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
					g_clear_error (&_inner_error_);
					return;
				}
				_tmp3_ = position;
				position = _tmp3_ + 1;
				_tmp4_ = _tmp2_;
				gtk_list_store_insert_with_values (model, NULL, _tmp3_, 0, _tmp4_, -1);
				_g_free0 (_tmp4_);
				goto __finally0;
				__catch0_g_convert_error:
				{
					GError * e;
					e = _inner_error_;
					_inner_error_ = NULL;
					g_print ("%s", e->message);
					_g_error_free0 (e);
				}
				__finally0:
				if (_inner_error_ != NULL) {
					_g_free0 (str);
					g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
					g_clear_error (&_inner_error_);
					return;
				}
				_g_free0 (str);
			}
		}
	}
}


void setup_standard_treeview (GtkTreeView* view, const gchar* title) {
	GtkCellRendererText* _tmp0_ = NULL;
	GtkCellRendererText* _tmp1_;
	GtkTreeViewColumn* _tmp2_ = NULL;
	GtkTreeViewColumn* _tmp3_;
	GtkTreeViewColumn* column;
	g_return_if_fail (view != NULL);
	g_return_if_fail (title != NULL);
	_tmp0_ = (GtkCellRendererText*) gtk_cell_renderer_text_new ();
	_tmp1_ = g_object_ref_sink (_tmp0_);
	_tmp2_ = gtk_tree_view_column_new_with_attributes (title, (GtkCellRenderer*) _tmp1_, "text", 0, NULL);
	column = (_tmp3_ = g_object_ref_sink (_tmp2_), _g_object_unref0 (_tmp1_), _tmp3_);
	gtk_tree_view_append_column (view, column);
	_g_object_unref0 (column);
}


gint _vala_main (gchar** args, int args_length1) {
	gint result = 0;
	TrackerConfig* _tmp0_ = NULL;
	TrackerConfig* _tmp1_;
	GtkBuilder* _tmp2_ = NULL;
	GtkBuilder* builder;
	GObject* _tmp3_ = NULL;
	GObject* _tmp4_;
	GtkWindow* _tmp5_;
	GtkWindow* _tmp6_;
	GObject* _tmp7_ = NULL;
	GObject* _tmp8_;
	GtkCheckButton* _tmp9_;
	GtkCheckButton* _tmp10_;
	GObject* _tmp11_ = NULL;
	GObject* _tmp12_;
	GtkCheckButton* _tmp13_;
	GtkCheckButton* _tmp14_;
	gboolean _tmp15_;
	gboolean _tmp16_;
	gboolean _tmp17_;
	GObject* _tmp18_ = NULL;
	GObject* _tmp19_;
	GtkSpinButton* _tmp20_;
	GtkSpinButton* _tmp21_;
	gint _tmp22_;
	GObject* _tmp23_ = NULL;
	GObject* _tmp24_;
	GtkCheckButton* _tmp25_;
	GtkCheckButton* _tmp26_;
	gboolean _tmp27_;
	GObject* _tmp28_ = NULL;
	GObject* _tmp29_;
	GtkCheckButton* _tmp30_;
	GtkCheckButton* _tmp31_;
	gboolean _tmp32_;
	GObject* _tmp33_ = NULL;
	GObject* _tmp34_;
	GtkCheckButton* _tmp35_;
	GtkCheckButton* _tmp36_;
	gboolean _tmp37_;
	gboolean _tmp38_;
	GObject* _tmp39_ = NULL;
	GObject* _tmp40_;
	GtkScale* _tmp41_;
	GtkScale* _tmp42_;
	gint _tmp43_;
	GObject* _tmp44_ = NULL;
	GObject* _tmp45_;
	GtkScale* _tmp46_;
	GtkScale* _tmp47_;
	gint _tmp48_;
	GObject* _tmp49_ = NULL;
	GObject* _tmp50_;
	GtkScale* _tmp51_;
	GtkScale* _tmp52_;
	gint _tmp53_;
	GObject* _tmp54_ = NULL;
	GObject* _tmp55_;
	GtkToggleButton* _tmp56_;
	GtkToggleButton* _tmp57_;
	GObject* _tmp58_ = NULL;
	GObject* _tmp59_;
	GtkNotebook* _tmp60_;
	GtkNotebook* _tmp61_;
	GObject* _tmp62_ = NULL;
	GObject* _tmp63_;
	GtkTreeView* _tmp64_;
	GtkTreeView* _tmp65_;
	GObject* _tmp66_ = NULL;
	GObject* _tmp67_;
	GtkTreeView* _tmp68_;
	GtkTreeView* _tmp69_;
	GObject* _tmp70_ = NULL;
	GObject* _tmp71_;
	GtkTreeView* _tmp72_;
	GtkTreeView* _tmp73_;
	GObject* _tmp74_ = NULL;
	GObject* _tmp75_;
	GtkTreeView* _tmp76_;
	GtkTreeView* _tmp77_;
	GObject* _tmp78_ = NULL;
	GObject* _tmp79_;
	GtkTreeView* _tmp80_;
	GtkTreeView* _tmp81_;
	const gchar* _tmp82_ = NULL;
	const gchar* _tmp83_ = NULL;
	const gchar* _tmp84_ = NULL;
	const gchar* _tmp85_ = NULL;
	const gchar* _tmp86_ = NULL;
	GObject* _tmp87_ = NULL;
	GObject* _tmp88_;
	GtkListStore* _tmp89_;
	GtkListStore* _tmp90_;
	GSList* _tmp91_ = NULL;
	gboolean _tmp92_;
	GObject* _tmp93_ = NULL;
	GObject* _tmp94_;
	GtkListStore* _tmp95_;
	GtkListStore* _tmp96_;
	GSList* _tmp97_ = NULL;
	GObject* _tmp98_ = NULL;
	GObject* _tmp99_;
	GtkListStore* _tmp100_;
	GtkListStore* _tmp101_;
	GSList* _tmp102_ = NULL;
	GObject* _tmp103_ = NULL;
	GObject* _tmp104_;
	GtkListStore* _tmp105_;
	GtkListStore* _tmp106_;
	GSList* _tmp107_ = NULL;
	GObject* _tmp108_ = NULL;
	GObject* _tmp109_;
	GtkListStore* _tmp110_;
	GtkListStore* _tmp111_;
	GSList* _tmp112_ = NULL;
	GError * _inner_error_ = NULL;
	gtk_init (&args_length1, &args);
	_tmp0_ = tracker_config_new_with_domain ("tracker-miner-fs");
	_tmp1_ = _tmp0_;
	_g_object_unref0 (config);
	config = _tmp1_;
	_tmp2_ = gtk_builder_new ();
	builder = _tmp2_;
	gtk_builder_add_from_file (builder, TRACKER_DATADIR G_DIR_SEPARATOR_S "tracker-preferences.ui", &_inner_error_);
	if (_inner_error_ != NULL) {
		_g_object_unref0 (builder);
		goto __catch1_g_error;
	}
	_tmp3_ = gtk_builder_get_object (builder, "tracker-preferences");
	_tmp5_ = _g_object_ref0 ((_tmp4_ = _tmp3_, GTK_IS_WINDOW (_tmp4_) ? ((GtkWindow*) _tmp4_) : NULL));
	_tmp6_ = _tmp5_;
	_g_object_unref0 (window);
	window = _tmp6_;
	_tmp7_ = gtk_builder_get_object (builder, "checkbutton_enable_index_on_battery");
	_tmp9_ = _g_object_ref0 ((_tmp8_ = _tmp7_, GTK_IS_CHECK_BUTTON (_tmp8_) ? ((GtkCheckButton*) _tmp8_) : NULL));
	_tmp10_ = _tmp9_;
	_g_object_unref0 (checkbutton_enable_index_on_battery);
	checkbutton_enable_index_on_battery = _tmp10_;
	_tmp11_ = gtk_builder_get_object (builder, "checkbutton_enable_index_on_battery_first_time");
	_tmp13_ = _g_object_ref0 ((_tmp12_ = _tmp11_, GTK_IS_CHECK_BUTTON (_tmp12_) ? ((GtkCheckButton*) _tmp12_) : NULL));
	_tmp14_ = _tmp13_;
	_g_object_unref0 (checkbutton_enable_index_on_battery_first_time);
	checkbutton_enable_index_on_battery_first_time = _tmp14_;
	_tmp15_ = tracker_config_get_index_on_battery (config);
	gtk_toggle_button_set_active ((GtkToggleButton*) checkbutton_enable_index_on_battery, _tmp15_);
	_tmp16_ = gtk_toggle_button_get_active ((GtkToggleButton*) checkbutton_enable_index_on_battery);
	gtk_widget_set_sensitive ((GtkWidget*) checkbutton_enable_index_on_battery_first_time, !_tmp16_);
	_tmp17_ = tracker_config_get_index_on_battery_first_time (config);
	gtk_toggle_button_set_active ((GtkToggleButton*) checkbutton_enable_index_on_battery_first_time, _tmp17_);
	_tmp18_ = gtk_builder_get_object (builder, "spinbutton_delay");
	_tmp20_ = _g_object_ref0 ((_tmp19_ = _tmp18_, GTK_IS_SPIN_BUTTON (_tmp19_) ? ((GtkSpinButton*) _tmp19_) : NULL));
	_tmp21_ = _tmp20_;
	_g_object_unref0 (spinbutton_delay);
	spinbutton_delay = _tmp21_;
	gtk_spin_button_set_increments (spinbutton_delay, (gdouble) 1, (gdouble) 1);
	_tmp22_ = tracker_config_get_initial_sleep (config);
	gtk_spin_button_set_value (spinbutton_delay, (gdouble) _tmp22_);
	_tmp23_ = gtk_builder_get_object (builder, "checkbutton_enable_monitoring");
	_tmp25_ = _g_object_ref0 ((_tmp24_ = _tmp23_, GTK_IS_CHECK_BUTTON (_tmp24_) ? ((GtkCheckButton*) _tmp24_) : NULL));
	_tmp26_ = _tmp25_;
	_g_object_unref0 (checkbutton_enable_monitoring);
	checkbutton_enable_monitoring = _tmp26_;
	_tmp27_ = tracker_config_get_enable_monitors (config);
	gtk_toggle_button_set_active ((GtkToggleButton*) checkbutton_enable_monitoring, _tmp27_);
	_tmp28_ = gtk_builder_get_object (builder, "checkbutton_index_removable_media");
	_tmp30_ = _g_object_ref0 ((_tmp29_ = _tmp28_, GTK_IS_CHECK_BUTTON (_tmp29_) ? ((GtkCheckButton*) _tmp29_) : NULL));
	_tmp31_ = _tmp30_;
	_g_object_unref0 (checkbutton_index_removable_media);
	checkbutton_index_removable_media = _tmp31_;
	_tmp32_ = tracker_config_get_index_removable_devices (config);
	gtk_toggle_button_set_active ((GtkToggleButton*) checkbutton_index_removable_media, _tmp32_);
	_tmp33_ = gtk_builder_get_object (builder, "checkbutton_index_optical_discs");
	_tmp35_ = _g_object_ref0 ((_tmp34_ = _tmp33_, GTK_IS_CHECK_BUTTON (_tmp34_) ? ((GtkCheckButton*) _tmp34_) : NULL));
	_tmp36_ = _tmp35_;
	_g_object_unref0 (checkbutton_index_optical_discs);
	checkbutton_index_optical_discs = _tmp36_;
	_tmp37_ = gtk_toggle_button_get_active ((GtkToggleButton*) checkbutton_index_removable_media);
	gtk_widget_set_sensitive ((GtkWidget*) checkbutton_index_optical_discs, _tmp37_);
	_tmp38_ = tracker_config_get_index_optical_discs (config);
	gtk_toggle_button_set_active ((GtkToggleButton*) checkbutton_index_optical_discs, _tmp38_);
	_tmp39_ = gtk_builder_get_object (builder, "hscale_disk_space_limit");
	_tmp41_ = _g_object_ref0 ((_tmp40_ = _tmp39_, GTK_IS_SCALE (_tmp40_) ? ((GtkScale*) _tmp40_) : NULL));
	_tmp42_ = _tmp41_;
	_g_object_unref0 (hscale_disk_space_limit);
	hscale_disk_space_limit = _tmp42_;
	_tmp43_ = tracker_config_get_low_disk_space_limit (config);
	gtk_range_set_value ((GtkRange*) hscale_disk_space_limit, (gdouble) _tmp43_);
	_tmp44_ = gtk_builder_get_object (builder, "hscale_throttle");
	_tmp46_ = _g_object_ref0 ((_tmp45_ = _tmp44_, GTK_IS_SCALE (_tmp45_) ? ((GtkScale*) _tmp45_) : NULL));
	_tmp47_ = _tmp46_;
	_g_object_unref0 (hscale_throttle);
	hscale_throttle = _tmp47_;
	_tmp48_ = tracker_config_get_throttle (config);
	gtk_range_set_value ((GtkRange*) hscale_throttle, (gdouble) _tmp48_);
	_tmp49_ = gtk_builder_get_object (builder, "hscale_drop_device_threshold");
	_tmp51_ = _g_object_ref0 ((_tmp50_ = _tmp49_, GTK_IS_SCALE (_tmp50_) ? ((GtkScale*) _tmp50_) : NULL));
	_tmp52_ = _tmp51_;
	_g_object_unref0 (hscale_drop_device_threshold);
	hscale_drop_device_threshold = _tmp52_;
	_tmp53_ = tracker_config_get_removable_days_threshold (config);
	gtk_range_set_value ((GtkRange*) hscale_drop_device_threshold, (gdouble) _tmp53_);
	_tmp54_ = gtk_builder_get_object (builder, "togglebutton_home");
	_tmp56_ = _g_object_ref0 ((_tmp55_ = _tmp54_, GTK_IS_TOGGLE_BUTTON (_tmp55_) ? ((GtkToggleButton*) _tmp55_) : NULL));
	_tmp57_ = _tmp56_;
	_g_object_unref0 (togglebutton_home);
	togglebutton_home = _tmp57_;
	_tmp58_ = gtk_builder_get_object (builder, "notebook");
	_tmp60_ = _g_object_ref0 ((_tmp59_ = _tmp58_, GTK_IS_NOTEBOOK (_tmp59_) ? ((GtkNotebook*) _tmp59_) : NULL));
	_tmp61_ = _tmp60_;
	_g_object_unref0 (notebook);
	notebook = _tmp61_;
	gtk_notebook_remove_page (notebook, 0);
	_tmp62_ = gtk_builder_get_object (builder, "treeview_index_recursively");
	_tmp64_ = _g_object_ref0 ((_tmp63_ = _tmp62_, GTK_IS_TREE_VIEW (_tmp63_) ? ((GtkTreeView*) _tmp63_) : NULL));
	_tmp65_ = _tmp64_;
	_g_object_unref0 (treeview_index_recursively);
	treeview_index_recursively = _tmp65_;
	_tmp66_ = gtk_builder_get_object (builder, "treeview_index_single");
	_tmp68_ = _g_object_ref0 ((_tmp67_ = _tmp66_, GTK_IS_TREE_VIEW (_tmp67_) ? ((GtkTreeView*) _tmp67_) : NULL));
	_tmp69_ = _tmp68_;
	_g_object_unref0 (treeview_index_single);
	treeview_index_single = _tmp69_;
	_tmp70_ = gtk_builder_get_object (builder, "treeview_ignored_directories");
	_tmp72_ = _g_object_ref0 ((_tmp71_ = _tmp70_, GTK_IS_TREE_VIEW (_tmp71_) ? ((GtkTreeView*) _tmp71_) : NULL));
	_tmp73_ = _tmp72_;
	_g_object_unref0 (treeview_ignored_directories);
	treeview_ignored_directories = _tmp73_;
	_tmp74_ = gtk_builder_get_object (builder, "treeview_ignored_directories_with_content");
	_tmp76_ = _g_object_ref0 ((_tmp75_ = _tmp74_, GTK_IS_TREE_VIEW (_tmp75_) ? ((GtkTreeView*) _tmp75_) : NULL));
	_tmp77_ = _tmp76_;
	_g_object_unref0 (treeview_ignored_directories_with_content);
	treeview_ignored_directories_with_content = _tmp77_;
	_tmp78_ = gtk_builder_get_object (builder, "treeview_ignored_files");
	_tmp80_ = _g_object_ref0 ((_tmp79_ = _tmp78_, GTK_IS_TREE_VIEW (_tmp79_) ? ((GtkTreeView*) _tmp79_) : NULL));
	_tmp81_ = _tmp80_;
	_g_object_unref0 (treeview_ignored_files);
	treeview_ignored_files = _tmp81_;
	_tmp82_ = _ ("Directory");
	setup_standard_treeview (treeview_index_recursively, _tmp82_);
	_tmp83_ = _ ("Directory");
	setup_standard_treeview (treeview_index_single, _tmp83_);
	_tmp84_ = _ ("Directory");
	setup_standard_treeview (treeview_ignored_directories, _tmp84_);
	_tmp85_ = _ ("Directory");
	setup_standard_treeview (treeview_ignored_directories_with_content, _tmp85_);
	_tmp86_ = _ ("File");
	setup_standard_treeview (treeview_ignored_files, _tmp86_);
	_tmp87_ = gtk_builder_get_object (builder, "liststore_index_recursively");
	_tmp89_ = _g_object_ref0 ((_tmp88_ = _tmp87_, GTK_IS_LIST_STORE (_tmp88_) ? ((GtkListStore*) _tmp88_) : NULL));
	_tmp90_ = _tmp89_;
	_g_object_unref0 (liststore_index_recursively);
	liststore_index_recursively = _tmp90_;
	_tmp91_ = tracker_config_get_index_recursive_directories_unfiltered (config);
	fill_in_model (liststore_index_recursively, _tmp91_);
	_tmp92_ = model_contains ((GtkTreeModel*) liststore_index_recursively, HOME_STRING);
	gtk_toggle_button_set_active (togglebutton_home, _tmp92_);
	_tmp93_ = gtk_builder_get_object (builder, "liststore_index_single");
	_tmp95_ = _g_object_ref0 ((_tmp94_ = _tmp93_, GTK_IS_LIST_STORE (_tmp94_) ? ((GtkListStore*) _tmp94_) : NULL));
	_tmp96_ = _tmp95_;
	_g_object_unref0 (liststore_index_single);
	liststore_index_single = _tmp96_;
	_tmp97_ = tracker_config_get_index_single_directories_unfiltered (config);
	fill_in_model (liststore_index_single, _tmp97_);
	_tmp98_ = gtk_builder_get_object (builder, "liststore_ignored_directories");
	_tmp100_ = _g_object_ref0 ((_tmp99_ = _tmp98_, GTK_IS_LIST_STORE (_tmp99_) ? ((GtkListStore*) _tmp99_) : NULL));
	_tmp101_ = _tmp100_;
	_g_object_unref0 (liststore_ignored_directories);
	liststore_ignored_directories = _tmp101_;
	_tmp102_ = tracker_config_get_ignored_directories (config);
	fill_in_model (liststore_ignored_directories, _tmp102_);
	_tmp103_ = gtk_builder_get_object (builder, "liststore_ignored_files");
	_tmp105_ = _g_object_ref0 ((_tmp104_ = _tmp103_, GTK_IS_LIST_STORE (_tmp104_) ? ((GtkListStore*) _tmp104_) : NULL));
	_tmp106_ = _tmp105_;
	_g_object_unref0 (liststore_ignored_files);
	liststore_ignored_files = _tmp106_;
	_tmp107_ = tracker_config_get_ignored_files (config);
	fill_in_model (liststore_ignored_files, _tmp107_);
	_tmp108_ = gtk_builder_get_object (builder, "liststore_gnored_directories_with_content");
	_tmp110_ = _g_object_ref0 ((_tmp109_ = _tmp108_, GTK_IS_LIST_STORE (_tmp109_) ? ((GtkListStore*) _tmp109_) : NULL));
	_tmp111_ = _tmp110_;
	_g_object_unref0 (liststore_gnored_directories_with_content);
	liststore_gnored_directories_with_content = _tmp111_;
	_tmp112_ = tracker_config_get_ignored_directories_with_content (config);
	fill_in_model (liststore_gnored_directories_with_content, _tmp112_);
	gtk_builder_connect_signals (builder, NULL);
	gtk_widget_show_all ((GtkWidget*) window);
	gtk_main ();
	_g_object_unref0 (builder);
	goto __finally1;
	__catch1_g_error:
	{
		GError * e;
		e = _inner_error_;
		_inner_error_ = NULL;
		fprintf (stderr, "Could not load UI: %s\n", e->message);
		result = 1;
		_g_error_free0 (e);
		return result;
	}
	__finally1:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return 0;
	}
	result = 0;
	return result;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return _vala_main (argv, argc);
}



