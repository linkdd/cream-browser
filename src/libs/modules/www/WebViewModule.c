/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  @defgroup WebView WebKit integration
  @ingroup modules
  @brief The #ModuleWebView is used to show HTTP/HTTPS web-pages

  @{
 */

#include "WebViewModule.h"
#include <marshal.h>
#include <local.h>

#define MODULE_WEB_VIEW_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), module_web_view_get_type (), ModuleWebViewPrivate))

/*! \typedef struct _ModuleWebViewPrivate ModuleWebViewPrivate */
typedef struct _ModuleWebViewPrivate ModuleWebViewPrivate;

/*! \struct _ModuleWebViewPrivate */
struct _ModuleWebViewPrivate
{
     GtkWidget *win;     /*!< The window of the DOM inspector */
};

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
     SWITCH_MODULE_SIGNAL,
     NB_SIGNALS
};

/*!
  \var static guint module_web_view_signals[NB_SIGNALS]
  \brief Signal definitions
 */
static guint module_web_view_signals[NB_SIGNALS] = { 0 };

static void module_web_view_class_init (ModuleWebViewClass *class);
static void module_web_view_init (ModuleWebView *obj);

static WebKitWebView *module_web_view_cb_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, ModuleWebView *obj);
static gboolean module_web_view_cb_close_inspector_win (WebKitWebInspector *inspector, ModuleWebView *obj);
static void module_web_view_cb_destroy_inspector_win (GtkWidget *win, ModuleWebView *obj);
static void module_web_view_cb_title_changed (ModuleWebView *webview, WebKitWebFrame *frame, gchar *title, gpointer data);
static void module_web_view_cb_load_progress_changed (ModuleWebView *webview, gint progress, gpointer data);
static void module_web_view_cb_load_committed (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data);
static void module_web_view_cb_load_finished (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data);
static gboolean module_web_view_cb_navigation (ModuleWebView *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *action, WebKitWebPolicyDecision *decision, gpointer data);
static gboolean module_web_view_cb_mimetype (ModuleWebView *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gchar *mimetype, WebKitWebPolicyDecision *decision, gpointer data);
static gboolean module_web_view_cb_download (ModuleWebView *webview, WebKitDownload *download, gpointer data);
static void module_web_view_cb_hoverlink (ModuleWebView *webview, gchar *title, gchar *link, gpointer data);
static WebKitWebView *module_web_view_cb_create_webview (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data);
static gboolean module_web_view_cb_ready (ModuleWebView *webview, gpointer data);

GtkType module_web_view_get_type (void)
{
     static GtkType module_web_view_type = 0;

     if (!module_web_view_type)
     {
          static const GtkTypeInfo module_web_view_info =
          {
               "ModuleWebView",
               sizeof (ModuleWebView),
               sizeof (ModuleWebViewClass),
               (GtkClassInitFunc) module_web_view_class_init,
               (GtkObjectInitFunc) module_web_view_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          module_web_view_type = gtk_type_unique (WEBKIT_TYPE_WEB_VIEW, &module_web_view_info);
     }

     return module_web_view_type;
}

static void module_web_view_class_init (ModuleWebViewClass *class)
{
     g_type_class_add_private (class, sizeof (ModuleWebViewPrivate));

     module_web_view_signals[URI_CHANGED_SIGNAL] = g_signal_new (
          "uri-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleWebViewClass, uri_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_web_view_signals[NEW_TITLE_SIGNAL] = g_signal_new (
          "new-title",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleWebViewClass, new_title),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_web_view_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleWebViewClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_web_view_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new (
          "new-download",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleWebViewClass, new_download),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__OBJECT,
          G_TYPE_BOOLEAN,
          1, G_TYPE_OBJECT);

     module_web_view_signals[SWITCH_MODULE_SIGNAL] = g_signal_new (
          "switch-module",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleWebViewClass, switch_module),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__STRING,
          G_TYPE_BOOLEAN,
          1, G_TYPE_STRING);
}

static void module_web_view_init (ModuleWebView *obj)
{
     obj->uri = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->jsmsg = NULL;
     obj->inspector = NULL;
     obj->settings = NULL;
     obj->ico = NULL;
}

/*!
  \fn GtkWidget *module_web_view_new (void)
  \brief Create a new ModuleWebView object

  \return A new ModuleWebView
 */
