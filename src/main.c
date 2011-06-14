/*
 * Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "local.h"

/*!
 * \addtogroup cream-browser
 * @{
 */

#define CREAM_GLOBAL_ERROR         cream_global_error_quark()

typedef enum
{
     CREAM_GLOBAL_ERROR_CONFIG,
     CREAM_GLOBAL_ERROR_THEME,
     CREAM_GLOBAL_ERROR_SOCKET,
     CREAM_GLOBAL_ERROR_CONNECT,
     CREAM_GLOBAL_ERROR_SEND,
     CREAM_GLOBAL_ERROR_FAILED
} CreamGlobalError;

static GQuark cream_global_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream");

     return domain;
}

struct global_t global;

/*!
 * \fn gchar *find_file (guint type, const gchar *filename)
 * @param type Type of file to find (#FILE_TYPE_CONFIG, #FILE_TYPE_DATA or #FILE_TYPE_CACHE).
 * @param filename Name of the file to find.
 * @return Path to the file, or <code>NULL</code> if not found.
 *
 * Find a file into the user/system directories.
 */
gchar *find_file (guint type, const gchar *filename)
{
     gchar *ret = NULL;
     int i;

     switch (type)
     {
          case FILE_TYPE_CONFIG:
               ret = g_build_filename (g_get_user_config_dir (), global.prgname, filename, NULL);

               if (!g_file_test (ret, G_FILE_TEST_EXISTS))
               {
                    const gchar * const *dirs = g_get_system_config_dirs ();

                    g_free (ret), ret = NULL;
                    for (i = 0; dirs[i] != NULL; ++i)
                    {
                         ret = g_build_filename (dirs[i], global.prgname, filename, NULL);

                         if (g_file_test (ret, G_FILE_TEST_EXISTS))
                              return ret;

                         g_free (ret), ret = NULL;
                    }
               }

               break;

          case FILE_TYPE_DATA:
               ret = g_build_filename (g_get_user_data_dir (), global.prgname, filename, NULL);

               if (!g_file_test (ret, G_FILE_TEST_EXISTS))
               {
                    const gchar * const *dirs = g_get_system_data_dirs ();

                    g_free (ret), ret = NULL;
                    for (i = 0; dirs[i] != NULL; ++i)
                    {
                         ret = g_build_filename (dirs[i], global.prgname, filename, NULL);

                         if (g_file_test (ret, G_FILE_TEST_EXISTS))
                              return ret;

                         g_free (ret), ret = NULL;
                    }
               }

               break;

          case FILE_TYPE_CACHE:
               ret = g_build_filename (g_get_user_cache_dir (), global.prgname, filename, NULL);

               if (g_file_test (ret, G_FILE_TEST_EXISTS))
                    return ret;

               g_free (ret), ret = NULL;
               break;
     }

     return ret;
}

/*!
 * \fn char *str_replace (const char *search, const char *replace, const char *string)
 * @param search Text to search into the string
 * @param replace Replace \a search by the specified text
 * @param string String to modify
 * @return A newly allocated string.
 *
 * Replace text into a string
 */
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

/*!
 * \fn void add_protocol (const gchar *scheme, CreamModule *mod)
 * @param scheme A scheme.
 * @param mod A #CreamModule object.
 *
 * Associate a protocol to a module.
 */
void add_protocol (const gchar *scheme, CreamModule *mod)
{
     g_hash_table_insert (global.protocols, g_strdup (scheme), mod);
}

/*!
 * \fn void del_protocol (CreamModule *mod)
 * @param mod A #CreamModule object.
 *
 * Delete a module from the list.
 */
void del_protocol (CreamModule *mod)
{
     g_hash_table_foreach_remove (global.protocols, (GHRFunc) g_direct_equal, mod);
}

/*!
 * \fn CreamModule *get_protocol (const gchar *scheme)
 * @param scheme A scheme.
 * @return The associated #CreamModule.
 *
 * Returns the associated #CreamModule object.
 */
CreamModule *get_protocol (const gchar *scheme)
{
     return (CreamModule *) g_hash_table_lookup (global.protocols, scheme);
}

