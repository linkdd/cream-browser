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

void cache_insert (Cache *cache, CacheType ctype, gchar *data)
{
     GError *error = NULL;

     g_mkdir_with_parents (cache->path, 0700);

     switch (ctype)
     {
          case CACHE_TYPE_COMMANDS:
          {
               gchar *path = g_build_filename (cache->path, "commands", NULL);
               GFile *f = g_file_new_for_path (path);
               GFileOutputStream *cout = g_file_append_to (f, G_FILE_CREATE_PRIVATE, NULL, &error);

               if (!cout && error)
               {
                    print_error (error, FALSE, "cache.insert.commands");
                    break;
               }


               g_object_unref (cout);
               g_free (path);
               break;
          }
     }
}
