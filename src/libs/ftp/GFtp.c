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

/*!
  @defgroup GFtp New FTP integration
  @ingroup FTP
  @brief Integration of the FTP protocol without curl

  @{
 */

#include "GFtp.h"

G_DEFINE_TYPE (GFtp, g_ftp, G_TYPE_OBJECT)

static void g_ftp_class_init (GFtpClass *klass)
{
     g_return_if_fail (klass != NULL);
}

static void g_ftp_init (GFtp *self)
{
     g_return_if_fail (self != NULL);

     self->socket_id = NULL;
     self->iochannel = NULL;
     self->hostname  = NULL;
     self->port      = 0;
}

/*!
  \fn GFtp *g_ftp_new (void)
  \brief Create a new GFtp object. Connect to a repository with #g_ftp_connect()
  \return New GFtp object
 */
GFtp *g_ftp_new (void)
{
     return G_FTP (g_object_new (G_TYPE_FTP, NULL));
}

/*!
  \fn static gboolean g_ftp_control_client (GIOChannel *channel)
  \brief Control the client socket.

  This function is a callback initialized in the function
  #g_ftp_connect().

  \param channel IOChannel associated to the socket
  \return TRUE on success, FALSE if failed

  \todo Parse commands and send response
 */
static gboolean g_ftp_control_client (GIOChannel *channel)
{
     GError *error = NULL;
     GIOStatus ret;

     gchar *line;
     gsize len;

     ret = g_io_channel_read_line (channel, &line, &len, NULL, &error);
     if (ret == G_IO_STATUS_ERROR)
     {
          g_warning ("Error reading socket : %s", error->message);
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
          /* parse command
           * send response
           */
     }

     g_free (line);
     return TRUE;
}

/*!
  \fn gboolean g_ftp_connect (GFtp *obj, const gchar *hostname, gint port)
  \brief Connect to a FTP repository

  \param obj GFtp object
  \param hostname Hostname of the FTP
  \param port Port of the connection (default 21)
  \return TRUE on success, FALSE if failed

  \todo Send commands for login
 */
gboolean g_ftp_connect (GFtp *obj, const gchar *hostname, gint port)
{
     GInetAddr *addr;

     g_return_val_if_fail (obj != NULL, FALSE);

     obj->hostname = g_strdup (hostname);
     obj->port     = port;

     addr = gnet_inetaddr_new (obj->hostname, obj->port);
     g_return_val_if_fail (addr != NULL, FALSE);

     obj->socket = gnet_tcp_socket_new (addr);
     g_return_val_if_fail (obj->socket != NULL, FALSE);

     obj->iochannel = gnet_tcp_socket_get_io_channel (obj->socket);

     g_io_add_watch (obj->iochannel, G_IO_IN | G_IO_HUP, (GIOFunc) g_ftp_control_client, NULL);
     /* send requests */

     return TRUE;
}

/*! @} */
