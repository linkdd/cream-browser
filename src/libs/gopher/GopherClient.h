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

/*!
  \file GopherClient.h
  \brief Gopher client header's file
  \author David Delassus
  \todo
 */

#ifndef __CLASS_GOPHER_CLIENT_H
#define __CLASS_GOPHER_CLIENT_H

#include <glib-object.h>
#include <gnet.h>

#define TYPE_GOPHER_CLIENT              (gopher_client_get_type ())
#define GOPHER_CLIENT(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GOPHER_CLIENT, GopherClient))
#define GOPHER_CLIENT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GOPHER_CLIENT, GopherClientClass))
#define IS_GOPHER_CLIENT(obj)           (G_TYPE_INSTANCE_TYPE ((obj), TYPE_GOPHER_CLIENT))
#define IS_GOPHER_CLIENT_CLASS(klass)   (G_TYPE_CLASS_TYPE ((klass), TYPE_GOPHER_CLIENT))
#define GOPHER_CLIENT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GOPHER_CLIENT, GopherClientClass))

typedef struct _GopherClient GopherClient;
typedef struct _GopherClientClass GopherClientClass;

struct _GopherClient
{
     GObject parent;

     GURI *uri;
     gchar *status;
};

struct _GopherClientClass
{
     GObjectClass parent;
     GObject *instance;
};

#endif /* __CLASS_GOPHER_CLIENT_H */
