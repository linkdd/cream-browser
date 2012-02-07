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
 * \addtogroup notebook
 * @{
 */

static void notebook_switch_page_cb (Notebook *self, GtkWidget *webview, guint page_num);

static void notebook_signal_title_changed_cb (WebView *webview, const gchar *title, Notebook *obj);
static void notebook_signal_favicon_changed_cb (WebView *webview, GdkPixbuf *favicon, Notebook *obj);
static void notebook_signal_grab_focus_cb (WebView *webview, Notebook *obj);

G_DEFINE_TYPE (Notebook, notebook, GTK_TYPE_NOTEBOOK);

/*!
 * @return A new #Notebook object.
 *
 * Create a new #Notebook object.
 */
GtkWidget *notebook_new (void)
{
     Notebook *obj = g_object_new (notebook_get_type (), NULL);

     g_signal_connect (G_OBJECT (obj), "switch-page", G_CALLBACK (notebook_switch_page_cb), NULL);

     return GTK_WIDGET (obj);
}

/*!
 * @param klass The #Notebook class structure.
 *
 * Initialize #Notebook class.
 */
static void notebook_class_init (NotebookClass *klass)
{
     return;
}

/*!
 * @param obj The #Notebook instance structure.
 *
 * Initialize #Notebook instance.
 */
static void notebook_init (Notebook *obj)
{
     obj->focus = NULL;
     obj->webviews = NULL;

     gtk_notebook_set_scrollable (GTK_NOTEBOOK (obj), TRUE);
     gtk_notebook_popup_enable (GTK_NOTEBOOK (obj));
}

/*! @} */

/*!
 * \defgroup notebook-members Members
 * \ingroup notebook
 *
 * @{
 */

/*!
 * \public \memberof Notebook
 * @param obj A #Notebook object.
 * @return A #WebView object.
 *
 * Return the current focused webview.
 */
GtkWidget *notebook_get_focus (Notebook *obj)
{
     g_return_val_if_fail (CREAM_IS_NOTEBOOK (obj), NULL);
     return obj->focus;
}

/*!
 * \public \memberof Notebook
 * @param obj A #Notebook object.
 * @param url URL to load.
 *
 * Open URL in the current webview.
 */
void notebook_open (Notebook *obj, const gchar *url)
{
     GtkWidget *webview;

     g_return_if_fail (CREAM_IS_NOTEBOOK (obj));
     g_return_if_fail (url != NULL);

     if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (obj)) == 0)
          notebook_tabopen (obj, url);
     else
     {
          webview = gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj),
               gtk_notebook_get_current_page (GTK_NOTEBOOK (obj))
          );

          webview_load_uri (CREAM_WEBVIEW (webview), url);
     }
}

/*!
 * \public \memberof Notebook
 * @param obj A #Notebook object.
 * @param url URL to load.
 *
 * Open URL in a new webview.
 */
void notebook_tabopen (Notebook *obj, const gchar *url)
{
     GObject *module;
     GtkWidget *webview;
     GtkWidget *tablabel;
     UriScheme u;

     g_return_if_fail (CREAM_IS_NOTEBOOK (obj));
     g_return_if_fail (uri_scheme_parse (&u, url));

     module = cream_browser_get_protocol (app, u.scheme);
     webview = webview_new (module);
     CREAM_WEBVIEW (webview)->notebook = GTK_WIDGET (obj);
     webview_load_uri (CREAM_WEBVIEW (webview), url);

     tablabel = g_object_new (CREAM_TYPE_NOTEBOOK_TAB_LABEL, NULL);
     gtk_widget_show_all (tablabel);
     gtk_notebook_append_page_menu (GTK_NOTEBOOK (obj), webview,
               tablabel,
               gtk_label_new ("")
     );

     g_signal_connect (G_OBJECT (webview), "module-changed",  G_CALLBACK (ui_show), NULL);
     g_signal_connect (G_OBJECT (webview), "grab-focus",      G_CALLBACK (notebook_signal_grab_focus_cb), obj);
     g_signal_connect (G_OBJECT (webview), "title-changed",   G_CALLBACK (notebook_signal_title_changed_cb), obj);
     g_signal_connect (G_OBJECT (webview), "favicon-changed", G_CALLBACK (notebook_signal_favicon_changed_cb), obj);

     gtk_widget_grab_focus (webview);
}

