/*
 * Copyright (C) 2009, Debarshi Ray <debarshir@src.gnome.org>
 * Copyright (C) 2009, Martyn Russell <martyn@lanedo.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "config.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libnautilus-extension/nautilus-menu-provider.h>
#include <libnautilus-extension/nautilus-property-page-provider.h>

#include <libtracker-sparql/tracker-sparql.h>

#include "tracker-tags-utils.h"
#include "tracker-tags-view.h"

#define TRACKER_TYPE_TAGS_EXTENSION (tracker_tags_extension_get_type ())
#define TRACKER_TAGS_EXTENSION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRACKER_TYPE_TAGS_EXTENSION, TrackerTagsExtension))
#define TRACKER_TAGS_EXTENSION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRACKER_TYPE_TAGS_EXTENSION, TrackerTagsExtensionClass))

#define TRACKER_IS_TAGS_EXTENSION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRACKER_TYPE_TAGS_EXTENSION))
#define TRACKER_IS_TAGS_EXTENSION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRACKER_TYPE_TAGS_EXTENSION))

typedef struct _TrackerTagsExtension TrackerTagsExtension;
typedef struct _TrackerTagsExtensionClass TrackerTagsExtensionClass;

struct _TrackerTagsExtension {
	GObject parent;
};

struct _TrackerTagsExtensionClass {
	GObjectClass parent;
};

typedef void (*MenuDataFreeFunc)(gpointer data);

typedef struct {
	GList *data;
	GtkWidget *widget;
} MenuData;

static void  tracker_tags_extension_menu_provider_iface_init          (NautilusMenuProviderIface         *iface);
static void  tracker_tags_extension_property_page_provider_iface_init (NautilusPropertyPageProviderIface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (TrackerTagsExtension, tracker_tags_extension, G_TYPE_OBJECT, 0,
                                G_IMPLEMENT_INTERFACE (NAUTILUS_TYPE_MENU_PROVIDER,
                                                       tracker_tags_extension_menu_provider_iface_init)
                                G_IMPLEMENT_INTERFACE (NAUTILUS_TYPE_PROPERTY_PAGE_PROVIDER,
                                                       tracker_tags_extension_property_page_provider_iface_init));

static MenuData *
menu_data_new (GList     *files,
               GtkWidget *window)
{
	MenuData *md;

	md = g_slice_new (MenuData);

	md->data = nautilus_file_info_list_copy (files);
	md->widget = window;

	return md;
}

static void
menu_data_free (MenuData *md)
{
	if (md->data) {
		nautilus_file_info_list_free (md->data);
	}

	g_slice_free (MenuData, md);
}

static void
menu_data_destroy (gpointer  data,
                   GClosure *closure)
{
	menu_data_free (data);
}

static void
menu_tags_activate_cb (NautilusMenuItem *menu_item,
                       gpointer          user_data)
{
	MenuData *md = user_data;
	GList *files = md->data;
	GtkWindow *window = GTK_WINDOW (md->widget);
	GtkWidget *action_area;
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *view;

	dialog = gtk_dialog_new_with_buttons (N_("Tags"),
	                                      window,
	                                      GTK_DIALOG_MODAL |
	                                      GTK_DIALOG_DESTROY_WITH_PARENT |
#if GTK_CHECK_VERSION (2,90,7)
	                                      0,
#else
	                                      GTK_DIALOG_NO_SEPARATOR,
#endif
	                                      GTK_STOCK_CLOSE,
	                                      GTK_RESPONSE_CLOSE,
	                                      NULL);
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 5);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 250, 375);
	gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_transient_for (GTK_WINDOW (dialog), window);
	g_signal_connect (dialog, "response",
	                  G_CALLBACK (gtk_widget_destroy),
	                  NULL);

	action_area = gtk_dialog_get_action_area (GTK_DIALOG (dialog));
	gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);

	vbox = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	gtk_box_set_spacing (GTK_BOX (vbox), 2);

	view = tracker_tags_view_new (files);
	gtk_widget_show (view);

	gtk_box_pack_start (GTK_BOX (vbox), view, TRUE, TRUE, 0);

	gtk_widget_show_all (dialog);
}

static GList *
extension_get_file_items (NautilusMenuProvider *provider,
                          GtkWidget            *window,
                          GList                *files)
{
	GList *menu_items = NULL;
	NautilusMenuItem *menu_item;

	if (files == NULL) {
		return NULL;
	}

	menu_item = nautilus_menu_item_new ("tracker-tags-new",
	                                    N_("Tags..."),
	                                    N_("Tag one or more files"),
	                                    NULL);
	g_signal_connect_data (menu_item, "activate",
	                       G_CALLBACK (menu_tags_activate_cb),
	                       menu_data_new (files, window),
	                       menu_data_destroy,
	                       G_CONNECT_AFTER);

	menu_items = g_list_prepend (menu_items, menu_item);

	return g_list_reverse (menu_items);
}

static GList *
extension_get_pages (NautilusPropertyPageProvider *provider,
                     GList                        *files)
{
	GList *property_pages = NULL;
	GtkWidget *label;
	GtkWidget *view;
	NautilusPropertyPage *property_page;

	if (files == NULL) {
		return NULL;
	}

	label = gtk_label_new (_("Tags"));
	view = tracker_tags_view_new (files);
	gtk_widget_show (view);
	property_page = nautilus_property_page_new ("tracker-tags", label, view);
	property_pages = g_list_prepend (property_pages, property_page);

	return property_pages;
}

static void
tracker_tags_extension_menu_provider_iface_init (NautilusMenuProviderIface *iface)
{
	iface->get_file_items = extension_get_file_items;
}

static void
tracker_tags_extension_property_page_provider_iface_init (NautilusPropertyPageProviderIface *iface)
{
	iface->get_pages = extension_get_pages;
}

static void
tracker_tags_extension_class_init (TrackerTagsExtensionClass *klass)
{
}

static void
tracker_tags_extension_class_finalize (TrackerTagsExtensionClass *klass)
{
}

static void
tracker_tags_extension_init (TrackerTagsExtension *self)
{
}

void
nautilus_module_initialize (GTypeModule *module)
{
	g_debug ("Initializing tracker-tags extension\n");
	tracker_tags_extension_register_type (module);
	tracker_tags_view_register_types (module);
}

void
nautilus_module_shutdown (void)
{
	g_debug ("Shutting down tracker-tags extension\n");
}

void
nautilus_module_list_types (const GType **types,
                            gint         *num_types)
{
	static GType type_list[1];

	type_list[0] = tracker_tags_extension_type_id;
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
