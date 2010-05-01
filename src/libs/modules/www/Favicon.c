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
  \file Favicon.c
  \brief Favicon definition
  \author David Delassus
 */

#include <config.h>
#include "Favicon.h"
#include <xdg.h>

static GdkPixbuf *g_load_pixbuf_from_stock (const gchar *icon_name, GtkIconSize size, GError **error)
{
     GError *local_error = NULL;
     GtkIconTheme *theme;
     GtkIconInfo *info;
     GdkPixbuf *icon;

     theme = gtk_icon_theme_get_default ();
     if (NULL == (info = gtk_icon_theme_lookup_icon (theme, icon_name, size, GTK_ICON_LOOKUP_FORCE_SIZE)))
          return NULL;

     icon = gtk_icon_info_load_icon (info, &local_error);
     if (icon == NULL || local_error != NULL)
     {
          g_propagate_error (error, local_error);
          return NULL;
     }

     return icon;
}

static void g_object_maybe_unref (gpointer obj)
{
     if (obj) g_object_unref (obj);
}

static gchar *favicon_get_cached_path (Favicon *obj, const gchar *uri, const gchar *subfolder)
{
     gchar *cache_path;
     gchar *checksum;
     gchar *extension;
     gchar *cached_filename;
     gchar *cached_path;

     if (subfolder)
          cache_path = g_build_filename (obj->cache_path, subfolder, NULL);
     else
          cache_path = obj->cache_path;

     g_mkdir_with_parents (cache_path, 0700);
     checksum = g_compute_checksum_for_string (G_CHECKSUM_MD5, uri, -1);

     extension = g_strrstr (uri, ".");
     cached_filename = g_strdup_printf ("%s%s", checksum, (extension ? extension : ""));
     g_free (checksum);

     cached_path = g_build_filename (cache_path, cached_filename, NULL);
     g_free (cached_filename);

     if (subfolder)
          g_free (cache_path);

     return cached_path;
}

/*!
  \fn Favicon *favicon_new (const gchar *uri)
  \brief Download a favicon and save it in the cache

  \param uri URI of the website
  \return A new Favicon object
 */
Favicon *favicon_new (const gchar *uri)
{
     Favicon *obj;

     if (uri == NULL) return NULL;

     obj = g_malloc (sizeof (Favicon));
     if (obj != NULL)
     {
          obj->uri  = NULL;
          obj->file = NULL;
          obj->ico  = NULL;

          obj->memory = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_maybe_unref);
          obj->cache_path = g_build_filename (get_xdg_var_by_name ("XDG_CACHE_HOME"), PACKAGE, NULL);

          if (g_str_has_prefix (uri, "http://") || g_str_has_prefix (uri, "https://"))
          {
               guint i = 8;

               while (uri[i] != '\0' && uri[i] != '/')
                    i++;

               if (uri[i] == '/')
               {
                    obj->uri = g_strdup (uri);
                    obj->uri[i] = 0;
                    obj->uri = g_strdup_printf ("%s/favicon.ico", obj->uri);
               }
               else
                    obj->uri = g_strdup_printf ("%s/favicon.ico", uri);

               obj->file = favicon_get_cached_path (obj, obj->uri, "icons");

               if (g_hash_table_lookup_extended (obj->memory, obj->file, NULL, (gpointer) &obj->ico))
               {
                    g_free (obj->file);
                    if (obj->ico)
                         g_object_ref (obj->ico);
               }
               else if ((obj->ico = gdk_pixbuf_new_from_file (obj->file, NULL)))
               {
                    g_free (obj->file);
               }
               else
               {
                    gchar *argv[] =
                    {
                         "/usr/bin/wget",
                         obj->uri,
                         "-O",
                         obj->file,
                         NULL
                    };

                    g_spawn_async (NULL, argv, NULL,
                         G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                         NULL, NULL, NULL, NULL);
               }

               g_free (obj->uri);
          }

          if (!obj->ico)
          {
               GError *error = NULL;

               obj->ico = g_load_pixbuf_from_stock (GTK_STOCK_FILE, GTK_ICON_SIZE_MENU, &error);
               if (obj->ico == NULL)
               {
                    if (error != NULL)
                    {
                         g_warning ("Error, can't load pixbuf: %s\n", error->message);
                         g_error_free (error);
                    }

                    return NULL;
               }
          }
          else
          {
               gint w, h;

               gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &w, &h);
               obj->ico = gdk_pixbuf_scale_simple (obj->ico, w, h, GDK_INTERP_BILINEAR);
          }
     }

     return obj;
}

/*!
  \fn void favicon_destroy (Favicon *obj)
  \brief Free memory used by the Favicon object
 */
void favicon_destroy (Favicon *obj)
{
     g_free (obj->uri);
     g_free (obj->file);
     g_free (obj->cache_path);
     g_hash_table_destroy (obj->memory);
     g_free (obj);
}
