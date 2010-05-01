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

Favicon *favicon_new (const gchar *uri);
void favicon_destroy (Favicon *obj);

#endif /* __CLASS_FAVICON_H */
