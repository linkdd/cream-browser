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

#include "../local.h"

/*!
 * \addtogroup mod-dummy
 *
 * @{
 */

CREAM_DEFINE_MODULE (CreamModuleDummy, cream_module_dummy);

static void cream_module_dummy_init (CreamModuleDummy *self)
{
     return;
}

/* methods */

static GtkWidget *cream_module_dummy_webview_new (CreamModule *self)
{
     GtkWidget *ret = gtk_label_new ("dummy module");
     return ret;
}

static void cream_module_dummy_load_uri (CreamModule *self, GtkWidget *webview, UriScheme *uri)
{
     gtk_label_set_text (GTK_LABEL (webview), uri->string);

     g_signal_emit (G_OBJECT (self), cream_module_dummy_signals[SIGNAL_URI_CHANGED], 0, webview, uri->string);
     g_signal_emit (G_OBJECT (self), cream_module_dummy_signals[SIGNAL_TITLE_CHANGED], 0, webview, uri->hostname);
}

static void cream_module_dummy_reload (CreamModule *self, GtkWidget *webview)
{
     return;
}

static void cream_module_dummy_backward (CreamModule *self, GtkWidget *webview)
{
     return;
}

static void cream_module_dummy_forward (CreamModule *self, GtkWidget *webview)
{
     return;
}

static gboolean cream_module_dummy_search (CreamModule *self, GtkWidget *webview, const gchar *text, gboolean forward)
{
     const gchar *lbl = gtk_label_get_text (GTK_LABEL (webview));
     return (strstr (lbl, text) ? TRUE : FALSE);
}

static void cream_module_dummy_proxy (CreamModule *self, const gchar *uri)
{
     return;
}

static void cream_module_dummy_useragent (CreamModule *self, const gchar *ua)
{
     return;
}


/*! @} */
