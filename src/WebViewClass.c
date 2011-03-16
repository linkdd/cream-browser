#include "local.h"

/*!
 * \addtogroup webview
 * @{
 */

static void webview_class_init (WebViewClass *klass);
static void webview_init (WebView *obj);
static void webview_destroy (GtkObject *obj);

enum
{
     WEBVIEW_LOAD_COMMIT_SIGNAL,
     WEBVIEW_LOAD_CHANGED_SIGNAL,
     WEBVIEW_LOAD_FINISHED_SIGNAL,

     WEBVIEW_URI_CHANGED_SIGNAL,
     WEBVIEW_TITLE_CHANGED_SIGNAL,
     WEBVIEW_STATUS_CHANGED_SIGNAL,

     WEBVIEW_RAISE_SIGNAL,

     WEBVIEW_NB_SIGNALS
};

static guint webview_signals[WEBVIEW_NB_SIGNALS] = { 0 };

/*!
 * \public \memberof WebView
 * \fn GtkType webview_get_type (void)
 * @return A \class{GtkType} object.
 *
 * Return informations about the object's type #WebView.
 */
GtkType webview_get_type (void)
{
     static GtkType webview_type = 0;

     if (!webview_type)
     {
          static const GtkTypeInfo webview_type_info = {
               "WebView",
               sizeof (WebView),
               sizeof (WebViewClass),
               (GtkClassInitFunc)  webview_class_init,
               (GtkObjectInitFunc) webview_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };
          webview_type = gtk_type_unique (GTK_TYPE_SCROLLED_WINDOW, &webview_type_info);
     }

     return webview_type;
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
     return (w ? w->mod : NULL);
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
     if (w && mod)
     {
          gtk_container_remove (GTK_CONTAINER (w), w->child);
          gtk_widget_destroy (w->child);
          w->mod = mod;
          w->child = mod->webview_new ();
          gtk_container_add (GTK_CONTAINER (w), w->child);
     }
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
     return (w ? w->child : NULL);
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
     return (w ? w->has_focus : FALSE);
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
     WebView *ow = WEB_VIEW (global.gui.webview);

     g_return_if_fail (w);

     ow->has_focus = FALSE;
     w->has_focus = TRUE;
     global.gui.webview = GTK_WIDGET (w);

     gtk_widget_hide (GTK_WIDGET (ow));
     gtk_widget_show_all (GTK_WIDGET (w));

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

     g_return_if_fail (w && uri);

     uri_scheme_parse (&u, uri);

     g_return_if_fail (get_protocol (u.proto) != 0);

     if (w->mod == get_protocol (u.proto))
          w->mod->call ("load-uri", w, &u);
     else
     {
          webview_set_module (w, get_protocol (u.proto));
          w->mod->call ("load-uri", w, &u);
     }

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_COMMIT_SIGNAL], 0);
}

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
     WebView *w = gtk_type_new (webview_get_type ());

     g_return_val_if_fail (w != NULL, NULL);

     w->mod = mod;
     if (mod)
          w->child = w->mod->webview_new ();

     gtk_container_add (GTK_CONTAINER (w), w->child);

     return GTK_WIDGET (w);
}

static void webview_class_init (WebViewClass *klass)
{
     GtkObjectClass *object_class = (GtkObjectClass *) klass;

     object_class->destroy = webview_destroy;

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

}

static void webview_init (WebView *obj)
{
     obj->mod   = NULL;
     obj->child = NULL;
     obj->has_focus = FALSE;
}

/* Destructor */

static void webview_destroy (GtkObject *obj)
{
     GtkObjectClass *object_class;
     WebView *w;

     g_return_if_fail (obj);
     g_return_if_fail (IS_WEB_VIEW (obj));
     w = WEB_VIEW (obj);

     gtk_container_remove (GTK_CONTAINER (w), w->child);
     gtk_widget_destroy (w->child);

     object_class = GTK_OBJECT_CLASS (gtk_type_class (webview_get_type ()));
     if (object_class->destroy)
          object_class->destroy (obj);
}

/*! @} */
