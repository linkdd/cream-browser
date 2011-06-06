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

static void list_destroy_module (struct ModuleCallback *el)
{
     g_free (el->name);
     g_free (el);
}

static void list_destroy_webview (GtkWidget *w)
{
     gtk_widget_destroy (w);
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
     g_list_foreach (webviews, (GFunc) list_destroy_webview, NULL);
     g_list_free (webviews);

     /* free memory used by mod_callbacks */
     g_free (mod_init);
     g_free (mod_unload);
     g_free (mod_webview_new);

     g_list_foreach (mod_call, (GFunc) list_destroy_module, NULL);
     g_list_free (mod_call);
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
