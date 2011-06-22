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

#define CREAM_SOCKET_ERROR         cream_socket_error_quark()

typedef enum
{
     CREAM_SOCKET_ERROR_SOCKET,
     CREAM_SOCKET_ERROR_FCNTL,
     CREAM_SOCKET_ERROR_BIND,
     CREAM_SOCKET_ERROR_SOCKNAME,
     CREAM_SOCKET_ERROR_LISTEN,
     CREAM_SOCKET_ERROR_FAILED
} CreamSocketError;

static GQuark cream_socket_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream.socket");

     return domain;
}

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
               print_error (error, FALSE, _("Error reading UNIX socket '%s'"), global.sock.path);
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
          if (!run_command (line, &error))
          {
               print_error (error, FALSE, _("Error while running command"));
          }

          if (error != NULL)
          {
               result = g_string_append (result, error->message);
               g_error_free (error);
          }
          result = g_string_append (result, "\r\n");

          ret = g_io_channel_write_chars (channel, result->str, result->len, &len, &error);
          if (ret == G_IO_STATUS_ERROR && error != NULL)
               print_error (error, FALSE, _("Error writing UNIX socket '%s'"), global.sock.path);
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
gboolean socket_init (GError **err)
{
     struct sockaddr_un sa_un;
     int sock_fd;
     int flags;
     socklen_t n;

     /* create path */
     global.sock.path = g_strdup_printf ("%s%s%s_%d_socket", g_get_tmp_dir (), G_DIR_SEPARATOR_S, PACKAGE, getpid ());

     /* create socket */
     strncpy (sa_un.sun_path, global.sock.path, sizeof (sa_un.sun_path) - 1);
     sa_un.sun_family = AF_UNIX;

     if (!(sock_fd = socket (AF_UNIX, SOCK_STREAM, 0)))
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_SOCKET,
                       _("socket(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          return FALSE;
     }

     if ((flags = fcntl (sock_fd, F_GETFL, 0)) == -1)
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_FCNTL,
                       _("fcntl(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          close (sock_fd);
          return FALSE;
     }

     /* make the socket non-blocking */
     if (fcntl (sock_fd, F_SETFL, flags | O_NONBLOCK) == -1)
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_FCNTL,
                       _("fcntl(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          close (sock_fd);
          return FALSE;
     }

     if (bind (sock_fd, (struct sockaddr *) &sa_un, SUN_LEN (&sa_un)) != 0)
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_BIND,
                       _("bind(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          close (sock_fd);
          return FALSE;
     }

     n = sizeof (sa_un);
     if (getsockname (sock_fd, (struct sockaddr *) &sa_un, &n))
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_SOCKNAME,
                       _("getsockname(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          close (sock_fd);
          return FALSE;
     }

     if (listen (sock_fd, 10))
     {
          g_set_error (err,
                       CREAM_SOCKET_ERROR,
                       CREAM_SOCKET_ERROR_LISTEN,
                       _("listen(%s) failed: %s"),
                       global.sock.path, g_strerror (errno)
          );
          close (sock_fd);
          return FALSE;
     }

     /* create channel */
     global.sock.channel = g_io_channel_unix_new (sock_fd);
     global.sock.fd = sock_fd;
     g_io_add_watch (global.sock.channel, G_IO_IN | G_IO_HUP, (GIOFunc) control_socket, NULL);

     return TRUE;
}
