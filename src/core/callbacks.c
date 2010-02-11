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

gboolean control_socket (GIOChannel *channel)
{
     struct sockaddr_un remote;
     unsigned int t = sizeof (remote);
     int csock;
     GIOChannel *cchannel;

     csock = accept (g_io_channel_unix_get_fd (channel),
                     (struct sockaddr *) &remote, &t);

     if ((cchannel = g_io_channel_unix_new (csock)))
     {
          g_io_add_watch (cchannel, G_IO_IN | G_IO_HUP, (GIOFunc) control_client_socket, cchannel);
     }

     return TRUE;
}

gboolean control_client_socket (GIOChannel *channel)
{
     GString *result = g_string_new ("");
     GError *error = NULL;
     GIOStatus ret;
     char *line;
     gsize len;

     ret = g_io_channel_read_line (channel, &line, &len, NULL, &error);
     if (ret == G_IO_STATUS_ERROR)
     {
          g_warning ("Error reading UNIX socket '%s' : %s\n", global.unix_sock.path, error->message);
          g_io_channel_shutdown (channel, TRUE, &error);
          return FALSE;
     }
     else if (ret == G_IO_STATUS_EOF)
     {
          /* shutdown and remove channel watch from main loop */
          g_io_channel_shutdown (channel, TRUE, &error);
          return FALSE;
     }

     if (line)
     {
          printf ("Client said: %s", line);
          g_string_append (result, "server thanks you\n");

          ret = g_io_channel_write_chars (channel, result->str, result->len, &len, &error);
          if (ret == G_IO_STATUS_ERROR)
          {
               g_warning ("Error writing UNIX socket '%s' : %s\n", global.unix_sock.path, error->message);
          }
          g_io_channel_flush (channel, &error);
     }

     if (error) g_error_free (error);
     g_string_free (result, TRUE);
     g_free (line);
     return TRUE;
}

void cb_cream_destroy (GtkWidget *emit, gpointer data)
{
     cream_release (EXIT_SUCCESS);
}

void cb_cream_update_notebook_title (GtkWidget *child, gpointer data)
{
     gtk_notebook_set_tab_label (global.notebook, child, cream_create_tab_label (CREAM_TABBED (child)));
}

void cb_cream_notebook_close_page (GtkButton *button, GtkWidget *child)
{
     gtk_notebook_remove_page (global.notebook,
          gtk_notebook_page_num (global.notebook, child)
     );

     if (gtk_notebook_get_n_pages (global.notebook) == 0)
          cream_release (EXIT_SUCCESS);
}

gboolean cb_inputbox_keys (GtkEntry *inputbox, GdkEventKey *event, CreamTabbed *obj)
{
     gboolean ret = TRUE;

     switch (event->keyval)
     {
          case GDK_Up:
               /* TODO: Go up in the commands' history */
               break;

          case GDK_Down:
               /* TODO: Go down in the commands' history */
               break;

          case GDK_Escape:
          {
               GtkWidget *content = cream_view_get_content (CREAM_VIEW (obj->creamview));

               if (MODULE_IS_WEB_VIEW (content))
               {
                    WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));
                    g_object_set (settings, "enable-caret-browsing", FALSE, NULL);
                    webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
               }

               echo (obj, "");
               gtk_widget_grab_focus (obj->creamview);
               break;
          }

          case GDK_Tab:
               /* TODO: Completion (next) */
               break;

          case GDK_ISO_Left_Tab:
               /* TODO: Completion (previous) */
               break;

          default: ret = FALSE; break;
     }

     return ret;
}
