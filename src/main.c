#include "local.h"

/*!
 * \addtogroup cream-browser
 * @{
 */

/*!
 * \struct Protocol
 * Used for add_protocol() and get_protocol().
 */
struct Protocol
{
     guint id;         /*!< Hashed scheme. (see #UriScheme) */
     CreamModule *mod; /*!< Associated module (see #CreamModule) */
};

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
     struct Protocol *el = malloc (sizeof (struct Protocol));

     el->id  = g_str_hash (scheme);
     el->mod = mod;

     global.protocols = g_list_append (global.protocols, el);
}

/*!
 * \fn void del_protocol (CreamModule *mod)
 * @param mod A #CreamModule object.
 *
 * Delete a module from the list.
 */
void del_protocol (CreamModule *mod)
{
     GList *tmp;

     for (tmp = global.protocols; tmp != NULL; tmp = tmp->next)
     {
          struct Protocol *el = tmp->data;
          if (el->mod == mod)
               global.protocols = g_list_remove (global.protocols, el);
     }
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
     GList *tmp;
     guint id = g_str_hash (scheme);

     for (tmp = global.protocols; tmp != NULL; tmp = tmp->next)
     {
          struct Protocol *el = tmp->data;
          if (el->id == id) return el->mod;
     }

     return NULL;
}

/* Callback function to receive all error from every modules. */
static void error_callback (guint domain, ErrorLevel level, const char *msg, gpointer data)
{
     const char *domainname = error_domain_get (domain);
     FILE *log = stderr;

     if (global.log)
     {
          gchar *path = g_build_path (g_get_home_dir (), ".cache", "cream-browser", "errors.log", NULL);

          log = fopen (path, "w");
          if (log == NULL)
          {
               log = stderr;
               fprintf (log, "Warning: Can't open '%s'\n", path);
          }

          g_free (path);
     }

     switch (level)
     {
          case ERROR_FATAL:
               fprintf (log, "Error fatal (%s): %s\n", domainname, msg);
               exit (EXIT_FAILURE);
               break;

          case ERROR_CRITICAL:
               fprintf (log, "Error (%s): %s\n", domainname, msg);
               break;

          case ERROR_WARNING:
               fprintf (log, "Warning (%s): %s\n", domainname, msg);
               break;
     }
}

/* Function called when exit() is called. */
static void quit (int code, void *data)
{
     if (global.sock.channel)
     {
          g_io_channel_shutdown (global.sock.channel, TRUE, NULL);
          close (global.sock.fd);
     }

     lua_ctx_close ();
}

/* Initialize every structures and modules. */
static void init (gchar *config)
{
     char *rc = config;

     on_exit (quit, NULL);

     g_set_prgname ("cream-browser");
     global.prgname = g_get_prgname ();

     global.domain = error_domain_register ("main");
     error_add_callback (error_callback, NULL);

     if (!modules_init ())
          error_send (global.domain, ERROR_FATAL, "GModule isn't supported.");

     if (!rc || !g_file_test (rc, G_FILE_TEST_EXISTS))
          if ((rc = find_file (FILE_TYPE_CONFIG, "rc.lua")) == NULL)
               error_send (global.domain, ERROR_FATAL, "Configuration not found.");

     socket_init ();

     lua_ctx_init ();
     lua_ctx_parse (rc);

}

/* Program used to send commands on the specified socket. */
static void creamctl (const char *cmd)
{
     if (global.sock.path && cmd)
     {
          int s, len;
          struct sockaddr_un remote;
          char tmp;

          if ((s = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
          {
               perror ("socket");
               exit (EXIT_FAILURE);
          }

          remote.sun_family = AF_UNIX;
          strcpy (remote.sun_path, (char *) global.sock.path);
          len = strlen (remote.sun_path) + sizeof (remote.sun_family);

          if (connect (s, (struct sockaddr *) &remote, len) == -1)
          {
               perror ("connect");
               exit (EXIT_FAILURE);
          }

          if ((send (s, cmd, strlen (cmd), 0) == -1) || (send (s, "\n", 1, 0) == -1))
          {
               perror ("send");
               exit (EXIT_FAILURE);
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
          { "log",     'l', 0, G_OPTION_ARG_NONE,    &global.log,       "Enable logging",     NULL },
          { "open",    'o', 0, G_OPTION_ARG_STRING,  &url,              "Open URL",           NULL },
          { "config",  'c', 0, G_OPTION_ARG_STRING,  &config,           "Load an alternate config file.", NULL },
          { "socket",  's', 0, G_OPTION_ARG_STRING,  &global.sock.path, "Unix socket's path", NULL },
          { "command", 'e', 0, G_OPTION_ARG_STRING,  &cmd,              "Send a command on the specified socket (see --socket,-s)", NULL },
          { "version", 'v', 0, G_OPTION_ARG_NONE,    &version,          "Show version informations", NULL },
          { NULL }
     };

     GOptionContext *optctx;
     GError *error = NULL;

     /* parse command line */
     optctx = g_option_context_new (" - Web browser");
     g_option_context_add_main_entries (optctx, options, PACKAGE);
     g_option_context_add_group (optctx, gtk_get_option_group (TRUE));

     if (!g_option_context_parse (optctx, &argc, &argv, &error) && error != NULL)
     {
          fprintf (stderr, "Option parsing failed: %s\n", error->message);
          g_error_free (error);
          exit (EXIT_FAILURE);
     }

     if (version)
     {
          printf ("%s %s, developped by David Delassus <linkdd@ydb.me>\n", PACKAGE, VERSION);
          printf ("Released under MIT license.\n");

          printf ("Builded with:\n");
          printf (" - GLib %s\n", LIB_GLIB_VERSION);
          printf (" - GTK+ %s\n", LIB_GTK_VERSION);
          printf (" - Lua  %s\n\n", LIB_LUA_VERSION);

          printf ("Builded on %s with:\n", ARCH);
          printf (" - CFLAGS:     %s\n", CFLAGS);
          printf (" - LDFLAGS:    %s\n", LDFLAGS);
          printf (" - PREFIX:     %s\n", PREFIX);
          printf (" - SYSCONFDIR: %s\n\n", SYSCONFDIR);

          if (HAVE_DEBUG)
               printf ("Builded in debug mode.\n");
          else
               printf ("Builded in release mode.\n");

          exit (EXIT_SUCCESS);
     }

     /* if cmd isn't NULL, use creamctl() */
     if (cmd != NULL)
          creamctl (cmd);

     gtk_init (&argc, &argv);

     init (config);

     gtk_main ();

     return EXIT_SUCCESS;
}

/*! @} */