/*!
 * \public \memberof Notebook
 * @param obj A #Notebook object.
 * @param page The page to close.
 *
 * Close a tab in the notebook.
 */
void notebook_close (Notebook *obj, gint page)
{
     GtkWidget *webview = g_object_ref (gtk_notebook_get_nth_page (GTK_NOTEBOOK (obj), page));
     GList *node = g_list_find (obj->webviews, webview);
     obj->webviews = g_list_remove_link (obj->webviews, node);
     gtk_notebook_remove_page (GTK_NOTEBOOK (obj), page);

     if (gtk_notebook_get_n_pages (GTK_NOTEBOOK (obj)) == 0)
          gtk_vim_split_close (GTK_VIM_SPLIT (app->gui.vimsplit));
}

/*! @} */

/*!
 * \defgroup notebook-cb Callbacks
 * \ingroup notebook
 * @{
 */

/*!
 * @param self A #Notebook object.
 * @param webview A #WebView object.
 * @param page_num The page number of the #WebView inside the #Notebook.
 *
 * This function handles the signal <code>"switch-page"</code> which is emitted
 * when the user changes the current page.
 * This handler requests the focus on the #Notebook.
 */
static void notebook_switch_page_cb (Notebook *self, GtkWidget *webview, guint page_num)
{
     self->focus = webview;
     g_signal_emit_by_name (G_OBJECT (self), "grab-focus");
}

/*!
 * @param webview A #WebView object.
 * @param title The #WebView's title.
 * @param obj A #Notebook object.
 *
 * This function handles the signal <code>"title-changed"</code> which is emitted
 * when the #WebView's title changed.
 */
static void notebook_signal_title_changed_cb (WebView *webview, const gchar *title, Notebook *obj)
{
     NotebookTabLabel *tablabel = CREAM_NOTEBOOK_TAB_LABEL (gtk_notebook_get_tab_label (GTK_NOTEBOOK (obj), GTK_WIDGET (webview)));
     notebook_tab_label_set_text (tablabel, title);
     gtk_notebook_set_menu_label_text (GTK_NOTEBOOK (obj), GTK_WIDGET (webview), title);
}

/*!
 * @param webview A #WebView object.
 * @param favicon The favicon's \class{GdkPixbuf}.
 * @param obj A #Notebook object.
 *
 * This function handles the signal <code>"favicon-changed"</code> which is emitted
 * when the website's favicon is loaded.
 */
static void notebook_signal_favicon_changed_cb (WebView *webview, GdkPixbuf *favicon, Notebook *obj)
{
     if (favicon == NULL) return;
     NotebookTabLabel *tablabel = CREAM_NOTEBOOK_TAB_LABEL (gtk_notebook_get_tab_label (GTK_NOTEBOOK (obj), GTK_WIDGET (webview)));
     notebook_tab_label_set_pixbuf (tablabel, favicon);
}

/*!
 * @param webview A #WebView object.
 * @param obj A Notebook object.
 *
 * This function handles the signal <code>"grab-focus"</code> which is emitted
 * when a #WebView request the focus.
 * This handler requests the focus on the #Notebook.
 *
 * \see \ref focus-changed
 */
static void notebook_signal_grab_focus_cb (WebView *webview, Notebook *obj)
{
     obj->focus = GTK_WIDGET (webview);
     gtk_notebook_set_current_page (GTK_NOTEBOOK (obj), gtk_notebook_page_num (GTK_NOTEBOOK (obj), obj->focus));
     g_signal_emit_by_name (G_OBJECT (obj), "grab-focus");
}

/*! @} */
