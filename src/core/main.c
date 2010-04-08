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

struct XDG_t XDG[] =
{
     { "XDG_CONFIG_HOME", "~/.config" },
     { "XDG_DATA_HOME",   "~/.local/share" },
     { "XDG_CACHE_HOME",  "~/.cache" },
     { "XDG_CONFIG_DIRS", PREFIX "/etc/xdg:/etc/xdg" },
     { "XDG_DATA_DIRS",   PREFIX "/share:/usr/local/share/:/usr/share/" },
     { NULL, NULL }
};


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

char *str_replace (const char *search, const char *replace, const char *string)
{
     gchar **buf;
     char *ret;

     g_return_val_if_fail (string != NULL, NULL);

     buf = g_strsplit (string, search, -1);
     ret = g_strjoinv (replace, buf);
     g_strfreev (buf);

     return ret;
}

gchar *get_xdg_var (struct XDG_t xdg)
{
     const gchar *actual_val = getenv (xdg.env);
     const gchar *home = g_get_home_dir ();
     gchar *ret;

     if (!actual_val || strcmp (actual_val, "") == 0)
     {
          if (xdg.val)
          {
               ret = str_replace ("~", home, xdg.val);
          }
          else
          {
               ret = NULL;
          }
     }
     else
     {
          ret = str_replace ("~", home, xdg.val);
     }

     return ret;
}

gchar *get_xdg_var_by_name (gchar *name)
{
     int i;

     for (i = 0; XDG[i].env != NULL; ++i)
          if (strcmp (XDG[i].env, name) == 0)
               return get_xdg_var (XDG[i]);

     return NULL;
}

gchar *find_xdg_file (int xdg_type, const char *filename)
{
     gchar *xdg_var = NULL;
     gchar **xdgv;
     int i;

     if (xdg_type == XDG_TYPE_CONFIG)
     {
          xdg_var = g_strconcat (get_xdg_var_by_name ("XDG_CONFIG_HOME"), ":",
                                 get_xdg_var_by_name ("XDG_CONFIG_DIRS"), NULL);
     }
     else if (xdg_type == XDG_TYPE_DATA)
     {
          xdg_var = g_strconcat (get_xdg_var_by_name ("XDG_DATA_HOME"), ":",
                                 get_xdg_var_by_name ("XDG_DATA_DIRS"), NULL);
     }
     else if (xdg_type == XDG_TYPE_CACHE)
     {
          xdg_var = get_xdg_var_by_name ("XDG_CACHE_HOME");
     }

     xdg_var = str_replace ("~", g_get_home_dir (), xdg_var);
     xdgv = g_strsplit (xdg_var, ":", -1);

     for (i = 0; xdgv[i] != NULL; ++i)
     {
          gchar *file = g_strconcat (xdgv[i], "/", filename, NULL);
          if (g_file_test (file, G_FILE_TEST_EXISTS))
          {
               g_strfreev (xdgv);
               return file;
          }
     }

     g_strfreev (xdgv);
     return NULL;
}

void init_socket (void)
{
     /* Init UNIX socket */
     gnet_init ();

     global.unix_sock.path    = g_strdup_printf ("%s/%s_%d_socket", g_get_tmp_dir (), PACKAGE, getpid ());
     global.unix_sock.sock    = gnet_unix_socket_server_new (global.unix_sock.path);
     global.unix_sock.channel = gnet_unix_socket_get_io_channel (global.unix_sock.sock);
     g_io_add_watch (global.unix_sock.channel, G_IO_IN | G_IO_HUP, (GIOFunc) control_socket, NULL);
}

