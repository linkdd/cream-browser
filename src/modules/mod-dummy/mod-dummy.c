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

#include "mod-dummy.h"

/* Initialize function */
static void dummy_init (gpointer data)
{
     printf ("Initialization of dummy.\n");
}

/* Unload function */
static void dummy_unload (gpointer data)
{
     printf ("Unloading of dummy.\n");
}

/* WebView new function */
static GtkWidget *dummy_webview_new (gpointer data)
{
     GtkWidget *ret = gtk_label_new ("dummy module");
     return ret;
}

/* A function callable with the call() function */
static void dummy_load_uri (GList *args, gpointer ret, gpointer data)
{
     GtkWidget *label = NULL;
     UriScheme *uri = NULL;

     g_return_if_fail (g_list_length (args) >= 2);

     label = (GtkWidget *) g_list_nth_data (args, 0);
     uri   = (UriScheme *) g_list_nth_data (args, 1);

     gchar *markup = g_markup_printf_escaped ("<span style=\"normal\">%s</span>\n<span style=\"italic\">%s</span>\n%s",
                                              uri->scheme, uri->userinfo, uri->hostname);

     gtk_label_set_markup (GTK_LABEL (label), markup);

     g_free (markup);
}

/* main program */
void module_main (void)
{
     set_init_func (dummy_init, NULL);
     set_unload_func (dummy_unload, NULL);
     set_webview_new_func (dummy_webview_new, NULL);

     add_call_function ("load-uri", dummy_load_uri, NULL);
}