GtkWidget *module_web_view_new (void)
{
     ModuleWebView *obj = gtk_type_new (module_web_view_get_type ());

     obj->settings = webkit_web_settings_new ();
     g_object_set (G_OBJECT (obj->settings), "enable-developer-extras", TRUE, NULL);
     webkit_web_view_set_settings (WEBKIT_WEB_VIEW (obj), obj->settings);

     obj->inspector = webkit_web_view_get_inspector (WEBKIT_WEB_VIEW (obj));
     g_object_set (G_OBJECT (obj->inspector), "javascript-profiling-enabled", TRUE, NULL);
     g_object_connect (G_OBJECT (obj->inspector),
          "signal::inspect-web-view", G_CALLBACK (module_web_view_cb_create_inspector_win), obj,
          "signal::close-window",     G_CALLBACK (module_web_view_cb_close_inspector_win),  obj,
          "signal::finished",         G_CALLBACK (module_web_view_cb_close_inspector_win),  obj,
     NULL);

     g_object_connect (G_OBJECT (obj),
          "signal::title-changed",                          G_CALLBACK (module_web_view_cb_title_changed),         NULL,
          "signal::load-progress-changed",                  G_CALLBACK (module_web_view_cb_load_progress_changed), NULL,
          "signal::load-committed",                         G_CALLBACK (module_web_view_cb_load_committed),        NULL,
          "signal::load-finished",                          G_CALLBACK (module_web_view_cb_load_finished),         NULL,
          "signal::navigation-policy-decision-requested",   G_CALLBACK (module_web_view_cb_navigation),            NULL,
          "signal::mime-type-policy-decision-requested",    G_CALLBACK (module_web_view_cb_mimetype),              NULL,
          "signal::download-requested",                     G_CALLBACK (module_web_view_cb_download),              NULL,
          "signal::hovering-over-link",                     G_CALLBACK (module_web_view_cb_hoverlink),             NULL,
          "signal::create-web-view",                        G_CALLBACK (module_web_view_cb_create_webview),        NULL,
          "signal::web-view-ready",                         G_CALLBACK (module_web_view_cb_ready),                 NULL,
     NULL);

     return GTK_WIDGET (obj);
}

/* WebKit signals */
static WebKitWebView *module_web_view_cb_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, ModuleWebView *obj)
{
     ModuleWebViewPrivate *priv = MODULE_WEB_VIEW_GET_PRIVATE (obj);
     GtkWidget *scroll;
     GtkWidget *page;

     priv->win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (priv->win), g_strconcat (
          "WebInspector - ", webkit_web_view_get_uri (view), NULL)
     );

     scroll = gtk_scrolled_window_new (NULL, NULL);
     gtk_container_add (GTK_CONTAINER (priv->win), scroll);

     page = webkit_web_view_new ();
     gtk_container_add (GTK_CONTAINER (scroll), page);

     gtk_widget_show_all (priv->win);

     g_signal_connect (G_OBJECT (priv->win), "destroy", G_CALLBACK (module_web_view_cb_destroy_inspector_win), obj);

     return WEBKIT_WEB_VIEW (page);
}

static gboolean module_web_view_cb_close_inspector_win (WebKitWebInspector *inspector, ModuleWebView *obj)
{
     ModuleWebViewPrivate *priv = MODULE_WEB_VIEW_GET_PRIVATE (obj);
     if (priv->win != NULL)
          gtk_widget_destroy (priv->win);
     return TRUE;
}

static void module_web_view_cb_destroy_inspector_win (GtkWidget *win, ModuleWebView *obj)
{
     ModuleWebViewPrivate *priv = MODULE_WEB_VIEW_GET_PRIVATE (obj);
     priv->win = NULL;

#if WEBKIT_CHECK_VERSION (1, 1, 17)
     webkit_web_inspector_close (obj->inspector);
#endif
}

static void module_web_view_cb_title_changed (ModuleWebView *webview, WebKitWebFrame *frame, gchar *title, gpointer data)
{
     if (webview->title != NULL)
          g_free (webview->title);
     webview->title = g_strdup (title);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[NEW_TITLE_SIGNAL],
          0,
          webview->title
     );
}

