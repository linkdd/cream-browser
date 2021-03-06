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

#ifndef __SOCKET_H
#define __SOCKET_H

/*!
 * \defgroup socket Socket
 * Socket definition.
 * @{
 */

#include <gio/gio.h>
#include <gio/gunixsocketaddress.h>
#include <glib.h>

G_BEGIN_DECLS

#define CREAM_TYPE_SOCKET             (socket_get_type ())
#define CREAM_SOCKET(obj)             (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_SOCKET, Socket))
#define CREAM_IS_SOCKET(obj)          (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_SOCKET))
#define CREAM_SOCKET_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_SOCKET, SocketClass))

typedef struct _Socket Socket;
typedef struct _SocketClass SocketClass;

/*!
 * \class Socket
 * Socket class definition.
 */
struct _Socket
{
     GSocket parent;

     GSocketAddress *addr;  /*!< UNIX socket address */
     GIOChannel *iochannel; /*!< IO Channel to make socket asynchronous */
     gchar *path;           /*!< Path of the socket */

     GError *error;
};

struct _SocketClass
{
     GSocketClass parent;
};

GType socket_get_type (void);
Socket *socket_new (GError **err);

const gchar *socket_get_path (Socket *obj);
GSocketAddress *socket_get_addr (Socket *obj);

G_END_DECLS

/*! @} */

#endif /* __SOCKET_H */
