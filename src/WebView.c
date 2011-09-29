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
 * \addtogroup webview
 * @{
 */

static void webview_destroy (GtkWidget *obj);

static void webview_signal_grab_focus_cb (GtkWidget *child, WebView *w);
static void webview_signal_uri_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *uri, WebView *w);
static void webview_signal_title_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *title, WebView *w);
static void webview_signal_favicon_changed_cb (CreamModule *self, GtkWidget *webview, GdkPixbuf *pixbuf, WebView *w);
static void webview_signal_progress_changed_cb (CreamModule *self, GtkWidget *webview, gdouble progress, WebView *w);
static void webview_signal_state_changed_cb (CreamModule *self, GtkWidget *webview, CreamMode state, WebView *w);
static gboolean webview_signal_download_cb (CreamModule *self, GtkWidget *webview, const gchar *file, WebView *w);

static void webview_connect_signals (WebView *w);

/* signals */
enum
{
     WEBVIEW_URI_CHANGED_SIGNAL,
     WEBVIEW_TITLE_CHANGED_SIGNAL,
     WEBVIEW_FAVICON_CHANGED_SIGNAL,
     WEBVIEW_STATUS_CHANGED_SIGNAL,

     WEBVIEW_RAISE_SIGNAL,
     WEBVIEW_DOWNLOAD_SIGNAL,

     WEBVIEW_MODULE_CHANGED_SIGNAL,

     WEBVIEW_NB_SIGNALS
};

static guint webview_signals[WEBVIEW_NB_SIGNALS] = { 0 };

G_DEFINE_TYPE (WebView, webview, GTK_TYPE_SCROLLED_WINDOW)

/*!
 * @param mod A #CreamModule object.
 * @return A #WebView object.
 *
 * Create a new #WebView.
 */
GtkWidget *webview_new (GObject *mod)
{
     WebView *w = g_object_new (CREAM_TYPE_WEBVIEW, NULL);

     g_return_val_if_fail (w != NULL, NULL);
     g_return_val_if_fail (mod != NULL, NULL);

     w->mod = mod;
     w->child = cream_module_webview_new (CREAM_MODULE (mod));
     webview_connect_signals (w);

     gtk_container_add (GTK_CONTAINER (w), w->child);

     return GTK_WIDGET (w);
}

/*!
 * @param klass The #WebView class structure.
 * Initialize #WebView class.
 */
static void webview_class_init (WebViewClass *klass)
{
     GtkWidgetClass *object_class = (GtkWidgetClass *) klass;

     object_class->destroy = webview_destroy;

     /* signals */
     webview_signals[WEBVIEW_URI_CHANGED_SIGNAL] = g_signal_new (
               "uri-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL] = g_signal_new (
               "title-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, title_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_FAVICON_CHANGED_SIGNAL] = g_signal_new (
               "favicon-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, favicon_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__OBJECT,
               G_TYPE_NONE,
               1, GDK_TYPE_PIXBUF);

     webview_signals[WEBVIEW_STATUS_CHANGED_SIGNAL] = g_signal_new (
               "status-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_RAISE_SIGNAL] = g_signal_new (
               "raise",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, raise),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0);

     webview_signals[WEBVIEW_DOWNLOAD_SIGNAL] = g_signal_new (
               "download-requested",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, download),
               NULL, NULL,
               cream_marshal_BOOLEAN__STRING,
               G_TYPE_BOOLEAN,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_MODULE_CHANGED_SIGNAL] = g_signal_new (
               "module-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, module_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__OBJECT,
               G_TYPE_NONE,
               1, G_TYPE_OBJECT);
}

/*!
 * @param obj The #WebView instance structure.
 * Initialize #WebView instance.
 */
static void webview_init (WebView *obj)
{
     obj->mod   = NULL;
     obj->child = NULL;
     obj->has_focus = FALSE;

     obj->uri    = NULL;
     obj->title  = NULL;
     obj->status = NULL;
     obj->load_status = 0;
}

/*!
 * @param obj A #WebView object.
 * Destroy the #WebView widget.
 */
static void webview_destroy (GtkWidget *obj)
{
     WebView *w = CREAM_WEBVIEW (obj);

     gtk_container_remove (GTK_CONTAINER (w), w->child);
     gtk_widget_destroy (w->child);

     if (w->uri) g_free (w->uri);
     if (w->title) g_free (w->title);
     if (w->status) g_free (w->status);
}

