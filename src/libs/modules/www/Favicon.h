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

#ifndef __CLASS_FAVICON_H
#define __CLASS_FAVICON_H

#include <gtk/gtk.h>

typedef struct _Favicon Favicon;

struct _Favicon
{
     gchar *uri;
     gchar *file;
     GdkPixbuf *ico;
     GtkWidget *img;

     GHashTable *memory;
     gchar *cache_path;
};

Favicon *favicon_new (const gchar *uri);
void favicon_destroy (Favicon *obj);

#endif /* __CLASS_FAVICON_H */
