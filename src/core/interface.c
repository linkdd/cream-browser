/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
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

void notebook_append_page (const gchar *uri)
{
     GtkWidget *creamtabbed = cream_tabbed_new ();
     cream_tabbed_load_uri (CREAM_TABBED (creamtabbed), uri);

     gtk_notebook_append_page (global.notebook, creamtabbed, cream_create_tab_label (CREAM_TABBED (creamtabbed)));
     gtk_notebook_set_tab_reorderable (global.notebook, creamtabbed, TRUE);

     g_signal_connect (G_OBJECT (creamtabbed), "update-notebook-title", G_CALLBACK (cb_cream_update_notebook_title), NULL);
}

GtkWidget *cream_interface_init (void)
{
     GdkGeometry hints = { 1, 1 }; /* GDK_HINT_MIN_SIZE for main_window */

     /* create main window */
     GtkWidget *main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (main_window), g_strdup_printf ("Cream-Browser <%d>", getpid ()));
     gtk_window_set_default_size (GTK_WINDOW (main_window), 800, 600);
     gtk_window_set_geometry_hints (GTK_WINDOW (main_window), NULL, &hints, GDK_HINT_MIN_SIZE);

     global.notebook = GTK_NOTEBOOK (gtk_notebook_new ());
     gtk_container_add (GTK_CONTAINER (main_window), GTK_WIDGET (global.notebook));

     /* connect signals */
     g_signal_connect (G_OBJECT (main_window), "destroy", G_CALLBACK (cb_cream_destroy), NULL);

     return main_window;
}
