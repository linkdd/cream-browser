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
static void webview_child_signal_connect (WebView *w);

/* signals */
enum
{
     WEBVIEW_LOAD_COMMIT_SIGNAL,
     WEBVIEW_LOAD_CHANGED_SIGNAL,
     WEBVIEW_LOAD_FINISHED_SIGNAL,

     WEBVIEW_URI_CHANGED_SIGNAL,
     WEBVIEW_TITLE_CHANGED_SIGNAL,
     WEBVIEW_STATUS_CHANGED_SIGNAL,

     WEBVIEW_RAISE_SIGNAL,
     WEBVIEW_DOWNLOAD_SIGNAL,

     WEBVIEW_NB_SIGNALS
};

static guint webview_signals[WEBVIEW_NB_SIGNALS] = { 0 };

G_DEFINE_TYPE (WebView, webview, GTK_TYPE_SCROLLED_WINDOW)

/* Constructors */

/*!
 * \public \memberof WebView
 * \fn GtkWidget *webview_new (CreaMModule *mod)
 * @param mod A #CreamModule object.
 * @return A #WebView object.
 *
 * Create a new #WebView.
 */
GtkWidget *webview_new (CreamModule *mod)
{
     WebView *w = g_object_new (CREAM_TYPE_WEBVIEW, NULL);

     g_return_val_if_fail (w != NULL, NULL);
     g_return_val_if_fail (mod != NULL, NULL);

     w->mod = mod;
     w->child = w->mod->webview_new ();
     webview_child_signal_connect (w);

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
     webview_signals[WEBVIEW_LOAD_COMMIT_SIGNAL] = g_signal_new (
               "load-commit",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_commit),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_LOAD_CHANGED_SIGNAL] = g_signal_new (
               "load-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__INT,
               G_TYPE_NONE,
               1, G_TYPE_INT);

     webview_signals[WEBVIEW_LOAD_FINISHED_SIGNAL] = g_signal_new (
               "load-finished",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_finished),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0);

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
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, download),
               NULL, NULL,
               cream_marshal_BOOLEAN__STRING,
               G_TYPE_BOOLEAN,
               1, G_TYPE_STRING);
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
     GtkObjectClass *obj_class = GTK_OBJECT_CLASS (gtk_type_class (webview_get_type ()));
     if (obj_class->destroy)
          obj_class->destroy (GTK_OBJECT (obj));
#endif
}


/* Methods */

/*!
 * \public \memberof WebView
 * \fn CreamModule *webview_get_module (WebView *w)
 * @param w A #WebView object.
 * @return A #CreamModule object.
 *
 * Get the #CreamModule used.
 */
CreamModule *webview_get_module (WebView *w)
{
     g_return_val_if_fail (CREAM_IS_WEBVIEW (w), NULL);
     return w->mod;
}

/*!
 * \public \memberof WebView
 * \fn void webview_set_module (WebView *w, CreamModule *mod)
 * @param w A #WebView object.
 * @param mod A #CreamModule object.
 *
 * Associate a new #CreamModule to the #WebView.
 */
void webview_set_module (WebView *w, CreamModule *mod)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     g_return_if_fail (mod != NULL);

     gtk_container_remove (GTK_CONTAINER (w), w->child);
     gtk_widget_destroy (w->child);
     w->mod = mod;
     w->child = mod->webview_new ();
     webview_child_signal_connect (w);
     gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (w), w->child);
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
          w->mod->call ("load-uri", NULL, w->child, &u, NULL);
     else
     {
          webview_set_module (w, get_protocol (u.scheme));
          w->mod->call ("load-uri", NULL, w->child, &u, NULL);
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

/* Signals */

static void webview_child_signal_load_commit (GtkWidget *child, const char *uri, WebView *w)
{
     if (w->uri) g_free (w->uri);
     w->uri = g_strdup (uri);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_COMMIT_SIGNAL], 0, uri);
}

static void webview_child_signal_load_changed (GtkWidget *child, gint progress, WebView *w)
{
     w->load_status = progress;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_CHANGED_SIGNAL], 0, progress);
}

static void webview_child_signal_load_finished (GtkWidget *child, WebView *w)
{
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_FINISHED_SIGNAL], 0);
}

static void webview_child_signal_uri_changed (GtkWidget *child, const char *uri, WebView *w)
{
     if (w->uri) g_free (w->uri);
     w->uri = g_strdup (uri);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_URI_CHANGED_SIGNAL], 0, uri);
}

static void webview_child_signal_title_changed (GtkWidget *child, const char *title, WebView *w)
{
     if (w->title) g_free (w->title);
     w->title = g_strdup (title);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL], 0, title);
}

static void webview_child_signal_status_changed (GtkWidget *child, const char *status, WebView *w)
{
     if (w->status) g_free (w->status);
     w->status = g_strdup (status);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_STATUS_CHANGED_SIGNAL], 0, status);
}

static gboolean webview_child_signal_download_requested (GtkWidget *child, const char *file_uri, WebView *w)
{
     gboolean ret = FALSE;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_DOWNLOAD_SIGNAL], 0, file_uri, &ret);
     return ret;
}

static void webview_child_signal_connect (WebView *w)
{
     g_return_if_fail (CREAM_IS_WEBVIEW (w));
     g_return_if_fail (w->child != NULL);

     g_signal_connect (G_OBJECT (w->child), "load-commit",        G_CALLBACK (webview_child_signal_load_commit),        w);
     g_signal_connect (G_OBJECT (w->child), "load-changed",       G_CALLBACK (webview_child_signal_load_changed),       w);
     g_signal_connect (G_OBJECT (w->child), "load-finished",      G_CALLBACK (webview_child_signal_load_finished),      w);
     g_signal_connect (G_OBJECT (w->child), "uri-changed",        G_CALLBACK (webview_child_signal_uri_changed),        w);
     g_signal_connect (G_OBJECT (w->child), "title-changed",      G_CALLBACK (webview_child_signal_title_changed),      w);
     g_signal_connect (G_OBJECT (w->child), "status-changed",     G_CALLBACK (webview_child_signal_status_changed),     w);
     g_signal_connect (G_OBJECT (w->child), "download-requested", G_CALLBACK (webview_child_signal_download_requested), w);
}

/*! @} */
