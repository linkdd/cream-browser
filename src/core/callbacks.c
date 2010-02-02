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

void signal_handler (int signum)
{
     switch (signum)
     {
          case SIGSEGV:
               fprintf (stderr, "Cream: Segmentation fault\n");
               cream_release (EXIT_FAILURE);
               break;
     }
}

void cb_cream_destroy (GtkWidget *emit, gpointer data)
{
     cream_release (EXIT_SUCCESS);
}

void cb_cream_uri_changed (GtkWidget *emit, gchar *uri, gpointer data)
{
     gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.url), g_markup_printf_escaped (
          "<span font=\"monospace bold 8\">%s</span>", uri)
     );
}

void cb_cream_new_title (GtkWidget *emit, gchar *title, gpointer data)
{
     gtk_window_set_title (GTK_WINDOW (global.interface.main_window), g_strconcat ("Cream - ", title, NULL));
}

void cb_cream_status_changed (GtkWidget *emit, gchar *status, gpointer data)
{
     gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.state), g_markup_printf_escaped (
          "<span font=\"monospace bold 8\">%s</span>", status)
     );
}
