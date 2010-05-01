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

#include "GFtp.h"

G_DEFINE_TYPE (GFtp, g_ftp, G_TYPE_OBJECT)

static void g_ftp_class_init (GFtpClass *klass)
{
     g_return_if_fail (klass != NULL);
}

static void g_ftp_init (GFtp *self)
{
     g_return_if_fail (self != NULL);

     self->conn = NULL;
}

GFtp *g_ftp_new (void)
{
     return G_FTP (g_object_new (G_TYPE_FTP, NULL));
}

static void g_ftp_connect_func (GConn *conn, GConnEvent *event, GFtp *obj)
{
     switch (event->type)
     {
          case GNET_CONN_ERROR:
          case GNET_CONN_CONNECT:
          case GNET_CONN_CLOSE:
          case GNET_CONN_TIMEOUT:
          default:
               break;
     }
}

void g_ftp_connect (GFtp *obj, const gchar *hostname, gint port)
{
     g_return_if_fail (obj != NULL);

     obj->conn = gnet_conn_new (hostname, port, g_ftp_connect_func, obj);
     gnet_conn_connect (obj->conn);
}

