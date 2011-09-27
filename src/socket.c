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

static gboolean control_client_socket (GIOChannel *channel);
static gboolean control_socket (GIOChannel *channel, GIOCondition cond, Socket *s);

G_DEFINE_TYPE (Socket, socket, G_TYPE_SOCKET)

/* constructors */
Socket *socket_new (GError **err)
{
     Socket *ret = g_initable_new (CREAM_TYPE_SOCKET,
               NULL, err,
               "family",   G_SOCKET_FAMILY_UNIX,
               "type",     G_SOCKET_TYPE_STREAM,
               "protocol", G_SOCKET_PROTOCOL_DEFAULT,
               NULL);

     if (!g_initable_init (G_INITABLE (ret), NULL, err))
          return NULL;

     g_socket_set_blocking (G_SOCKET (ret), FALSE);

     if (!g_socket_bind (G_SOCKET (ret), ret->addr, TRUE, err))
          return NULL;

     if (!g_socket_listen (G_SOCKET (ret), err))
          return NULL;

     ret->iochannel = g_io_channel_unix_new (g_socket_get_fd (G_SOCKET (ret)));
     g_io_add_watch (ret->iochannel, G_IO_IN | G_IO_HUP, (GIOFunc) control_socket, ret);

     return ret;
}

static void socket_class_init (SocketClass *klass)
{
     return;
}

static void socket_init (Socket *self)
{
     self->path  = g_strdup_printf ("%s%s%s_%d.socket", g_get_tmp_dir (), G_DIR_SEPARATOR_S, PACKAGE, getpid ());
     self->addr  = g_unix_socket_address_new (self->path);
}

/* callbacks */

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
               CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
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
               CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);

          if (error != NULL)
          {
               result = g_string_append (result, error->message);
               g_error_free (error);
          }
          result = g_string_append (result, "\r\n");

          ret = g_io_channel_write_chars (channel, result->str, result->len, &len, &error);
          if (ret == G_IO_STATUS_ERROR && error != NULL)
               CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
          g_io_channel_flush (channel, NULL);
     }

     g_string_free (result, TRUE);
     g_free (line);

     return TRUE;
}

static gboolean control_socket (GIOChannel *channel, GIOCondition cond, Socket *s)
{
     GError *err = NULL;
     GSocket *csock = g_socket_accept (G_SOCKET (s), NULL, &err);
     GIOChannel *ciochannel;

     if (err != NULL)
     {
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, err);
          return FALSE;
     }

     g_return_val_if_fail (csock != NULL, FALSE);

     ciochannel = g_io_channel_unix_new (g_socket_get_fd (csock));
     g_io_add_watch (ciochannel, G_IO_IN | G_IO_HUP, (GIOFunc) control_client_socket, ciochannel);
     return TRUE;
}

/* methods */

const gchar *socket_get_path (Socket *obj)
{
     g_return_val_if_fail (CREAM_IS_SOCKET (obj), NULL);
     return obj->path;
}

GSocketAddress *socket_get_addr (Socket *obj)
{
     g_return_val_if_fail (CREAM_IS_SOCKET (obj), NULL);
     return obj->addr;
}

