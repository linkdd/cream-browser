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

static void *cream_statusbar_new (void)
{
     GdkColor bg, fg;

     global.interface.statusbar.box      = gtk_hbox_new (FALSE, 0);
     global.interface.statusbar.eventbox = gtk_event_box_new ();
     global.interface.statusbar.url      = gtk_label_new (NULL);
     global.interface.statusbar.state    = gtk_label_new (NULL);

     /* change statusbar color */
     gdk_color_parse ("#000000", &bg);
     gtk_widget_modify_bg (global.interface.statusbar.eventbox, GTK_STATE_NORMAL, &bg);

     gdk_color_parse ("#FFFFFF", &fg);
     gtk_widget_modify_fg (global.interface.statusbar.url, GTK_STATE_NORMAL, &fg);
     gtk_widget_modify_fg (global.interface.statusbar.state, GTK_STATE_NORMAL, &fg);

     /* align the URL and the state */
     gtk_misc_set_alignment (GTK_MISC (global.interface.statusbar.url), 0.0, 0.0);
     gtk_misc_set_alignment (GTK_MISC (global.interface.statusbar.state), 1.0, 0.0);

     gtk_label_set_selectable (GTK_LABEL (global.interface.statusbar.state), TRUE);

     gtk_box_pack_start (GTK_BOX (global.interface.statusbar.box), global.interface.statusbar.url, TRUE, TRUE, 2);
     gtk_box_pack_start (GTK_BOX (global.interface.statusbar.box), global.interface.statusbar.state, FALSE, FALSE, 2);

     gtk_container_add (GTK_CONTAINER (global.interface.statusbar.eventbox), global.interface.statusbar.box);
}

static void cream_inputbox_new (void)
{
     PangoFontDescription *font;

     global.interface.inputbox = gtk_entry_new ();

     /* Change inputbox style */
     /* font */
     font = pango_font_description_from_string ("monospace normal 8");
     gtk_widget_modify_font (global.interface.inputbox, font);
     pango_font_description_free (font);

     /* border */
     gtk_entry_set_inner_border (GTK_ENTRY (global.interface.inputbox), NULL);
     gtk_entry_set_has_frame (GTK_ENTRY (global.interface.inputbox), FALSE);
}

GtkWidget *cream_interface_init (void)
{
     GdkGeometry hints = { 1, 1 }; /* GDK_HINT_MIN_SIZE for global.interface.main_window */

     /* create main window */
     global.interface.main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (global.interface.main_window), "Cream-Browser");
     gtk_window_set_default_size (GTK_WINDOW (global.interface.main_window), 800, 600);
     gtk_window_set_geometry_hints (GTK_WINDOW (global.interface.main_window), NULL, &hints, GDK_HINT_MIN_SIZE);

     global.interface.box = gtk_vbox_new (FALSE, 0);
     gtk_container_add (GTK_CONTAINER (global.interface.main_window), global.interface.box);

     cream_inputbox_new ();
     gtk_box_pack_end (GTK_BOX (global.interface.box), global.interface.inputbox, FALSE, FALSE, 0);

     cream_statusbar_new ();
     gtk_box_pack_end (GTK_BOX (global.interface.box), global.interface.statusbar.eventbox, FALSE, FALSE, 0);

     global.interface.view = cream_view_new ();
     cream_view_load_uri (CREAM_VIEW (global.interface.view), "http://cream-browser.net");
     gtk_box_pack_start (GTK_BOX (global.interface.box), global.interface.view, TRUE, TRUE, 0);

     /* restore cookies */
     global.browser.cookies = soup_cookie_jar_text_new (CREAM_FILE ("cookies.txt"), FALSE);
     soup_session_add_feature (webkit_get_default_session (), SOUP_SESSION_FEATURE (global.browser.cookies));

     /* connect signals */
     g_signal_connect (G_OBJECT (global.interface.main_window), "destroy",        G_CALLBACK (cb_cream_destroy),        NULL);
     g_signal_connect (G_OBJECT (global.interface.view),        "uri-changed",    G_CALLBACK (cb_cream_uri_changed),    NULL);
     g_signal_connect (G_OBJECT (global.interface.view),        "new-title",      G_CALLBACK (cb_cream_new_title),      NULL);
     g_signal_connect (G_OBJECT (global.interface.view),        "status-changed", G_CALLBACK (cb_cream_status_changed), NULL);

     return global.interface.main_window;
}
