#include "local.h"

static guint domain = 0;

/* Control the client socket. */
static gboolean control_client_socket (GIOChannel *channel)
{
     GString *result = g_string_new ("\n");
     GError *error = NULL;
     GIOStatus ret;
     char *line;
     gsize len;

     ret = g_io_channel_read_line (channel, &line, &len, NULL, &error);
     if (ret == G_IO_STATUS_ERROR)
     {
          if (error != NULL)
          {
               error_send (domain, ERROR_WARNING, "Error reading UNIX socket '%s': %s", global.sock.path, error->message);
               g_error_free (error);
          }
          g_io_channel_shutdown (channel, TRUE, NULL);

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
          /* parse command */
          result = g_string_append (result, "\r\n");

          ret = g_io_channel_write_chars (channel, result->str, result->len, &len, &error);
          if (ret == G_IO_STATUS_ERROR && error != NULL)
          {
               error_send (domain, ERROR_WARNING, "Error writing UNIX socket '%s': %s", global.sock.path, error->message);
               g_error_free (error);
          }
          g_io_channel_flush (channel, NULL);
     }

     g_string_free (result, TRUE);
     g_free (line);

     return TRUE;
}

/* Control the server socket. */
static gboolean control_socket (GIOChannel *channel)
{
     struct sockaddr_un remote;
     unsigned int t = sizeof (remote);

     int csock;
     GIOChannel *cchannel;

     csock = accept (g_io_channel_unix_get_fd (channel), (struct sockaddr *) &remote, &t);
     if ((cchannel = g_io_channel_unix_new (csock)))
     {
          g_io_add_watch (cchannel, G_IO_IN | G_IO_HUP, (GIOFunc) control_client_socket, cchannel);
     }

     return TRUE;
}

/* Initialize socket. */
void socket_init (void)
{
     struct sockaddr_un sa_un;
     int sock_fd;
     int flags;
     socklen_t n;

     domain = error_domain_register ("socket");

     /* create path */
     global.sock.path = g_strdup_printf ("%s/%s_%d_socket", g_get_tmp_dir (), PACKAGE, getpid ());

     /* create socket */
     strncpy (sa_un.sun_path, global.sock.path, sizeof (sa_un.sun_path) - 1);
     sa_un.sun_family = AF_UNIX;

     if (!(sock_fd = socket (AF_UNIX, SOCK_STREAM, 0)))
     {
          error_send (domain, ERROR_WARNING, "socket(%s) failed: %s", global.sock.path, g_strerror (errno));
          return;
     }

     if ((flags = fcntl (sock_fd, F_GETFL, 0)) == -1)
     {
          error_send (domain, ERROR_WARNING, "fcntl(%s) failed: %s", global.sock.path, g_strerror (errno));
          close (sock_fd);
          return;
     }

     /* make the socket non-blocking */
     if (fcntl (sock_fd, F_SETFL, flags | O_NONBLOCK) == -1)
     {
          error_send (domain, ERROR_WARNING, "fcntl(%s) failed: %s", global.sock.path, g_strerror (errno));
          close (sock_fd);
          return;
     }

     if (bind (sock_fd, (struct sockaddr *) &sa_un, SUN_LEN (&sa_un)) != 0)
     {
          close (sock_fd);
          return;
     }

     n = sizeof (sa_un);
     if (getsockname (sock_fd, (struct sockaddr *) &sa_un, &n))
     {
          close (sock_fd);
          return;
     }

     if (listen (sock_fd, 10))
     {
          close (sock_fd);
          return;
     }

     /* create channel */
     global.sock.channel = g_io_channel_unix_new (sock_fd);
     global.sock.fd = sock_fd;
     g_io_add_watch (global.sock.channel, G_IO_IN | G_IO_HUP, (GIOFunc) control_socket, NULL);
}
