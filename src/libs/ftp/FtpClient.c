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
  @defgroup FtpClient New FTP integration
  @ingroup FTP
  @brief Integration of the FTP protocol without curl

  @{
 */

#include "FtpClient.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

G_DEFINE_TYPE (FtpClient, ftp_client, G_TYPE_OBJECT)

static void ftp_client_class_init (FtpClientClass *klass)
{
     g_return_if_fail (klass != NULL);
}

static void ftp_client_init (FtpClient *self)
{
     g_return_if_fail (self != NULL);

     self->iochannel = NULL;
     self->hostname  = NULL;
     self->port      = 0;
}

/*!
  \fn FtpClient *ftp_client_new (void)
  \brief Create a new FtpClient object. Connect to a repository with #ftp_client_connect()
  \return New FtpClient object
 */
FtpClient *ftp_client_new (void)
{
     return FTP_CLIENT (g_object_new (FTP_CLIENT_TYPE, NULL));
}

/*!
  \fn static gboolean ftp_client_send_cmd (FtpClient *obj, const gchar *cmd)
  \brief Send a command to the server

  \param obj FTP client
  \param cmd Command to send

  \return TRUE on success, FALSE if fail
 */
static gboolean ftp_client_send_cmd (FtpClient *obj, const gchar *cmd)
{
     gsize written = 0;
     gssize len = strlen (cmd);
     GIOStatus status;

     g_return_val_if_fail (obj != NULL, FALSE);

     status = g_io_channel_write_chars (obj->iochannel, cmd, len, &written, NULL);
     if (status != G_IO_STATUS_NORMAL || len != written)
     {
          g_warning ("FTP: Error while sending command '%s'", cmd);
          return FALSE;
     }

     return TRUE;
}

/*!
  \fn static gboolean ftp_client_control_client (GIOChannel *channel)
  \brief Control the client socket.

  This function is a callback initialized in the function
  #ftp_client_connect().

  \param channel IOChannel associated to the socket
  \return TRUE on success, FALSE if failed

  \todo Parse commands and send response
 */
static gboolean ftp_client_control_client (GIOChannel *channel)
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
  \fn gboolean ftp_client_connect (FtpClient *obj, const gchar *hostname, gint port)
  \brief Connect to a FTP repository

  \param obj FtpClient object
  \param hostname Hostname of the FTP
  \param port Port of the connection (default 21)
  \return TRUE on success, FALSE if failed

  \todo Send errors to the browser
 */
gboolean ftp_client_connect (FtpClient *obj, const gchar *hostname, gint port)
{
     struct hostent *hostinfo = NULL;
     struct sockaddr_in sin = { 0 };
     int flags;

     g_return_val_if_fail (obj != NULL, FALSE);

     obj->hostname = g_strdup (hostname);
     obj->port     = port;

     /* Create Socket */
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
     g_io_add_watch (obj->iochannel, G_IO_IN | G_IO_HUP, (GIOFunc) ftp_client_control_client, NULL);

     return TRUE;
}

/*!
  \fn gboolean ftp_client_login (FtpClient *obj, const gchar *username, const gchar *password)
  \brief Send login informations to the FTP server

  \param obj The FTP client
  \param username Account's username, maybe "anonymous" ?
  \param password Account's password, NULL if no password is needed (for anonymous account)

  \return TRUE on success, FALSE if fail

  \todo Check response
 */
gboolean ftp_client_login (FtpClient *obj, const gchar *username, const gchar *password)
{
     gchar *buf;

     g_return_val_if_fail (obj != NULL, FALSE);
     g_return_val_if_fail (username != NULL, FALSE);

     buf = g_strdup_printf ("USER %s\r\n", username);
     if (!ftp_client_send_cmd (obj, buf))
     {
          g_warning ("FTP: Error: Can't send username.");
          return FALSE;
     }
     g_free (buf);

     if (password != NULL)
     {
          buf = g_strdup_printf ("PASS %s\r\n", password);
          if (!ftp_client_send_cmd (obj, buf))
          {
               g_warning ("FTP: Error: Can't send password.");
               return FALSE;
          }
          g_free (buf);
     }

     return TRUE;
}

/*!
  \fn gboolean ftp_client_set_mode (FtpClient *obj, gint mode)
  \brief Set transfer mode for FTP server

  \param obj FTP client
  \param mode Transfer mode

  \return TRUE on success, FALSE if fail
 */
gboolean ftp_client_set_mode (FtpClient *obj, gint mode)
{
     gchar *buf;

     g_return_val_if_fail (obj != NULL, FALSE);

     buf = g_strdup_printf ("TYPE %c\r\n", mode);
     if (!ftp_client_send_cmd (obj, buf))
     {
          g_warning ("FTP: Error: Can't set mode");
          return FALSE;
     }
     g_free (buf);

     return TRUE;
}

/*! @} */
