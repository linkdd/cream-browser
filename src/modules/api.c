#include "api.h"

extern void module_main (void);

struct ModuleCallback
{
     gchar *name;
     gpointer func;
     gpointer data;
};

static GList *mod_call = NULL;
static struct ModuleCallback *mod_init = NULL;
static struct ModuleCallback *mod_unload = NULL;
static struct ModuleCallback *mod_webview_new = NULL;

static GList *webviews = NULL;

static void list_destroy_data (gpointer data)
{
     struct ModuleCallback *el = (struct ModuleCallback *) data;
     g_free (el->name);
     g_free (el);
}

/* Standard API for Cream-Browser */

CREAM_MODULE_API void module_init (void)
{
     module_main ();

     if (mod_init) ((ModuleInitFunc) mod_init->func) (mod_init->data);
}

CREAM_MODULE_API void module_unload (void)
{
     if (mod_unload) ((ModuleUnloadFunc) mod_unload->func) (mod_unload->data);

     /* free all webviews previously created */
     g_list_free_full (webviews, (GDestroyNotify) gtk_widget_destroy);

     /* free memory used by mod_callbacks */
     g_free (mod_init);
     g_free (mod_unload);
     g_free (mod_webview_new);

     g_list_free_full (mod_call, list_destroy_data);
}

CREAM_MODULE_API GtkWidget *module_webview_new (void)
{
     GtkWidget *ret = NULL;

     if (mod_webview_new)
     {
          ret = ((ModuleWebViewNewFunc) mod_webview_new->func) (mod_webview_new->data);
          g_return_val_if_fail (ret, NULL);
          webviews = g_list_append (webviews, ret);
     }

     return ret;
}

CREAM_MODULE_API void module_call (const char *name, gpointer ret, ...)
{
     GList *tmp = NULL;

     for (tmp = mod_call; tmp != NULL; tmp = tmp->next)
     {
          struct ModuleCallback *el = (struct ModuleCallback *) tmp->data;

          if (g_str_equal (el->name, name))
          {
               GList *args = NULL;
               gpointer p;
               va_list ap;

               va_start (ap, ret);
               while ((p = va_arg (ap, gpointer)))
                    args = g_list_append (args, p);
               va_end (ap);

               ((ModuleCallFunc) el->func) (args, ret, el->data);
               break;
          }
     }
}

/* Utilities for the module */

void set_init_func (ModuleInitFunc fn, gpointer data)
{
     if (!mod_init)
          mod_init = g_malloc (sizeof (struct ModuleCallback));

     mod_init->func = fn;
     mod_init->data = data;
}

void set_unload_func (ModuleUnloadFunc fn, gpointer data)
{
     if (!mod_unload)
          mod_unload = g_malloc (sizeof (struct ModuleCallback));

     mod_unload->func = fn;
     mod_unload->data = data;
}

void set_webview_new_func (ModuleWebViewNewFunc fn, gpointer data)
{
     if (!mod_webview_new)
          mod_webview_new = g_malloc (sizeof (struct ModuleCallback));

     mod_webview_new->func = fn;
     mod_webview_new->data = data;
}

void add_call_function (const gchar *name, ModuleCallFunc fn, gpointer data)
{
     struct ModuleCallback *el = NULL;
     GList *tmp = NULL;

     for (tmp = mod_call; tmp != NULL; tmp = tmp->next)
     {
          el = (struct ModuleCallback *) tmp->data;
          if (g_str_equal (name, el->name))
               return;
     }

     el = g_malloc (sizeof (struct ModuleCallback));
     el->name  = g_strdup (name);
     el->func  = fn;
     el->data  = data;

     mod_call = g_list_append (mod_call, el);
}