/* Print a GError */
void print_error (GError *error, gboolean abort, const gchar *fmt, ...)
{
     gchar *str;

     g_return_if_fail (error != NULL);

     if (fmt != NULL)
     {
          va_list args;

          va_start (args, fmt);
          str = g_strdup_vprintf (fmt, args);
          str = g_strdup_printf (_("Error (%s:%d): %s: %s\n"), g_quark_to_string (error->domain), error->code, str, error->message);
          va_end (args);
     }
     else
          str = g_strdup_printf (_("Error (%s:%d): %s\n"), g_quark_to_string (error->domain), error->code, error->message);

     g_error_free (error);

     if (global.flog) fprintf (global.flog, "%s", str);
     fprintf (stderr, "%s", str);

     if (abort)
          exit (EXIT_FAILURE);
}

/* Function called when exit() is called. */
static void quit (void)
{
     if (gtk_main_level () > 0)
          gtk_main_quit ();

     if (global.sock.channel)
     {
          g_io_channel_shutdown (global.sock.channel, TRUE, NULL);
          close (global.sock.fd);
          unlink (global.sock.path);
     }

     g_hash_table_remove_all (global.protocols);

     lua_ctx_close ();

     if (global.flog)
          fclose (global.flog);

     if (HAVE_DEBUG)
          g_on_error_query (global.prgname);
}

/* Initialize every structures and modules. */
static void init (gchar *config)
{
     GError *error = NULL;
     char *rc = config;

     atexit (quit);

     g_set_prgname ("cream-browser");
     global.prgname = g_get_prgname ();

     /* open log */
     if (global.log || HAVE_DEBUG)
     {
          gchar *path = g_build_filename (g_get_user_config_dir (), global.prgname, PACKAGE ".log", NULL);
          global.flog = fopen (path, "a");
     }

     global.protocols = g_hash_table_new (g_str_hash, g_str_equal);

     /* init modules */
     if (!modules_init (&error))
          print_error (error, TRUE, NULL);

     /* find lua config */
     if (!rc || !g_file_test (rc, G_FILE_TEST_EXISTS))
     {
          if ((rc = find_file (FILE_TYPE_CONFIG, "rc.lua")) == NULL)
          {
               error = g_error_new (CREAM_GLOBAL_ERROR, CREAM_GLOBAL_ERROR_CONFIG, _("Configuration not found."));
               print_error (error, TRUE, NULL);
          }
     }

     /* init socket */
     if (!socket_init (&error))
          print_error (error, TRUE, NULL);

     /* init gui */
     ui_init ();

     /* init and parse lua */
     if (!lua_ctx_init (&error))
          print_error (error, TRUE, NULL);

     if (!lua_ctx_parse (rc, &error))
          print_error (error, TRUE, NULL);

     if (!global.theme)
     {
          error = g_error_new (CREAM_GLOBAL_ERROR, CREAM_GLOBAL_ERROR_THEME, _("Theme wasn't initialized."));
          print_error (error, TRUE, NULL);
     }

}

