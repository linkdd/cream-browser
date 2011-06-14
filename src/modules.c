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

/*!
 * \addtogroup modules
 * @{
 */

#define CREAM_MODULE_ERROR         cream_module_error_quark()

typedef enum
{
     CREAM_MODULE_ERROR_SUPPORTED,
     CREAM_MODULE_ERROR_OPEN,
     CREAM_MODULE_ERROR_SYMBOLS,
     CREAM_MODULE_ERROR_CLOSE,
     CREAM_MODULE_ERROR_FAILED
} CreamModuleError;

static GQuark cream_module_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream.modules");

     return domain;
}

static GList *modules = NULL;

/*!
 * \fn gboolean modules_init (GError **err)
 * @return <code>TRUE</code> if \class{GModule} is supported, <code>FALSE</code> otherwise.
 *
 * Check if \class{GModule} is supported.
 */
gboolean modules_init (GError **err)
{
     if (!g_module_supported ())
     {
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_SUPPORTED,
                       _("GModule isn't supported.")
          );

          return FALSE;
     }

     return TRUE;
}

/*!
 * \fn CreamModule *modules_load (const char *filename, GError **err)
 * @param filename Filename of the library to load.
 * @param err A \class{GError} pointer in order to follow possible errors.
 * @return A #CreamModule object.
 *
 * Open a \class{GModule} object and check if it contains the correct API (see #CreamModule).
 * If the module is alreay loaded, just returns it.
 */
CreamModule *modules_load (const char *filename, GError **err)
{
     CreamModule *m;
     GList *tmp;

     guint id = g_str_hash (filename);

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          m = tmp->data;
          if (m->module_id == id)
               return m;
     }

     /* open module */
     m = g_malloc (sizeof (CreamModule));

     m->module = g_module_open (g_strdup_printf ("libcreammod-%s.so", filename), G_MODULE_BIND_LAZY);
     if (!m->module)
     {
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_OPEN,
                       _("%s: Couldn't open module: %s"),
                       filename, g_module_error ()
          );
          return NULL;
     }

     /* load symbols */
     if (!(g_module_symbol (m->module, "module_init", (gpointer *) &m->init)
          && g_module_symbol (m->module, "module_unload", (gpointer *) &m->unload)
          && g_module_symbol (m->module, "module_webview_new", (gpointer *) &m->webview_new)
          && g_module_symbol (m->module, "module_call", (gpointer *) &m->call))
          || !(m->init != NULL && m->unload != NULL && m->webview_new != NULL && m->call != NULL)
        )
     {
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_SYMBOLS,
                       _("%s: Couldn't load symbols: %s"),
                       filename, g_module_error ()
          );

          if (!g_module_close (m->module))
               g_warning ("%s: %s", filename, g_module_error ());

          return NULL;
     }

     m->init ();

     m->modulename = g_strdup (filename);
     m->module_id = id;

     modules = g_list_append (modules, m);

     return m;
}

/*!
 * \fn void modules_unload (CreamModule *mod)
 * @param mod A #CreamModule object.
 *
 * Unload a module
 */
void modules_unload (CreamModule *mod)
{
     GList *tmp;

     g_return_if_fail (mod);

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          CreamModule *m = tmp->data;

          if (m->module_id == mod->module_id)
          {
               m->unload ();

               if (!g_module_close (m->module))
                    g_warning ("%s: %s", m->modulename, g_module_error ());

               modules = g_list_remove_link (modules, tmp);

               g_free (m->modulename);
               break;
          }
     }
}

/*! @} */