static void module_web_view_cb_load_progress_changed (ModuleWebView *webview, gint progress, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup_printf ("Transfering data from %s (%d%%)", webview->uri, progress);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static void module_web_view_cb_load_committed (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data)
{
     /* update URL */
     if (webview->uri != NULL)
          g_free (webview->uri);
     webview->uri = g_strdup (webkit_web_view_get_uri (WEBKIT_WEB_VIEW (webview)));
     webview->ico = favicon_new (webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[URI_CHANGED_SIGNAL],
          0,
          webview->uri
     );

     /* update status */
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup_printf ("Waiting reply from %s", webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static void module_web_view_cb_load_finished (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup (webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static gboolean module_web_view_cb_navigation (ModuleWebView *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, WebKitWebNavigationAction *action, WebKitWebPolicyDecision *decision, gpointer data)
{
     const gchar *uri = webkit_network_request_get_uri (request);

     if (!g_str_has_prefix (uri, "http://") && !g_str_has_prefix (uri, "https://"))
     {
          gboolean ret = FALSE;

          g_signal_emit (
               G_OBJECT (webview),
               module_web_view_signals[SWITCH_MODULE_SIGNAL],
               0,
               uri,
               &ret
          );

          if (ret)
               webkit_web_policy_decision_ignore (decision);
          else
               webkit_web_policy_decision_use (decision);
     }
     else if (g_str_has_prefix (uri, "javascript:"))
     {
          module_web_view_js_script_execute (webview, &uri[11]);
          webkit_web_policy_decision_ignore (decision);
     }
     else
     {
          webkit_web_policy_decision_use (decision);
     }

     return TRUE;
}

static gboolean module_web_view_cb_mimetype (ModuleWebView *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gchar *mimetype, WebKitWebPolicyDecision *decision, gpointer data)
{
     if (webkit_web_view_can_show_mime_type (WEBKIT_WEB_VIEW (webview), mimetype))
          webkit_web_policy_decision_use (decision);
     else
          webkit_web_policy_decision_download (decision);

     return TRUE;
}

static gboolean module_web_view_cb_download (ModuleWebView *webview, WebKitDownload *download, gpointer data)
{
     gboolean ret = FALSE;

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[NEW_DOWNLOAD_SIGNAL],
          0,
          download,
          &ret
     );

     return ret;
}

static void module_web_view_cb_hoverlink (ModuleWebView *webview, gchar *title, gchar *link, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);

     if (link == NULL)
          webview->status = g_strdup (webview->uri);
     else
          webview->status = g_strdup_printf ("Link: %s", link);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static WebKitWebView *module_web_view_cb_create_webview (ModuleWebView *webview, WebKitWebFrame *frame, gpointer data)
{
     GtkWidget *ret = module_web_view_new ();
     CreamTabbed *creamtabbed = CREAM_TABBED (notebook_append_page ("about:blank"));

     cream_view_set_content (CREAM_VIEW (creamtabbed->creamview), ret);

     return WEBKIT_WEB_VIEW (ret);
}

static gboolean module_web_view_cb_ready (ModuleWebView *webview, gpointer data)
{
     gtk_widget_show_all (GTK_WIDGET (webview));

     webview->uri   = g_strdup (webkit_web_view_get_uri (WEBKIT_WEB_VIEW (webview)));
     webview->title = g_strdup (webkit_web_view_get_title (WEBKIT_WEB_VIEW (webview)));
     webview->ico   = favicon_new (webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[URI_CHANGED_SIGNAL],
          0, webview->uri
     );

     g_signal_emit (
          G_OBJECT (webview),
          module_web_view_signals[NEW_TITLE_SIGNAL],
          0, webview->title
     );

     return TRUE;
}

/* ModuleWebView methods */

/*!
  \fn void module_web_view_load_uri (ModuleWebView *view, const gchar *uri)
  \brief Load new URI

  \param view A ModuleWebView
  \param uri New URI to load
 */
void module_web_view_load_uri (ModuleWebView *view, const gchar *uri)
{
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), uri);
}

/* Execute JavaScript script */
static gchar *jsapi_ref_to_string (JSContextRef context, JSValueRef ref)
{
     JSStringRef string_ref;
     gchar *string;
     size_t length;

     string_ref = JSValueToStringCopy (context, ref, NULL);
     length = JSStringGetMaximumUTF8CStringSize (string_ref);
     string = g_new (gchar, length);
     JSStringGetUTF8CString (string_ref, string, length);
     JSStringRelease (string_ref);

     return string;
}

static void jsapi_evaluate_script (ModuleWebView *view, const gchar *script, gchar **value, gchar **message)
{
     WebKitWebFrame *frame = webkit_web_view_get_main_frame (WEBKIT_WEB_VIEW (view));
     JSGlobalContextRef context = webkit_web_frame_get_global_context (frame);
     JSStringRef str;
     JSValueRef val, exception;

     str = JSStringCreateWithUTF8CString (script);
     val = JSEvaluateScript (context, str, JSContextGetGlobalObject (context), NULL, 0, &exception);

     if (!val)
     {
          if (message != NULL)
               *message = jsapi_ref_to_string (context, exception);
     }
     else
     {
          if (value != NULL)
               *value = jsapi_ref_to_string (context, val);
     }
}

/*!
  \fn void module_web_view_js_script_execute (ModuleWebView *view, const gchar *script)
  \brief Execute JavaScript script

  \param view A ModuleWebView
  \param script Script to execute
 */
void module_web_view_js_script_execute (ModuleWebView *view, const gchar *script)
{
     gchar *value = NULL, *message = NULL;

     g_return_if_fail (script != NULL);

     jsapi_evaluate_script (view, script, &value, &message);

     if (view->jsmsg)
          g_free (view->jsmsg);

     if (message)
     {
          view->jsmsg = g_strdup (message);
          g_free (message);
     }

     if (value)
     {
          view->jsmsg = g_strdup (value);
          g_free (value);
     }
}

/*!
  \fn const gchar *module_web_view_get_uri (ModuleWebView *view)
  \brief Get the URI of the loaded page
 */
const gchar *module_web_view_get_uri (ModuleWebView *view)
{
     return (const gchar *) view->uri;
}

/*!
  \fn const gchar *module_web_view_get_title (ModuleWebView *view)
  \brief Get the title of the loaded page
 */
const gchar *module_web_view_get_title (ModuleWebView *view)
{
     return (const gchar *) view->title;
}

/*!
  \fn const gchar *module_web_view_get_jsmsg (ModuleWebView *view)
  \brief Get the latest JavaScript message
 */
const gchar *module_web_view_get_jsmsg (ModuleWebView *view)
{
     return (const gchar *) view->jsmsg;
}

/*!
  \fn const gchar *module_web_view_get_status (ModuleWebView *view)
  \brief Get the status of the loaded page
 */
const gchar *module_web_view_get_status (ModuleWebView *view)
{
     return (const gchar *) view->status;
}

/*!
  \fn gboolean module_web_view_get_view_source_mode (ModuleWebView *view)
  \brief Check if we are seeing the page's source
 */
gboolean module_web_view_get_view_source_mode (ModuleWebView *view)
{
     return webkit_web_view_get_view_source_mode (WEBKIT_WEB_VIEW (view));
}

/*!
  \fn void module_web_view_set_view_source_mode (ModuleWebView *view, gboolean mode)
  \brief Set the view mode

  \param view A ModuleWebView object
  \param mode TRUE if you want to see the page's source
 */
void module_web_view_set_view_source_mode (ModuleWebView *view, gboolean mode)
{
     webkit_web_view_set_view_source_mode (WEBKIT_WEB_VIEW (view), mode);
}

/*!
  \fn gfloat module_web_view_get_zoom_level (ModuleWebView *view)
  \brief Get the zoom level
 */
gfloat module_web_view_get_zoom_level (ModuleWebView *view)
{
     return webkit_web_view_get_zoom_level (WEBKIT_WEB_VIEW (view));
}

/*!
  \fn void module_web_view_zoom_in (ModuleWebView *view)
  \brief Zoom in the page
 */
void module_web_view_zoom_in (ModuleWebView *view)
{
     webkit_web_view_zoom_in (WEBKIT_WEB_VIEW (view));
}

/*!
  \fn void module_web_view_zoom_out (ModuleWebView *view)
  \brief Zoom out the page
 */
void module_web_view_zoom_out (ModuleWebView *view)
{
     webkit_web_view_zoom_out (WEBKIT_WEB_VIEW (view));
}

/*!
  \fn void module_web_view_set_zoom_level (ModuleWebView *view, gfloat zoom)
  \brief Set the page's zoom level

  \param view A ModuleWebView
  \param zoom The new zoom level
 */
void module_web_view_set_zoom_level (ModuleWebView *view, gfloat zoom)
{
     webkit_web_view_set_zoom_level (WEBKIT_WEB_VIEW (view), zoom);
}

/*!
  \fn WebKitWebInspector *module_web_view_get_web_inspector (ModuleWebView *view)
  \brief Get the DOM inspector of the page
 */
WebKitWebInspector *module_web_view_get_web_inspector (ModuleWebView *view)
{
     return view->inspector;
}

/*!
  \fn WebKitWebSettings *module_web_view_get_settings (ModuleWebView *view)
  \brief Get the settings of the WebKitWebView
 */
WebKitWebSettings *module_web_view_get_settings (ModuleWebView *view)
{
     return view->settings;
}

/*!
  \fn Favicon *module_web_view_get_favicon (ModuleWebView *view)
  \brief Get the page's favicon

  \return A Favicon object containing the page's favicon
 */
Favicon *module_web_view_get_favicon (ModuleWebView *view)
{
     return view->ico;
}

