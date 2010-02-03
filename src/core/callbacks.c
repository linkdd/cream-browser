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
     GdkColor color;
     gboolean ssl = g_str_has_prefix (uri, "https://");
     gboolean back = cream_view_can_go_back (CREAM_VIEW (global.interface.view));
     gboolean fwd = cream_view_can_go_forward (CREAM_VIEW (global.interface.view));

     if (back || fwd)
     {
          gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.url), g_markup_printf_escaped (
               "<span font=\"monospace bold 8\">%s</span> [%s%s]", uri, (back ? "+" : ""), (fwd ? "-" : ""))
          );
     }
     else
     {
          gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.url), g_markup_printf_escaped (
               "<span font=\"monospace bold 8\">%s</span>", uri)
          );
     }

     gdk_color_parse ((ssl ? "#B0FF00" : "#000000"), &color);
     gtk_widget_modify_bg (global.interface.statusbar.eventbox, GTK_STATE_NORMAL, &color);

     gdk_color_parse ((ssl ? "#000000" : "#FFFFFF"), &color);
     gtk_widget_modify_fg (global.interface.statusbar.url, GTK_STATE_NORMAL, &color);
     gtk_widget_modify_fg (global.interface.statusbar.state, GTK_STATE_NORMAL, &color);
}

void cb_cream_new_title (GtkWidget *emit, gchar *title, gpointer data)
{
     gtk_window_set_title (GTK_WINDOW (global.interface.main_window), g_strconcat (title, " | Cream-Browser", NULL));
}

void cb_cream_status_changed (GtkWidget *emit, gchar *status, gpointer data)
{
     gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.url), g_markup_printf_escaped (
          "<span font=\"monospace bold 8\">%s</span>", status)
     );
}

void cb_cream_scroll (GtkAdjustment *vadjust, gpointer data)
{
     gint max = gtk_adjustment_get_upper (vadjust) - gtk_adjustment_get_page_size (vadjust);
     gint val = (gint) (gtk_adjustment_get_value (vadjust) / max * 100);
     gchar *str;

     if (max == 0) /* If we see all the page */
          str = g_strdup ("All");
     else if (val == 0) /* we are on the page's top */
          str = g_strdup ("Top");
     else if (val == 100) /* we are on the page's bottom */
          str = g_strdup ("Bot");
     else
          str = g_strdup_printf ("%d%%", val);

     gtk_label_set_markup (GTK_LABEL (global.interface.statusbar.state), g_markup_printf_escaped ("<span font=\"monospace bold 8\">%s</span>", str));
}
