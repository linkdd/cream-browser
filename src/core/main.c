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

struct global_t global;

GOptionEntry entries[] =
{
     { "config", 'c', 0, G_OPTION_ARG_FILENAME, &global.cmdline.config, "Load an alternative configuration file" },
     { "open", 'o', 0, G_OPTION_ARG_STRING, &global.cmdline.url, "Open URL" },
     { NULL }
};

const struct handler_t cream_handlers[] =
{
     { "download", NULL },
     { NULL, NULL }
};

void init_socket (void)
{
     /* Init UNIX socket */
     gnet_init ();

     global.unix_sock.path    = g_strdup_printf ("%s/%s_%d_socket", g_get_tmp_dir (), CREAM_PROGNAME, getpid ());
     global.unix_sock.sock    = gnet_unix_socket_server_new (global.unix_sock.path);
     global.unix_sock.channel = gnet_unix_socket_get_io_channel (global.unix_sock.sock);
     g_io_add_watch (global.unix_sock.channel, G_IO_IN | G_IO_HUP, (GIOFunc) control_socket, NULL);
}

gboolean cream_init (int *argc, char ***argv, GError **error)
{
     GError *local_error = NULL;
     GOptionContext *ctx;
     const gchar *home;

     /* init global structure */
     global.cmdline.config = NULL;
     global.cmdline.url = NULL;

     global.browser.user_agent = NULL;
     global.browser.homepage   = NULL;
     global.browser.encoding   = NULL;

     global.notebook = NULL;

     /* init signals */
     signal (SIGSEGV, signal_handler);
     signal (SIGINT,  signal_handler);
     signal (SIGKILL, signal_handler);
     signal (SIGTERM, signal_handler);

     /* parse command line */
     ctx = g_option_context_new ("");
     g_option_context_add_main_entries (ctx, entries, "cream");

     if (!g_option_context_parse (ctx, argc, argv, &local_error) && local_error != NULL)
     {
          g_propagate_error (error, local_error);
          return FALSE;
     }

     /* crate directories */
     home = g_get_home_dir ();
     g_mkdir_with_parents (g_build_filename (home, ".cream-browser", NULL), 0711);
     g_mkdir_with_parents (g_build_filename (home, ".cream-browser", "downloads", NULL), 0755);

     init_socket ();

     /* init CURL before any thread started */
     curl_global_init (CURL_GLOBAL_DEFAULT);

     /* init GTK/Glib */
     gtk_init (argc, argv);

     if (!g_thread_supported ())
          g_thread_init (NULL);

     /* restore cookies */
     global.browser.cookies = soup_cookie_jar_text_new (CREAM_FILE ("cookies.txt"), FALSE);
     soup_session_add_feature (webkit_get_default_session (), SOUP_SESSION_FEATURE (global.browser.cookies));

     return TRUE;
}

void cream_release (int exit_code)
{
     soup_cookie_jar_save (global.browser.cookies);

     /* free memory */
     free (global.cmdline.config);
     free (global.cmdline.url);

     free (global.browser.user_agent);
     free (global.browser.homepage);
     free (global.browser.encoding);

     unlink (global.unix_sock.path);

     gtk_main_quit ();

     curl_global_cleanup ();
     exit (exit_code);
}

int main (int argc, char **argv)
{
     GError *error = NULL;
     GtkWidget *win;

     if (!cream_init (&argc, &argv, &error) || error != NULL)
     {
          fprintf (stderr, "Error: %s\n", error->message);
          g_error_free (error);
          exit (EXIT_FAILURE);
     }

     win = cream_interface_init ();

     notebook_append_page ("http://google.fr/");
     notebook_append_page ("http://linkdd.ydb.me/");
     notebook_append_page ("http://cream-browser.net/");

     gtk_widget_show_all (win);
     gtk_main ();
     return 0;
}
