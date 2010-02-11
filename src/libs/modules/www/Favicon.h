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
  \file Favicon.h
  \brief Favicon definition
  \author David Delassus
 */

#ifndef __CLASS_FAVICON_H
#define __CLASS_FAVICON_H

#include <gtk/gtk.h>

typedef struct _Favicon Favicon;

/*! \struct _Favicon */
struct _Favicon
{
     gchar *uri;         /*!< URI of the favicon */
     gchar *file;        /*!< File on the disk */
     GdkPixbuf *ico;     /*!< Icon */

     GHashTable *memory; /*!< Memory containing all the favicon previously loaded */
     gchar *cache_path;  /*!< Path to the cache (to save the favicon in) */
};

Favicon *favicon_new (const gchar *uri, const gchar *ico_uri);
void favicon_destroy (Favicon *obj);

#endif /* __CLASS_FAVICON_H */
