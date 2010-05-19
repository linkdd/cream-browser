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
  @defgroup CreamFtp New FTP integration
  @ingroup FTP
  @brief Integration of the FTP protocol without curl

  @{
 */

#include "CreamFtp.h"

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

G_DEFINE_TYPE (CreamFtp, cream_ftp, G_TYPE_OBJECT)

static void cream_ftp_class_init (CreamFtpClass *klass)
{
     g_return_if_fail (klass != NULL);
}

static void cream_ftp_init (CreamFtp *self)
{
     g_return_if_fail (self != NULL);

     self->iochannel = NULL;
     self->hostname  = NULL;
     self->port      = 0;
}

/*!
  \fn CreamFtp *cream_ftp_new (void)
  \brief Create a new CreamFtp object. Connect to a repository with #cream_ftp_connect()
  \return New CreamFtp object
 */
CreamFtp *cream_ftp_new (void)
{
     return CREAM_FTP (g_object_new (CREAM_TYPE_FTP, NULL));
}

/*!
  \fn static gboolean cream_ftp_control_client (GIOChannel *channel)
  \brief Control the client socket.

  This function is a callback initialized in the function
  #cream_ftp_connect().

  \param channel IOChannel associated to the socket
  \return TRUE on success, FALSE if failed

  \todo Parse commands and send response
 */
static gboolean cream_ftp_control_client (GIOChannel *channel)
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
  \fn gboolean cream_ftp_connect (CreamFtp *obj, const gchar *hostname, gint port)
  \brief Connect to a FTP repository

  \param obj CreamFtp object
  \param hostname Hostname of the FTP
  \param port Port of the connection (default 21)
  \return TRUE on success, FALSE if failed

  \todo Send requests to the server (login, etc...)
  \todo Send errors to the browser
 */
gboolean cream_ftp_connect (CreamFtp *obj, const gchar *hostname, gint port)
{
     struct hostent *hostinfo = NULL;
     struct sockaddr_in sin = { 0 };
     int flags;

     g_return_val_if_fail (obj != NULL, FALSE);

     obj->hostname = g_strdup (hostname);
     obj->port     = port;

     if (-1 == (obj->sock = socket (AF_INET, SOCK_STREAM, 0)))
     {
          perror ("socket()");
          return FALSE;
     }

     if (-1 == (flags = fcntl (obj->sock, F_GETFL, 0)))
     {
          perror ("fcntl()");
          close (obj->sock);
          return FALSE;
     }

     /* Make the socket non-blocking */
     if (-1 == fcntl (obj->sock, F_SETFL, flags | O_NONBLOCK))
     {
          perror ("fcntl()");
          close (obj->sock);
          return FALSE;
     }

     /* get host and connect */
     if (NULL == (hostinfo = gethostbyname (hostname)))
     {
          /* send "Unknow host" to the browser */
          close (obj->sock);
          return FALSE;
     }

     sin.sin_addr   = *(struct in_addr *) hostinfo->h_addr;
     sin.sin_port   = htons (port);
     sin.sin_family = AF_INET;

     if (-1 == connect (obj->sock, (struct sockaddr *) &sin, sizeof (struct sockaddr)))
     {
          perror ("connect()");
          close (obj->sock);
          return FALSE;
     }

     /* create IOChannel */
     obj->iochannel = g_io_channel_unix_new (obj->sock);

     g_io_add_watch (obj->iochannel, G_IO_IN | G_IO_HUP, (GIOFunc) cream_ftp_control_client, NULL);
     /* send requests */

     return TRUE;
}

/*! @} */
