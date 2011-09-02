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

static void webview_destroy (GObject *obj);
static void webview_connect_signals (WebView *w);

/* signals */
enum
{
     WEBVIEW_URI_CHANGED_SIGNAL,
     WEBVIEW_TITLE_CHANGED_SIGNAL,
     WEBVIEW_STATUS_CHANGED_SIGNAL,

     WEBVIEW_RAISE_SIGNAL,
     WEBVIEW_DOWNLOAD_SIGNAL,

     WEBVIEW_MODULE_CHANGED_SIGNAL,

     WEBVIEW_NB_SIGNALS
};

static guint webview_signals[WEBVIEW_NB_SIGNALS] = { 0 };

G_DEFINE_TYPE (WebView, webview, GTK_TYPE_SCROLLED_WINDOW)

/* Constructors */

/*!
 * \public \memberof WebView
 * \fn GtkWidget *webview_new (GObject *mod)
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

     gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (w), w->child);

     return GTK_WIDGET (w);
}

static void webview_class_init (WebViewClass *klass)
{
#if GTK_CHECK_VERSION (3, 0, 0)
     typedef void (*DestroyCallback) (GtkWidget *);
     GtkWidgetClass *object_class = (GtkWidgetClass *) klass;
#else
     typedef void (*DestroyCallback) (GtkObject *);
     GtkObjectClass *object_class = (GtkObjectClass *) klass;
#endif

     object_class->destroy = (DestroyCallback) webview_destroy;

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

/* Destructor */

static void webview_destroy (GObject *obj)
{
     WebView *w = CREAM_WEBVIEW (obj);

     gtk_container_remove (GTK_CONTAINER (w), w->child);
     gtk_widget_destroy (w->child);

     if (w->uri) g_free (w->uri);
     if (w->title) g_free (w->title);
     if (w->status) g_free (w->status);

#if !GTK_CHECK_VERSION (3, 0, 0)
     GtkObjectClass *obj_class = GTK_OBJECT_CLASS (gtk_type_class (CREAM_TYPE_WEBVIEW));
     if (obj_class->destroy)
          obj_class->destroy (GTK_OBJECT (obj));
#endif
}


/* Methods */

/*!
 * \public \memberof WebView
 * \fn GObject *webview_get_module (WebView *w)
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
 * \fn void webview_set_module (WebView *w, GObject *mod)
 * @param w A #WebView object.
 * @param mod A #CreamModule object.
 *
 * Associate a new #CreamModule to the #WebView.
 */
void webview_set_module (WebView *w, GObject *mod)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     g_return_if_fail (mod != NULL);

     gtk_widget_destroy (w->child);
     w->mod = mod;
     w->child = cream_module_webview_new (CREAM_MODULE (mod));
     webview_connect_signals (w);
     gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (w), w->child);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_MODULE_CHANGED_SIGNAL], 0, mod);
}

/*!
 * \public \memberof WebView
 * \fn GtkWidget *webview_get_child (WebView *w)
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
 * \fn gboolean webview_has_focus (WebView *w)
 * @param w A #WebView object.
 * @return <code>TRUE</code> if the #WebView is focused, <code>FALSE</code> otherwise.
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
 * \fn void webview_raise (WebView *w)
 * @param w A #WebView object.
 *
 * Give focus to the #WebView.
 */
void webview_raise (WebView *w)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     w->has_focus = TRUE;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_RAISE_SIGNAL], 0);
}

/*!
 * \public \memberof WebView
 * \fn void webview_load_uri (WebView *w, const gchar *uri)
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

     g_return_if_fail (get_protocol (u.scheme) != 0);

     if (w->mod == get_protocol (u.scheme))
          cream_module_load_uri (CREAM_MODULE (w->mod), w->child, &u);
     else
     {
          webview_set_module (w, get_protocol (u.scheme));
          cream_module_load_uri (CREAM_MODULE (w->mod), w->child, &u);
     }
}

/*!
 * \public \memberof WebView
 * \fn const gchar *webview_get_uri (WebView *w)
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
 * \fn const gchar *webview_get_title (WebView *w)
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
 * \fn const gchar *webview_get_status (WebView *w)
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

/* signals */

static void webview_signal_grab_focus_cb (GtkWidget *child, WebView *w)
{
     g_signal_emit_by_name (G_OBJECT (w), "grab-focus");
}

static void webview_signal_uri_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *uri, WebView *w)
{
     if (webview == w->child)
     {
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_URI_CHANGED_SIGNAL], 0, uri);

          if (w->uri) g_free (w->uri);
          w->uri = g_strdup (uri);
     }

     if (GTK_WIDGET (w) == global.gui.fwebview)
          statusbar_set_link (CREAM_STATUSBAR (global.gui.statusbar), uri);
}

static void webview_signal_title_changed_cb (CreamModule *self, GtkWidget *webview, const gchar *title, WebView *w)
{
     if (webview == w->child)
     {
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL], 0, title);

          if (w->title) g_free (w->title);
          w->title = g_strdup (title);
     }

     if (GTK_WIDGET (w) == global.gui.fwebview)
     {
          gchar *title = g_strdup_printf ("%s - %s", PACKAGE, webview_get_title (CREAM_WEBVIEW (w)));
          gtk_window_set_title (GTK_WINDOW (global.gui.window), title);
          g_free (title);
     }
}

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

     if (GTK_WIDGET (w) == global.gui.fwebview)
     {
          statusbar_set_link (CREAM_STATUSBAR (global.gui.statusbar), status);
          statusbar_set_progress (CREAM_STATUSBAR (global.gui.statusbar), progress);
     }
}

static gboolean webview_signal_download_cb (CreamModule *self, GtkWidget *webview, const gchar *file, WebView *w)
{
     gboolean ret = FALSE;

     if (webview == w->child)
          g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_DOWNLOAD_SIGNAL], 0, file, &ret);

     return ret;
}

static void webview_connect_signals (WebView *w)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));

     g_signal_connect (G_OBJECT (w->child), "grab-focus",       G_CALLBACK (webview_signal_grab_focus_cb),       w);
     g_signal_connect (G_OBJECT (w->mod),   "uri-changed",      G_CALLBACK (webview_signal_uri_changed_cb),      w);
     g_signal_connect (G_OBJECT (w->mod),   "title-changed",    G_CALLBACK (webview_signal_title_changed_cb),    w);
     g_signal_connect (G_OBJECT (w->mod),   "progress-changed", G_CALLBACK (webview_signal_progress_changed_cb), w);
     g_signal_connect (G_OBJECT (w->mod),   "download",         G_CALLBACK (webview_signal_download_cb),         w);
}

/*! @} */