gboolean cream_init (int *argc, char ***argv, GError **error)
{
     GError *local_error = NULL;
     GOptionContext *ctx;
     gchar *cookie;

     /* init global structure */
     global.cmdline.config = NULL;
     global.cmdline.url = NULL;

     global.notebook = NULL;

     /* init GTK/Glib */
     gtk_init (argc, argv);

     if (!g_thread_supported ())
          g_thread_init (NULL);

     /* parse command line */
     ctx = g_option_context_new ("");
     g_option_context_add_main_entries (ctx, entries, "cream");

     if (!g_option_context_parse (ctx, argc, argv, &local_error) && local_error != NULL)
     {
          g_propagate_error (error, local_error);
          return FALSE;
     }

     /* crate directories */
     g_mkdir_with_parents (g_build_filename (get_xdg_var_by_name ("XDG_CONFIG_HOME"), "cream-browser", NULL), 0711);
     g_mkdir_with_parents (g_build_filename (get_xdg_var_by_name ("XDG_CONFIG_HOME"), "cream-browser", "downloads", NULL), 0755);

     if (global.cmdline.config == NULL)
     {
          global.cmdline.config = g_strdup (find_xdg_file (XDG_TYPE_CONFIG, "/cream-browser/config"));
          if (global.cmdline.config == NULL)
          {
               g_set_error (error, CREAM_ERROR, CREAM_ERROR_XDGFILENOTFOUND, "Can't find configuration.");
               return FALSE;
          }
     }

     /* load configuration */
     if (!cream_config_load (global.cmdline.config, &global.cfg, &local_error) && local_error != NULL)
     {
          g_propagate_error (error, local_error);
          return FALSE;
     }

     init_socket ();

     /* init CURL before any thread started */
     curl_global_init (CURL_GLOBAL_DEFAULT);

     /* restore cookies */
     if ((cookie = global.cfg.global.cookie) == NULL)
     {
          cookie = g_build_filename (get_xdg_var_by_name ("XDG_CONFIG_HOME"), "cream-browser", "cookies.txt", NULL);
     }
     cookie = str_replace ("~", g_get_home_dir (), cookie);

     global.browser.cookies = soup_cookie_jar_text_new (cookie, FALSE);
     soup_session_add_feature (webkit_get_default_session (), SOUP_SESSION_FEATURE (global.browser.cookies));

     if (global.cfg.global.history != NULL)
     {
          /* restore history */;
     }

     if (global.cfg.global.bookmarks != NULL)
     {
          /* restore bookmarks */;
     }


     return TRUE;
}

void cream_release (int exit_code)
{
     soup_cookie_jar_save (global.browser.cookies);

     if (global.cfg.global.history != NULL)
     {
          /* save history */;
     }

     if (global.cfg.global.bookmarks != NULL)
     {
          /* save bookmarks */;
     }

     cream_config_free (&global.cfg);

     /* free memory */
     free (global.cmdline.config);
     free (global.cmdline.url);

     unlink (global.unix_sock.path);

     gtk_main_quit ();

     curl_global_cleanup ();
     exit (exit_code);
}

gboolean run_command (const gchar *cmd, GString **ret)
{
     extern struct cmd_t commands[];
     GError *error = NULL;
     gint argc;
     gchar **argv;
     int i;

     if (!g_shell_parse_argv (cmd, &argc, &argv, &error) || error != NULL)
     {
          if (ret != NULL)
               *ret = g_string_new (error->message);
          g_error_free (error);
          return FALSE;
     }

     for (i = 0; i < argc; ++i)
          printf ("[%s] ", argv[i]);
     printf ("\n");

     if (ret != NULL)
          *ret = g_string_new ("success\r\n");

     return TRUE;
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

     if (global.cmdline.url != NULL)
          notebook_append_page (global.cmdline.url);
     else
          notebook_append_page (global.cfg.global.homepage);

     gtk_widget_show_all (win);
     gtk_main ();
     return 0;
}

GQuark cream_error_quark (void)
{
     static GQuark cream_error = 0;

     if (cream_error == 0)
     {
          cream_error = g_quark_from_string ("CREAM_ERROR");
     }

     return cream_error;
}

