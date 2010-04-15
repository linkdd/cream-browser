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

void init_variables (void)
{
     struct variable_t internal_variables[] =
     {
          /* <key>            <value>,  <read-write>   <type>    <storage> */
          { "homepage",       NULL,          TRUE,     String,   &global.cfg.global.homepage },
          { "encodage",       NULL,          TRUE,     String,   &global.cfg.global.encodage },
          { "javascript",     NULL,          TRUE,     Boolean,  &global.cfg.global.javascript },
          { "mode",           NULL,          TRUE,     Integer,  &global.browser.mode },
          { "config",         NULL,          FALSE,    String,   &global.cmdline.config },
          { "bookmarks",      NULL,          FALSE,    String,   &global.cfg.global.bookmarks },
          { "history",        NULL,          FALSE,    String,   &global.cfg.global.history },
          { "cookie",         NULL,          FALSE,    String,   &global.cfg.global.cookie },
          /* browser variables */
          { "version",        VERSION,       FALSE,    None,     NULL },
          { "webkit_major",   WEBKIT_MAJOR,  FALSE,    None,     NULL },
          { "webkit_minor",   WEBKIT_MINOR,  FALSE,    None,     NULL },
          { "webkit_micro",   WEBKIT_MICRO,  FALSE,    None,     NULL },
          /* system informations */
          { "arch",           ARCH,          FALSE,    None,     NULL },
          { "uname-a",        UNAME_A,       FALSE,    None,     NULL },
          { "uname-s",        UNAME_S,       FALSE,    None,     NULL },
          { "uname-n",        UNAME_N,       FALSE,    None,     NULL },
          { "uname-r",        UNAME_R,       FALSE,    None,     NULL },
          { "uname-v",        UNAME_V,       FALSE,    None,     NULL },
          { "uname-p",        UNAME_P,       FALSE,    None,     NULL },
          { "uname-i",        UNAME_I,       FALSE,    None,     NULL },
          { "uname-o",        UNAME_O,       FALSE,    None,     NULL },

          { NULL,             NULL,          FALSE,    None,     NULL }
     };
     int i;

     for (i = 0; internal_variables[i].key != NULL; ++i)
     {
          struct variable_t *tmp = malloc (sizeof (struct variable_t));

          tmp->key        = g_strdup (internal_variables[i].key);
          if (internal_variables[i].value != NULL)
          {
               tmp->value = g_strdup (internal_variables[i].value);
          }
          else
          {
               tmp->value = NULL;
          }
          tmp->readwrite  = internal_variables[i].readwrite;
          tmp->type       = internal_variables[i].type;
          tmp->data       = internal_variables[i].data;

          global.browser.variables = g_slist_append (global.browser.variables, tmp);
     }
}

gboolean cream_init (int *argc, char ***argv, GError **error)
{
     GError *local_error = NULL;
     GOptionContext *ctx;
     gchar *cookie;

     /* init global structure */
     global.cmdline.config = NULL;
     global.cmdline.url = NULL;

     global.browser.variables = NULL;

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

     global.browser.mode = InsertMode;
     global.browser.clipboard = gtk_clipboard_get (GDK_SELECTION_PRIMARY);

     init_variables ();

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

CreamTabbed *get_current_creamtabbed (void)
{
     GtkWidget *child;

     child = gtk_notebook_get_nth_page (global.notebook,
               gtk_notebook_get_current_page (global.notebook));

     return CREAM_TABBED (child);
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

gboolean set (char *key, char *value)
{
     struct variable_t *tmp;
     int i, len = g_slist_length (global.browser.variables);

     for (i = 0; i < len; ++i)
     {
          tmp = (struct variable_t *) g_slist_nth_data (global.browser.variables, i);

          if (g_str_equal (key, tmp->key))
          {
               if (tmp->readwrite)
               {
                    free (tmp->value);
                    tmp->value = g_strdup (value);

                    if (tmp->data != NULL)
                    {
                         switch (tmp->type)
                         {
                              case String:  *(char **) tmp->data = g_strdup (value); break;
                              case Integer: *(int *) tmp->data = (int) strtol (value, NULL, 10); break;
                              case Boolean:
                                   if (!strcasecmp (value, "true"))
                                        *(gboolean *) tmp->data = TRUE;
                                   else
                                        *(gboolean *) tmp->data = FALSE;
                                   break;
                             default: break;
                         }
                    }
                    return TRUE;
               }
               else
                    return FALSE;
          }
     }

     tmp = malloc (sizeof (struct variable_t));
     tmp->key       = g_strdup (key);
     tmp->value     = g_strdup (value);
     tmp->readwrite = TRUE;
     tmp->type      = None;
     tmp->data      = NULL;

     global.browser.variables = g_slist_append (global.browser.variables, tmp);

     return TRUE;
}

char *get (char *key)
{
     struct variable_t *tmp;
     int i, len = g_slist_length (global.browser.variables);

     for (i = 0; i < len; ++i)
     {
          tmp = (struct variable_t *) g_slist_nth_data (global.browser.variables, i);

          if (g_str_equal (key, tmp->key))
          {
               if (tmp->value == NULL && tmp->type != None)
               {
                    switch (tmp->type)
                    {
                         case String:
                              return *(char **) tmp->data;

                         case Integer:
                              return g_strdup_printf ("%d", * (int *) tmp->data);

                         case Boolean:
                              if ((* (gboolean *) tmp->data) == TRUE)
                                   return "true";
                              else
                                   return "false";

                         default:
                              return NULL;
                    }
               }

               return tmp->value;
          }
     }

     return NULL;
}
