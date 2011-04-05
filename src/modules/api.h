#ifndef __API_H
#define __API_H

#include <gtk/gtk.h>
#include <stdarg.h>

#include <WebViewClass.h>
#include <ViewAreaClass.h>
#include <CreamTabClass.h>

#ifdef G_OS_WIN32
     #define CREAM_MODULE_API __declspec(dllexport)
#else
     #define CREAM_MODULE_API __attribute__((visibility("default")))
#endif

#ifndef CREAM_MODULE_API
     #define CREAM_MODULE_API
#endif

typedef void (*ModuleInitFunc) (gpointer);
typedef void (*ModuleUnloadFunc) (gpointer);
typedef GtkWidget * (*ModuleWebViewNewFunc) (gpointer);
typedef void (*ModuleCallFunc) (GList *, gpointer, gpointer);

void set_init_func (ModuleInitFunc fn, gpointer data);
void set_unload_func (ModuleUnloadFunc fn, gpointer data);
void set_webview_new_func (ModuleWebViewNewFunc fn, gpointer data);
void add_call_function (const gchar *name, ModuleCallFunc fn, gpointer data);

#endif /* __API_H */
