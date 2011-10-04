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

#include "local.h"

gchar *cache_path (CacheType type, const gchar *file)
{
     gchar *ret = NULL;

     switch (type)
     {
          case CACHE_TYPE_COMMANDS:
               ret = g_build_path (G_DIR_SEPARATOR_S, g_get_user_cache_dir (), PACKAGE, app->profile, "commands", NULL);
               break;

          case CACHE_TYPE_HISTORY:
               ret = g_build_path (G_DIR_SEPARATOR_S, g_get_user_cache_dir (), PACKAGE, app->profile, "history", file, NULL);
               break;

          case CACHE_TYPE_SESSION:
               ret = g_build_path (G_DIR_SEPARATOR_S, g_get_user_cache_dir (), PACKAGE, app->profile, "session", file, NULL);
               break;

          case CACHE_TYPE_COOKIES:
               ret = g_build_path (G_DIR_SEPARATOR_S, g_get_user_cache_dir (), PACKAGE, app->profile, "cookies", file, NULL);
               break;

          case CACHE_TYPE_NONE:
          default:
               ret = g_build_path (G_DIR_SEPARATOR_S, g_get_user_cache_dir (), PACKAGE, app->profile, file, NULL);
               break;
     }

     return ret;
}

void cache_appendto (const gchar *path, const gchar *data)
{
     GError *error = NULL;
     GFileOutputStream *cout;
     GFile *f;
     gchar *cleared;

     f = g_file_new_for_path (path);

     if (!g_file_query_exists (f, NULL))
          g_file_make_directory_with_parents (g_file_get_parent (f), NULL, NULL);

     cout = g_file_append_to (f, G_FILE_CREATE_PRIVATE, NULL, &error);

     if (error != NULL)
     {
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
          return;
     }

     cleared = g_strdup_printf ("%s\r\n", data);
     if (g_output_stream_write (G_OUTPUT_STREAM (cout), cleared, strlen (cleared), NULL, &error) == -1 && error != NULL)
     {
          g_free (cleared);
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
          return;
     }
     g_free (cleared);

     if (!g_output_stream_close (G_OUTPUT_STREAM (cout), NULL, &error) && error != NULL)
     {
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
          return;
     }
}

gchar **cache_read (const gchar *path)
{
     GError *error = NULL;
     gchar *contents = NULL;
     gchar **lines = NULL;
     gsize length;

     if (!g_file_get_contents (path, &contents, &length, &error) && error != NULL)
     {
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
          return NULL;
     }

     lines = g_strsplit (contents, "\r\n", -1);
     g_free (contents);
     return lines;
}
