#ifndef __SOCKET_H
#define __SOCKET_H

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

struct _Socket
{
     GSocket parent;

     GSocketAddress *addr;
     GIOChannel *iochannel;
     gchar *path;

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

#endif /* __SOCKET_H */
