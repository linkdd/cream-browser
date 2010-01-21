#include "Favicon.h"

static void g_object_maybe_unref (gpointer obj)
{
     if (obj) g_object_unref (obj);
}

static void favicon_get_cached_path (Favicon *obj, const gchar *subfolder)
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
     checksum = g_compute_checksum_for_string (G_CHECKSUM_MD5, obj->uri, -1);

     extension = g_strrstr (obj->uri, ".");
     cached_filename = g_strdup_printf ("%s%s", checksum, extension ? extension : "");

     g_free (checksum);

     cached_path = g_build_filename (cache_path, cached_filename, NULL);
     g_free (cached_filename);

     if (subfolder)
          g_free (cache_path);

     obj->file = g_strdup (cached_path);
}

Favicon *favicon_new (const gchar *uri)
{
     Favicon *obj;

     g_return_val_if_fail (uri != NULL, NULL);

     obj = g_malloc (sizeof (Favicon));
     if (obj != NULL)
     {
          obj->uri = g_strdup (uri);

          obj->memory = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_maybe_unref);
          obj->cache_path = g_build_filename (g_get_user_cache_dir (), "cream-browser", NULL);
          favicon_get_cached_path (obj, "icons");

          if (g_hash_table_lookup_extended (obj->memory, obj->file, NULL, (gpointer) &obj->img))
          {
               if (obj->img)
                    g_object_ref (obj->img);
          }
          else if ((obj->img = gdk_pixbuf_new_from_file (obj->file, NULL)))
          {
               ;
          }
          else
          {
               gchar *arg_v[] =
               {
                    "/usr/bin/wget",
                    obj->uri,
                    "-O",
                    obj->file,
                    NULL
               };

               g_spawn_async (NULL, arg_v, NULL,
                         G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL,
                         NULL, NULL, NULL, NULL);

               obj->img = gdk_pixbuf_new_from_file (obj->file, NULL);
          }
     }

     return obj;
}

void favicon_destroy (Favicon *obj)
{
     g_free (obj->uri);
     g_free (obj->file);
     g_free (obj->cache_path);
     g_hash_table_destroy (obj->memory);
     g_free (obj);
}
