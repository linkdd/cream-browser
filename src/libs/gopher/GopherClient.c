/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GopherClient.h"

#define GOPHER_CLIENT_GET_PRIVATE(obj)       (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_GOPHER_CLIENT, GopherClientPrivate))

typedef struct _GopherClientPrivate GopherClientPrivate;
struct _GopherClientPrivate
{
     gchar *hostname;
     gint port;

     GTcpSocket *sock;
     GIOChannel *chan;
     GIOError error;

     gchar buffer[1024];
     gsize length;
};

static GObject *gopher_client_constructor (GType type, guint n_properties, GObjectConstructParam *properties);
static void gopher_client_finalize (GObject *self);

G_DEFINE_TYPE (GopherClient, gopher_client, G_TYPE_OBJECT);

static void gopher_client_class_init (GopherClientClass *class)
{
     GObjectClass *gobject_class = G_OBJECT_CLASS (class);

     g_type_class_add_private (class, sizeof (GopherClientPrivate));

     gobject_class->constructor = gopher_client_constructor;
     gobject_class->finalize    = gopher_client_finalize;
     class->instance = NULL;
}

static void gopher_client_init (GopherClient *self)
{
     GopherClientPrivate *priv = GOPHER_CLIENT_GET_PRIVATE (self);

     priv->hostname = NULL;
     priv->port = 0;
     priv->sock = NULL;
     priv->chan = NULL;
     priv->error = G_IO_ERROR_NONE;
     priv->length = 0;

     self->uri = NULL;
     self->status = NULL;
}

static GObject *gopher_client_constructor (GType type, guint n_properties, GObjectConstructParam *properties)
{
     GopherClientClass *class= NULL;

     class = g_type_class_peek (TYPE_GOPHER_CLIENT);
     if (class->instance == NULL)
     {
          class->instance = G_OBJECT_CLASS (gopher_client_parent_class)->constructor (type, n_properties, properties);
     }
     else
     {
          class->instance = g_object_ref (class->instance);
     }

     return class->instance;
}

static void gopher_client_finalize (GObject *self)
{
     GopherClientClass *class = NULL;

     class = g_type_class_peek (TYPE_GOPHER_CLIENT);
     class->instance = NULL;
     G_OBJECT_CLASS (gopher_client_parent_class)->finalize (self);
}

GopherClient *gopher_client_new (void)
{
     GopherClient *obj = g_object_new (TYPE_GOPHER_CLIENT, NULL);
     return obj;
}

static void *gopher_client_load_uri_thread (void *data)
{
     GopherClient *obj = (GopherClient *) data;
     GopherClientPrivate *priv = GOPHER_CLIENT_GET_PRIVATE (obj);
     GInetAddr *addr;

     priv->hostname = obj->uri->hostname;
     priv->port = 70;

     /* Create the address */
     addr = gnet_inetaddr_new (priv->hostname, priv->port);
     if (!addr)
     {
          /* load error */
          return NULL;
     }

     /* Create socket */
     priv->sock = gnet_tcp_socket_new (addr);
     gnet_inetaddr_delete (addr);

     if (!priv->sock)
     {
          /* load error */
          return NULL;
     }

     priv->chan = gnet_tcp_socket_get_io_channel (priv->sock);

     /* Send command */
     priv->error = gnet_io_channel_writen (priv->chan, priv->buffer, priv->length, &priv->length);
     if (priv->error != G_IO_ERROR_NONE)
     {
          /* load error */
          return NULL;
     }

     /* receive data */
     priv->error = gnet_io_channel_readn (priv->chan, priv->buffer, priv->length, &priv->length);
     if (priv->error != G_IO_ERROR_NONE)
     {
          /* load error */
          return NULL;
     }

     /* load finished */
     return NULL;
}

void gopher_client_load_uri (GopherClient *obj, gchar *uri)
{
     /* load committed */

     obj->uri = gnet_uri_new (uri);

     if (!g_thread_create (&gopher_client_load_uri_thread, obj, FALSE, NULL) != 0)
     {
          /* load error */
     }
}