/*! @} */

/*!
 * \defgroup webview-members Members
 * \ingroup webview
 * @{
 */

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return A #CreamModule object.
 *
 * Get the #CreamModule used.
 */
GObject *webview_get_module (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->mod;
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @param mod A #CreamModule object.
 *
 * Associate a new #CreamModule to the #WebView.
 *
 * \see \ref w-module-changed
 */
void webview_set_module (WebView *w, GObject *mod)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     g_return_if_fail (mod != NULL);

     gtk_widget_destroy (w->child);
     w->mod = mod;
     w->child = cream_module_webview_new (CREAM_MODULE (mod));
     webview_connect_signals (w);
     gtk_container_add (GTK_CONTAINER (w), w->child);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_MODULE_CHANGED_SIGNAL], 0, mod);
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return A \class{GtkWidget} object.
 *
 * Get the widget created by the module.
 */
GtkWidget *webview_get_child (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->child;
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return \c TRUE if the #WebView is focused, \c FALSE otherwise.
 *
 * Check if the #WebView is focused.
 */
gboolean webview_has_focus (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), FALSE);
     return w->has_focus;
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 *
 * Give focus to the #WebView.
 *
 * \see \ref w-raise
 */
void webview_raise (WebView *w)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     w->has_focus = TRUE;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_RAISE_SIGNAL], 0);
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @param uri The URI to load.
 *
 * Load a new URI.
 */
void webview_load_uri (WebView *w, const gchar *uri)
{
     UriScheme u;

     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     g_return_if_fail (uri != NULL);

     uri_scheme_parse (&u, uri);

     g_return_if_fail (cream_browser_get_protocol (app, u.scheme) != 0);

     if (w->mod == cream_browser_get_protocol (app, u.scheme))
          cream_module_load_uri (CREAM_MODULE (w->mod), w->child, &u);
     else
     {
          webview_set_module (w, cream_browser_get_protocol (app, u.scheme));
          cream_module_load_uri (CREAM_MODULE (w->mod), w->child, &u);
     }
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return Loaded URI.
 *
 * Get the loaded URI.
 */
const gchar *webview_get_uri (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->uri;
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return Title of the loaded page.
 *
 * Get the title of the loaded page.
 */
const gchar *webview_get_title (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->title;
}

/*!
 * \public \memberof WebView
 * @param w A #WebView object.
 * @return The current status.
 *
 * Get the current page's status.
 */
const gchar *webview_get_status (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->status;
}

/*!
 * \private \memberof WebView
 * @param w A #WebView object
 *
 * Connect callbacks handlers to #CreamModule's signals and child widget's signals.
 */
static void webview_connect_signals (WebView *w)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));

     g_signal_connect (G_OBJECT (w->child), "grab-focus",        G_CALLBACK (webview_signal_grab_focus_cb),        w);
     g_signal_connect (G_OBJECT (w->mod),   "uri-changed",       G_CALLBACK (webview_signal_uri_changed_cb),       w);
     g_signal_connect (G_OBJECT (w->mod),   "title-changed",     G_CALLBACK (webview_signal_title_changed_cb),     w);
     g_signal_connect (G_OBJECT (w->mod),   "favicon-changed",   G_CALLBACK (webview_signal_favicon_changed_cb),   w);
     g_signal_connect (G_OBJECT (w->mod),   "progress-changed",  G_CALLBACK (webview_signal_progress_changed_cb),  w);
     g_signal_connect (G_OBJECT (w->mod),   "state-changed",     G_CALLBACK (webview_signal_state_changed_cb),     w);
     g_signal_connect (G_OBJECT (w->mod),   "download",          G_CALLBACK (webview_signal_download_cb),          w);
}

/*! @} */

/*!
 * \defgroup webview-cb Callbacks
 * \ingroup webview
 * #WebView's child signal handlers.
 * @{
 */

/*!
 * @param child The child widget (owned by the #CreamModule).
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"grab-focus"</code> which is emitted
 * when the child widget requests the focus.
 * This handler requestsa the focus on the #WebView.
 *
 * \see \ref focus-changed
 */
