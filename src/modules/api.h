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