/* Program used to send commands on the specified socket. */
static void creamctl (const char *cmd)
{
     GError *error = NULL;

     if (global.sock.path && cmd)
     {
          int s, len;
          struct sockaddr_un remote;
          char tmp;

          if ((s = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
          {
               error = g_error_new (CREAM_GLOBAL_ERROR, CREAM_GLOBAL_ERROR_SOCKET, "%s", g_strerror (errno));
               print_error (error, TRUE, "socket");
          }

          remote.sun_family = AF_UNIX;
          strcpy (remote.sun_path, (char *) global.sock.path);
          len = strlen (remote.sun_path) + sizeof (remote.sun_family);

          if (connect (s, (struct sockaddr *) &remote, len) == -1)
          {
               error = g_error_new (CREAM_GLOBAL_ERROR, CREAM_GLOBAL_ERROR_CONNECT, "%s", g_strerror (errno));
               print_error (error, TRUE, "connect");
          }

          if ((send (s, cmd, strlen (cmd), 0) == -1) || (send (s, "\n", 1, 0) == -1))
          {
               error = g_error_new (CREAM_GLOBAL_ERROR, CREAM_GLOBAL_ERROR_SEND, "%s", g_strerror (errno));
               print_error (error, TRUE, "send");
          }

          while ((len = recv (s, &tmp, 1, 0)))
          {
               putchar (tmp);
               if (tmp == '\n')
                    break;
          }

          close (s);

          exit (EXIT_SUCCESS);
     }
     else
     {
          fprintf (stderr, "Usage: cream-browser -s /path/to/socket -c \"command\"\n");
          exit (EXIT_FAILURE);
     }
}

int main (int argc, char **argv)
{
     gchar *url = NULL, *cmd = NULL, *config = NULL;
     gboolean version = FALSE;

     GOptionEntry options[] =
     {
          { "log",     'l', 0, G_OPTION_ARG_NONE,    &global.log,       gettext_noop ("Enable logging"),     NULL },
          { "open",    'o', 0, G_OPTION_ARG_STRING,  &url,              gettext_noop ("Open URL"),           NULL },
          { "config",  'c', 0, G_OPTION_ARG_STRING,  &config,           gettext_noop ("Load an alternate config file."), NULL },
          { "socket",  's', 0, G_OPTION_ARG_STRING,  &global.sock.path, gettext_noop ("Unix socket's path"), NULL },
          { "command", 'e', 0, G_OPTION_ARG_STRING,  &cmd,              gettext_noop ("Send a command on the specified socket (see --socket,-s)"), NULL },
          { "version", 'v', 0, G_OPTION_ARG_NONE,    &version,          gettext_noop ("Show version informations"), NULL },
          { NULL }
     };

     GOptionContext *optctx;
     GError *error = NULL;

     setlocale (LC_ALL, "");
     bindtextdomain (PACKAGE, LOCALEDIR);
     textdomain (PACKAGE);

     /* parse command line */
     optctx = g_option_context_new (" - Web browser");
     g_option_context_add_main_entries (optctx, options, PACKAGE);
     g_option_context_add_group (optctx, gtk_get_option_group (TRUE));

     if (!g_option_context_parse (optctx, &argc, &argv, &error) && error != NULL)
          print_error (error, TRUE, NULL);

     if (version)
     {
          printf (_("%s %s, developped by David Delassus <david.jose.delassus@gmail.com>\n"), PACKAGE, VERSION);
          printf (_("Released under %s license.\n"), LICENSE);

          printf (_("Builded with:\n"));
          printf (" - GLib %s\n", LIB_GLIB_VERSION);
          printf (" - GTK+ %s\n", LIB_GTK_VERSION);
          printf (" - Lua  %s\n\n", LIB_LUA_VERSION);

          printf (_("Builded on %s with:\n"), ARCH);
          printf (" - C Compiler: %s\n", COMPILER);
          printf (" - CFLAGS:     %s\n", CFLAGS);
          printf (" - LDFLAGS:    %s\n", LDFLAGS);

          if (HAVE_DEBUG)
               printf (_("Builded in debug mode.\n"));
          else
               printf (_("Builded in release mode.\n"));

          exit (EXIT_SUCCESS);
     }

     /* if cmd isn't NULL, use creamctl() */
     if (cmd != NULL)
          creamctl (cmd);

     gtk_init (&argc, &argv);

     init (config);

     GtkWidget *nb = notebook_new ();
     GtkWidget *nb2 = notebook_new ();
     GtkWidget *nb3 = notebook_new ();

     notebook_tabopen (NOTEBOOK (nb), "dummy://user:pass@domain.com/path/to/index.html#anchor");
     notebook_tabopen (NOTEBOOK (nb2), "dummy://user2:pas2s@domain2.com/path2/to2/index2.html#anchor2");
     notebook_tabopen (NOTEBOOK (nb3), "dummy://user3:pas3s@domain3.com/path3/to3/index3.html#anchor3");

     gtk_vim_split_add (GTK_VIM_SPLIT (global.gui.vimsplit), nb, GTK_ORIENTATION_VERTICAL);
     gtk_vim_split_add (GTK_VIM_SPLIT (global.gui.vimsplit), nb2, GTK_ORIENTATION_VERTICAL);
     gtk_vim_split_add (GTK_VIM_SPLIT (global.gui.vimsplit), nb3, GTK_ORIENTATION_HORIZONTAL);

     ui_show ();
     gtk_main ();

     return EXIT_SUCCESS;
}

/*! @} */
