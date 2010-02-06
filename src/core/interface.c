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

void notebook_append_page (GtkNotebook *notebook, gchar *uri)
{
     GtkWidget *creamtabbed = cream_tabbed_new ();
     cream_tabbed_load_uri (CREAM_TABBED (creamtabbed), uri);
     GtkWidget *label = gtk_label_new (cream_tabbed_get_title (CREAM_TABBED (creamtabbed)));

     gtk_notebook_append_page (notebook, creamtabbed, label);
}

GtkWidget *cream_interface_init (void)
{
     GdkGeometry hints = { 1, 1 }; /* GDK_HINT_MIN_SIZE for main_window */
     GtkWidget *main_window;
     GtkWidget *notebook;

     /* create main window */
     main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (main_window), "Cream-Browser");
     gtk_window_set_default_size (GTK_WINDOW (main_window), 800, 600);
     gtk_window_set_geometry_hints (GTK_WINDOW (main_window), NULL, &hints, GDK_HINT_MIN_SIZE);

     notebook = gtk_notebook_new ();
     gtk_container_add (GTK_CONTAINER (main_window), notebook);
     notebook_append_page (GTK_NOTEBOOK (notebook), "http://google.fr");
     notebook_append_page (GTK_NOTEBOOK (notebook), "http://linkdd.ydb.me");
     notebook_append_page (GTK_NOTEBOOK (notebook), "http://cream-browser.net");

     /* restore cookies */
     global.browser.cookies = soup_cookie_jar_text_new (CREAM_FILE ("cookies.txt"), FALSE);
     soup_session_add_feature (webkit_get_default_session (), SOUP_SESSION_FEATURE (global.browser.cookies));

     /* connect signals */
     g_signal_connect (G_OBJECT (main_window), "destroy", G_CALLBACK (cb_cream_destroy), NULL);

     return main_window;
}
