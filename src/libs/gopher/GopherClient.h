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
