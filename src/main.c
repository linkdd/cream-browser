#include "local.h"

struct global_t global;

/*!
 * \fn char *str_replace (const char *search, const char *replace, const char *string)
 * \brief Replace text into a string
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
 * \fn static void error_callback (guint domain, ErrorLevel level, const char *msg, gpointer data)
 * \brief Callback function to receive all error from every modules.
 */
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

/*!
 * \fn static void quit (int code, void *data)
 * \brief Function called when exit() is called.
 */
static void quit (int code, void *data)
{
     printf ("exit");
     return;
}

/*!
 * \fn static void init (void)
 * \brief Initialize every structures and modules.
 */
static void init (void)
{
     on_exit (quit, NULL);

     global.domain = error_domain_register ("main");
     error_add_callback (error_callback, NULL);

     if (!modules_init ())
          error_send (global.domain, ERROR_FATAL, "GModule isn't supported.");

     libconfig_init ();
     socket_init ();
}

/*!
 * \fn static void creamctl (const char *cmd)
 * \brief Program used to send commands on the specified socket.
 */
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
     gchar *url = NULL, *cmd = NULL;

     GOptionEntry options[] =
     {
          { "log",     'l', 0, G_OPTION_ARG_NONE,    &global.log,       "Enable logging",     NULL },
          { "open",    'o', 0, G_OPTION_ARG_STRING,  &url,              "Open URL",           NULL },
          { "socket",  's', 0, G_OPTION_ARG_STRING,  &global.sock.path, "Unix socket's path", NULL },
          { "command", 'c', 0, G_OPTION_ARG_STRING,  &cmd,              "Send a command on the specified socket (see --socket,-s)", NULL },
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

     /* if cmd isn't NULL, use creamctl() */
     if (cmd != NULL)
          creamctl (cmd);

     init ();

     gtk_main ();

     return EXIT_SUCCESS;
}
