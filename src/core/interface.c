/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

#include "local.h"

GtkWidget *cream_create_tab_label (CreamTabbed *obj)
{
     GtkWidget *hbox, *label, *button;
     GdkPixbuf *ico;

     gchar *txt = g_strdup (cream_tabbed_get_title (obj));
     GtkRcStyle *rcstyle;
     GtkWidget *image;

     hbox = gtk_hbox_new (FALSE, 0);

     ico = cream_tabbed_get_favicon (obj);
     if (ico)
          gtk_box_pack_start (GTK_BOX (hbox), gtk_image_new_from_pixbuf (ico), FALSE, FALSE, 0);
     else
          gtk_box_pack_start (GTK_BOX (hbox), gtk_image_new_from_stock (GTK_STOCK_FILE, GTK_ICON_SIZE_MENU), FALSE, FALSE, 0);

     /* truncate title */
     if (txt == NULL)
          txt = g_strdup ("about:blank");

     if (strlen (txt) > 25)
     {
          txt[24] = 0;
          txt = g_strdup_printf ("%s...", txt);
     }

     label = gtk_label_new (txt);
     gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 5);

     /* button */
     image = gtk_image_new_from_stock (GTK_STOCK_CLOSE, GTK_ICON_SIZE_MENU);
     gtk_misc_set_alignment (GTK_MISC (image), 0.0, 0.0);

     rcstyle = gtk_rc_style_new ();
     rcstyle->xthickness = rcstyle->ythickness = 0;

     button = gtk_button_new ();
     gtk_button_set_image (GTK_BUTTON (button), image);
     gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);

     gtk_widget_modify_style (button, rcstyle);
     g_object_unref (rcstyle);

     gtk_box_pack_end (GTK_BOX (hbox), button, FALSE, FALSE, 0);

     gtk_widget_show_all (hbox);

     g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (cb_cream_notebook_close_page), GTK_WIDGET (obj));

     return hbox;
}

GtkWidget *notebook_append_page (const gchar *uri)
{
     GtkWidget *creamtabbed = cream_tabbed_new ();
     cream_tabbed_load_uri (CREAM_TABBED (creamtabbed), uri);

     gtk_notebook_append_page (global.notebook, creamtabbed, cream_create_tab_label (CREAM_TABBED (creamtabbed)));
     gtk_notebook_set_tab_reorderable (global.notebook, creamtabbed, TRUE);

     g_signal_connect (G_OBJECT (creamtabbed), "update-notebook-title", G_CALLBACK (cb_cream_update_notebook_title), NULL);
     gtk_widget_show_all (GTK_WIDGET (global.notebook));

     return creamtabbed;
}

GtkWidget *cream_interface_init (void)
{
     GdkGeometry hints = { 1, 1 }; /* GDK_HINT_MIN_SIZE for main_window */
     gchar *path;

     /* create main window */
     GtkWidget *main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (main_window), g_strdup_printf ("Cream-Browser <%d>", getpid ()));
     gtk_window_set_default_size (GTK_WINDOW (main_window), 800, 600);
     gtk_window_set_geometry_hints (GTK_WINDOW (main_window), NULL, &hints, GDK_HINT_MIN_SIZE);
     gtk_window_set_wmclass (GTK_WINDOW (main_window), "Cream-Browser", "Browser");

     if ((path = find_xdg_file (XDG_TYPE_DATA, "/cream-browser/cream.png")) != NULL)
          gtk_window_set_icon_from_file (GTK_WINDOW (main_window), path, NULL);

     global.notebook = GTK_NOTEBOOK (gtk_notebook_new ());
     gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (global.notebook));

     /* connect signals */
     g_signal_connect (G_OBJECT (main_window), "destroy", G_CALLBACK (cb_cream_destroy), NULL);
     g_signal_connect (G_OBJECT (global.notebook), "key-press-event", G_CALLBACK (bind_getkey), NULL);

     return main_window;
}

GtkStatusIcon *cream_icon_init (GtkWidget *window)
{
     GtkStatusIcon *CreamIcon;
     GtkWidget *menu, *item;
     gchar *path;

     if ((path = find_xdg_file (XDG_TYPE_DATA, "/cream-browser/cream.png")) == NULL)
          return NULL;

     /* System Tray Icon */
     /* Tray icon file */
     CreamIcon = gtk_status_icon_new_from_file (path);

     gtk_status_icon_set_tooltip (GTK_STATUS_ICON (CreamIcon),
          g_strdup_printf ("Cream-Browser <%d>", getpid ()));
     gtk_status_icon_set_visible (GTK_STATUS_ICON (CreamIcon), TRUE);

     /* popup menu for tray icon */
     menu = gtk_menu_new ();

     item = gtk_menu_item_new_with_label ("View");
     g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (cb_tray_view), window);
     gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

     item = gtk_menu_item_new_with_label ("Hide");
     g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (cb_tray_hide), window);
     gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

     item = gtk_menu_item_new_with_label ("Quit");
     g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (cb_cream_destroy), window);
     gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

     g_signal_connect (G_OBJECT (CreamIcon), "activate",   G_CALLBACK (cb_tray_activated), window);
     g_signal_connect (G_OBJECT (CreamIcon), "popup-menu", G_CALLBACK (cb_tray_popup),     menu);

     gtk_widget_show_all (menu);

     return GTK_STATUS_ICON (CreamIcon);
}
