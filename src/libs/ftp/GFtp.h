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

#ifndef __CLASS_G_FTP_H
#define __CLASS_G_FTP_H

#include <glib-object.h>
#include <gnet.h>

#define G_TYPE_FTP            (g_ftp_get_type ())
#define G_FTP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_FTP, GFtp))
#define G_FTP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_FTP, GFtpClass))
#define G_IS_FTP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_FTP))
#define G_IS_FTP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_FTP))
#define G_FTP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_FTP, GFtpClass))

typedef struct _GFtp GFtp;
typedef struct _GftpClass GFtpClass;

struct _GFtp
{
     GObject parent;

     GTcpSocket *socket;

     GIOChannel *iochannel;
     gchar *hostname;
     gint port;
};

struct _GFtpClass
{
     GObjectClass parent;
};

GType g_ftp_get_type (void);

GFtp *g_ftp_new (void);
gboolean g_ftp_connect (GFtp *obj, const gchar *hostname, gint port);

#endif /* __CLASS_G_FTP_H */
