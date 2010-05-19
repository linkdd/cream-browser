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

#ifndef __CLASS_CREAM_FTP_H
#define __CLASS_CREAM_FTP_H

/*!
  @addtogroup CreamFtp
  @{
 */

#include <glib-object.h>

#define CREAM_TYPE_FTP            (cream_ftp_get_type ())
#define CREAM_FTP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CREAM_TYPE_FTP, CreamFtp))
#define CREAM_FTP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CREAM_TYPE_FTP, CreamFtpClass))
#define CREAM_IS_FTP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CREAM_TYPE_FTP))
#define CREAM_IS_FTP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CREAM_TYPE_FTP))
#define CREAM_FTP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CREAM_TYPE_FTP, CreamFtpClass))

/*! \typedef struct _CreamFtp CreamFtp */
/*! \typedef struct _CreamFtpClass CreamFtpClass */

typedef struct _CreamFtp CreamFtp;
typedef struct _CreamFtpClass CreamFtpClass;

/*!
  \struct _CreamFtp
  \todo Implements FTP protocol
 */
struct _CreamFtp
{
     GObject parent;

     int sock;                /*!< Socket */
     GIOChannel *iochannel;   /*!< IOChannel associated to the socket */
     gchar *hostname;         /*!< Hostname */
     gint port;               /*!< Port */
};

/*!
  \struct _CreamFtpClass
  \todo Add signals (for connection, loading, etc...)
 */
struct _CreamFtpClass
{
     GObjectClass parent;
};

GType cream_ftp_get_type (void);

CreamFtp *cream_ftp_new (void);
gboolean cream_ftp_connect (CreamFtp *obj, const gchar *hostname, gint port);

/* @} */

#endif /* __CLASS_CREAM_FTP_H */
