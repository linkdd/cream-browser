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

CreamBrowser *app = NULL;

/*!
 * \addtogroup utils
 * @{
 */

/*!
 * @param type Type of file to find (#FILE_TYPE_CONFIG, #FILE_TYPE_DATA or #FILE_TYPE_CACHE).
 * @param filename Name of the file to find.
 * @return Path to the file, or <code>NULL</code> if not found.
 *
 * Find a file into the user/system directories.
 */
gchar *find_file (guint type, const gchar *filename)
{
     gchar *prgname = g_get_prgname ();
     gchar *ret = NULL;
     int i;

     switch (type)
     {
          case FILE_TYPE_CONFIG:
               ret = g_build_filename (g_get_user_config_dir (), prgname, filename, NULL);

               if (!g_file_test (ret, G_FILE_TEST_EXISTS))
               {
                    const gchar * const *dirs = g_get_system_config_dirs ();

                    g_free (ret), ret = NULL;
                    for (i = 0; dirs[i] != NULL; ++i)
                    {
                         ret = g_build_filename (dirs[i], prgname, filename, NULL);

                         if (g_file_test (ret, G_FILE_TEST_EXISTS))
                              return ret;

                         g_free (ret), ret = NULL;
                    }
               }

               break;

          case FILE_TYPE_DATA:
               ret = g_build_filename (g_get_user_data_dir (), prgname, filename, NULL);

               if (!g_file_test (ret, G_FILE_TEST_EXISTS))
               {
                    const gchar * const *dirs = g_get_system_data_dirs ();

                    g_free (ret), ret = NULL;
                    for (i = 0; dirs[i] != NULL; ++i)
                    {
                         ret = g_build_filename (dirs[i], prgname, filename, NULL);

                         if (g_file_test (ret, G_FILE_TEST_EXISTS))
                              return ret;

                         g_free (ret), ret = NULL;
                    }
               }

               break;

          case FILE_TYPE_CACHE:
               ret = g_build_filename (g_get_user_cache_dir (), prgname, filename, NULL);

               if (g_file_test (ret, G_FILE_TEST_EXISTS))
                    return ret;

               g_free (ret), ret = NULL;
               break;
     }

     return ret;
}

/*!
 * @param search Text to search into the string
 * @param replace Replace \a search by the specified text
 * @param string String to modify
 * @return A newly allocated string.
 *
 * Replace text into a string
 */
char *str_replace (const char *search, const char *replace, const char *string)
{
     gchar **buf;
     char *ret;

     g_return_val_if_fail (string != NULL, NULL);

     buf = g_strsplit (string, search, -1);
     ret = g_strjoinv (replace, buf);
     g_strfreev (buf);

     return ret;
}

/*! @} */

int main (int argc, char **argv)
{
     int status;

     app = cream_browser_new ();
     status = g_application_run (G_APPLICATION (app), argc, argv);

     /* we are not running a command-line part of cream-browser */
     if (!app->cmdline)
     {
          g_object_run_dispose (G_OBJECT (app));
          g_object_unref (app);
     }

     return status;
}
