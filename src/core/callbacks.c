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
     GString *result = g_string_new ("\n");
     GError *error = NULL;
     GIOStatus ret;
     char *line;
     gsize len;

     ret = g_io_channel_read_line (channel, &line, &len, NULL, &error);
     if (ret == G_IO_STATUS_ERROR)
     {
          g_warning ("Error reading UNIX socket '%s' : %s", global.unix_sock.path, error->message);
          g_io_channel_shutdown (channel, TRUE, NULL);
          g_error_free (error);
          return FALSE;
     }
     else if (ret == G_IO_STATUS_EOF)
     {
          /* shutdown and remove channel watch from main loop */
          g_io_channel_shutdown (channel, TRUE, NULL);
          return FALSE;
     }

     if (line)
     {
          run_command (line, &result, get_current_creamtabbed ());

          ret = g_io_channel_write_chars (channel, result->str, result->len, &len, &error);
          if (ret == G_IO_STATUS_ERROR)
          {
               g_warning ("Error writing UNIX socket '%s' : %s\n", global.unix_sock.path, error->message);
               g_error_free (error);
          }
          g_io_channel_flush (channel, NULL);
     }

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
     gboolean ret = FALSE;
     const gchar *text = gtk_entry_get_text (inputbox);

     static int cmd_history_id = -1;
     GString *cmd_history;

     if (strlen (text) == 0)
     {
          if (bind_buffer != NULL)
               g_string_free (bind_buffer, TRUE);
          bind_buffer = g_string_new (event->string);

          ret = bind_parse_buffer (obj);
     }
     else
     {
          global.browser.mode = CmdMode;
     }

     if (!ret)
     {
          ret = TRUE;
          switch (event->keyval)
          {
               case GDK_Up:
                    if (global.browser.cmd_history == NULL)
                         break;

                    cmd_history_id--;
                    if (cmd_history_id < 0)
                         cmd_history_id = g_slist_length (global.browser.cmd_history) - 1;
                    printf ("%d\n", cmd_history_id);

                    cmd_history = g_slist_nth_data (global.browser.cmd_history, cmd_history_id);
                    echo (obj, cmd_history->str);
                    break;

               case GDK_Down:
                    if (global.browser.cmd_history == NULL)
                         break;

                    cmd_history_id++;
                    if (cmd_history_id >= g_slist_length (global.browser.cmd_history))
                    {
                         cmd_history_id = g_slist_length (global.browser.cmd_history) - 1;
                         echo (obj, "");
                    }
                    else
                    {
                         cmd_history = g_slist_nth_data (global.browser.cmd_history, cmd_history_id);
                         echo (obj, cmd_history->str);
                    }
                    break;

               case GDK_Tab:
                    /*! \todo Completion (next) */
                    break;

               case GDK_ISO_Left_Tab:
                    /*! \todo Completion (previous) */
                    break;

               case GDK_Escape:
                    echo (obj, "");
                    global.browser.mode = BindMode;
                    gtk_widget_grab_focus (GTK_WIDGET (obj));

                    if (bind_buffer != NULL)
                    {
                         g_string_free (bind_buffer, TRUE);
                         bind_buffer = NULL;
                    }
                    break;

               case GDK_Return:
                    cmd_history_id = -1;

               default: ret = FALSE; break;
          }
     }

     return ret;
}

void cb_tray_view (GtkMenuItem *item, gpointer window)
{
     gtk_widget_show (GTK_WIDGET (window));
     gtk_window_deiconify (GTK_WINDOW (window));
}

void cb_tray_hide (GtkMenuItem *item, gpointer window)
{
     gtk_widget_hide (GTK_WIDGET (window));
     gtk_window_iconify (GTK_WINDOW (window));
}

void cb_tray_activated (GObject *trayIcon, gpointer window)
{
     /*
      * XXX TODO FIXME
      * On first start, need to click twice on the icon to hide ....
      */
     if (gtk_window_is_active (GTK_WINDOW (window)))
     {
          gtk_widget_hide (GTK_WIDGET (window));
          gtk_window_iconify (GTK_WINDOW (window));
     }
     else
     {
          gtk_widget_show (GTK_WIDGET (window));
          gtk_window_deiconify (GTK_WINDOW (window));
          gtk_window_present (GTK_WINDOW (window));
     }
}

void cb_tray_popup (GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
     gtk_menu_popup (GTK_MENU (popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}