static void webview_signal_grab_focus_cb (GtkWidget *child, WebView *w)
{
     g_signal_emit_by_name (G_OBJECT (w), "grab-focus");
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child widget of a #WebView.
 * @param uri The new URI loaded.
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"uri-changed"</code> which is emitted
 * when the child widget of a #WebView request the loading of a new URI.
 * This handler is able to modify the #Statusbar and emit the signal \ref w-uri-changed.
 */
static void webview_signal_uri_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *uri, WebView *w)
{
     if (webview == w->child)
     {
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_URI_CHANGED_SIGNAL], 0, uri);

          if (w->uri) g_free (w->uri);
          w->uri = g_strdup (uri);
     }

     if (GTK_WIDGET (w) == cream_browser_get_focused_webview (app))
     {
          statusbar_set_link (CREAM_STATUSBAR (app->gui.statusbar), uri);
          statusbar_set_history (CREAM_STATUSBAR (app->gui.statusbar),
                    cream_module_can_go_back (self, webview),
                    cream_module_can_go_forward (self, webview));
     }
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child of a #WebView.
 * @param title The new page's title.
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"title-changed"</code> which is emitted
 * when the loaded page changes its title.
 * This handler is able to modify the toplevel window and emit the signal \ref w-title-changed.
 */
static void webview_signal_title_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *title, WebView *w)
{
     if (webview == w->child)
     {
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL], 0, title);

          if (w->title) g_free (w->title);
          w->title = g_strdup (title);
     }

     if (GTK_WIDGET (w) == cream_browser_get_focused_webview (app))
     {
          gchar *title = g_strdup_printf ("%s - %s", PACKAGE, webview_get_title (CREAM_WEBVIEW (w)));
          gtk_window_set_title (GTK_WINDOW (app->gui.window), title);
          g_free (title);
     }
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child of a #WebView.
 * @param pixbuf The favicon's \class{GdkPixbuf}.
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"favicon-changed"</code> which is emitted
 * when the favicon is loaded.
 * This handler emit the signal \ref w-favicon-changed.
 */
static void webview_signal_favicon_changed_cb (CreamModule *self, GtkWidget *webview, GdkPixbuf *pixbuf, WebView *w)
{
     if (webview == w->child)
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_FAVICON_CHANGED_SIGNAL], 0, pixbuf);
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child of a #WebView.
 * @param progress The load progress.
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"progress-changed"</code> which is emitted
 * on the page's loading.
 * This handler is able to modify the #Statusbar and emit the signal \ref w-status-changed.
 */
static void webview_signal_progress_changed_cb (CreamModule *self, GtkWidget *webview, gdouble progress, WebView *w)
{
     gchar *status = NULL;

     if (progress == 0)
          status = g_strdup (_("Waiting for hostname..."));
     else if (progress == 1)
          status = g_strdup (w->uri);
     else
          status = g_strdup_printf (_("Transfering data from %s..."), w->uri);

     if (webview == w->child)
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_STATUS_CHANGED_SIGNAL], 0, status);
     else
          g_free (status);

     if (GTK_WIDGET (w) == cream_browser_get_focused_webview (app))
     {
          statusbar_set_link (CREAM_STATUSBAR (app->gui.statusbar), status);
          statusbar_set_progress (CREAM_STATUSBAR (app->gui.statusbar), progress);
     }
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child of a #WebView.
 * @param state See #CreamMode.
 * @param w A #WebView object.
 *
 * This function handles the signal <code>"state-changed"</code> which is emitted
 * when the child widget wants to modify the #CreamBrowser's state.
 */
static void webview_signal_state_changed_cb (CreamModule *self, GtkWidget *webview, CreamMode state, WebView *w)
{
     if (GTK_WIDGET (w) == cream_browser_get_focused_webview (app))
          statusbar_set_state (CREAM_STATUSBAR (app->gui.statusbar), state);
}

/*!
 * @param self A #CreamModule object.
 * @param webview The child of a #WebView.
 * @param file File URI to download.
 * @param w A #WebView object.
 * @return \c TRUE if the signal was handled (will stop all other handlers).
 *
 * This function handles the signal <code>"download"</code> which is emitted when
 * the user attempt to start a download (or when the webview can't show the file's
 * content).
 * This handler emit the signal \ref w-download
 */
static gboolean webview_signal_download_cb (CreamModule *self, GtkWidget *webview, const gchar *file, WebView *w)
{
     gboolean ret = FALSE;

     if (webview == w->child)
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_DOWNLOAD_SIGNAL], 0, file, &ret);

     return ret;
}

/*! @} */
