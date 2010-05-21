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

#ifndef __CLASS_FTP_CLIENT_H
#define __CLASS_FTP_CLIENT_H

/*!
  @addtogroup FtpClient
  @{
 */

#include <glib-object.h>

#define FTP_CLIENT_TYPE            (ftp_client_get_type ())
#define FTP_CLIENT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FTP_CLIENT_TYPE, FtpClient))
#define FTP_CLIENT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), FTP_CLIENT_TYPE, FtpClientClass))
#define IS_FTP_CLIENT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FTP_CLIENT_TYPE))
#define IS_FTP_CLIENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), FTP_CLIENT_TYPE))
#define FTP_CLIENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), FTP_CLIENT_TYPE, FtpClientClass))

/*! \typedef struct _FtpClient FtpClient */
/*! \typedef struct _FtpClientClass FtpClientClass */

typedef struct _FtpClient FtpClient;
typedef struct _FtpClientClass FtpClientClass;

/*!
  \struct _FtpClient
  \todo Implements FTP protocol
 */
struct _FtpClient
{
     GObject parent;

     int sock;                /*!< Socket */
     GIOChannel *iochannel;   /*!< IOChannel associated to the socket */
     gchar *hostname;         /*!< Hostname */
     gint port;               /*!< Port */
};

/*!
  \struct _FtpClientClass
  \todo Add signals (for connection, loading, etc...)
 */
struct _FtpClientClass
{
     GObjectClass parent;
};

GType ftp_client_get_type (void);

FtpClient *ftp_client_new (void);
gboolean ftp_client_connect (FtpClient *obj, const gchar *hostname, gint port);
gboolean ftp_client_login (FtpClient *obj, const gchar *username, const gchar *password);
gboolean ftp_client_set_mode (FtpClient *obj, gint mode);

/* @} */

#endif /* __CLASS_FTP_CLIENT_H */
