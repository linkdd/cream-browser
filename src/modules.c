#include "local.h"

/*!
 * \addtogroup modules
 * @{
 */

static guint domain = 0;
static GList *modules = NULL;

/*!
 * \fn gboolean modules_init (void)
 * @return <code>TRUE</code> if \class{GModule} is supported, <code>FALSE</code> otherwise.
 *
 * Check if \class{GModule} is supported and register the error domain.
 */
gboolean modules_init (void)
{
     if (!g_module_supported ())
          return FALSE;

     domain = error_domain_register ("modules");

     return TRUE;
}

/*!
 * \fn guint modules_load (const char *filename)
 * @param filename Filename of the library to load.
 * @return The module identifier.
 *
 * Open a \class{GModule} object and check if it contains the correct API (see #CreamModule).
 * If the module is alreay loaded, just returns its identifier.
 */
guint modules_load (const char *filename)
{
     CreamModule *m;
     GList *tmp;

     guint id = g_str_hash (filename);

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          m = tmp->data;
          if (m->module_id == id)
               return id;
     }

     /* open module */
     m = g_malloc (sizeof (CreamModule));
     m->module = g_module_open (filename, G_MODULE_BIND_LAZY);
     if (!m->module)
     {
          error_send (domain, ERROR_CRITICAL, "%s: Couldn't open module: %s", filename, g_module_error ());
          g_free (m);
          return -1;
     }

     /* load symbols */
     if (!(g_module_symbol (m->module, "module_init", (gpointer *) &m->init)
          && g_module_symbol (m->module, "module_unload", (gpointer *) &m->unload)
          && g_module_symbol (m->module, "module_webview_new", (gpointer *) &m->webview_new)
          && g_module_symbol (m->module, "module_call", (gpointer *) &m->call))
          || !(m->init != NULL && m->unload != NULL && m->webview_new != NULL && m->call != NULL)
        )
     {
          error_send (domain, ERROR_CRITICAL, "%s: Couldn't load symbols: %s", filename, g_module_error ());
          if (!g_module_close (m->module))
               error_send (domain, ERROR_WARNING, "%s: %s", filename, g_module_error ());
          g_free (m);
          return -1;
     }

     m->modulename = g_strdup (filename);
     m->module_id = id;

     modules = g_list_append (modules, m);

     return id;
}

/*!
 * \fn void modules_unload (guint id)
 * @param id Module's identifier
 *
 * Unload a module
 */
void modules_unload (guint id)
{
     GList *tmp;

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          CreamModule *m = tmp->data;

          if (m->module_id == id)
          {
               if (!g_module_close (m->module))
                    error_send (domain, ERROR_WARNING, "%s: %s", m->modulename, g_module_error ());

               modules = g_list_remove_link (modules, tmp);

               g_free (m->modulename);
               g_free (m);

               break;
          }
     }
}

/*!
 * \fn CreamModule *modules_get (guint id)
 * @param id Module's identifier
 * @return A #CreamModule object.
 *
 * Return the #CreamModule object corresponding to the \a id.
 *
 */
CreamModule *modules_get (guint id)
{
     GList *tmp;

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          CreamModule *m = tmp->data;

          if (m->module_id == id)
               return m;
     }

     return NULL;
}

/*! @} */
