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
  @defgroup About Module 'About'
  @ingroup modules
  @brief Internal browser protocole 'about:' to show history/bookmarks/etc...

  @{
 */

#include "AboutModule.h"
#include <marshal.h>
#include "local.h"

#define MODULE_ABOUT_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), module_about_get_type (), ModuleAboutPrivate))

/*! \typedef struct _ModuleAboutPrivate ModuleAboutPrivate */
typedef struct _ModuleAboutPrivate ModuleAboutPrivate;

/*! \struct _ModuleAboutPrivate */
struct _ModuleAboutPrivate
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
  \var static guint module_about_signals[NB_SIGNALS]
  \brief Signal definitions
 */
static guint module_about_signals[NB_SIGNALS];

static void module_about_class_init (ModuleAboutClass *class);
static void module_about_init (ModuleAbout *obj);

static WebKitWebView *module_about_cb_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, ModuleAbout *obj);
static gboolean module_about_cb_close_inspector_win (WebKitWebInspector *inspector, ModuleAbout *obj);
static void module_about_cb_destroy_inspector_win (GtkWidget *win, ModuleAbout *obj);
static void module_about_cb_title_changed (ModuleAbout *webview, WebKitWebFrame *frame, gchar *title, gpointer data);
static void module_about_cb_load_progress_changed (ModuleAbout *webview, gint progress, gpointer data);
static void module_about_cb_load_committed (ModuleAbout *webview, WebKitWebFrame *frame, gpointer data);
static void module_about_cb_load_finished (ModuleAbout *webview, WebKitWebFrame *frame, gpointer data);
static WebKitNavigationResponse module_about_cb_navigation (ModuleAbout *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gpointer data);
static gboolean module_about_cb_mimetype (ModuleAbout *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gchar *mimetype, WebKitWebPolicyDecision *decision, gpointer data);
static gboolean module_about_cb_download (ModuleAbout *webview, WebKitDownload *download, gpointer data);
static void module_about_cb_hoverlink (ModuleAbout *webview, gchar *title, gchar *link, gpointer data);

GtkType module_about_get_type (void)
{
     static GtkType module_about_type = 0;

     if (!module_about_type)
     {
          static const GtkTypeInfo module_about_info =
          {
               "ModuleAbout",
               sizeof (ModuleAbout),
               sizeof (ModuleAboutClass),
               (GtkClassInitFunc) module_about_class_init,
               (GtkObjectInitFunc) module_about_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          module_about_type = gtk_type_unique (WEBKIT_TYPE_WEB_VIEW, &module_about_info);
     }

     return module_about_type;
}

static void module_about_class_init (ModuleAboutClass *class)
{
     g_type_class_add_private (class, sizeof (ModuleAboutPrivate));

     module_about_signals[URI_CHANGED_SIGNAL] = g_signal_new (
          "uri-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, uri_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[NEW_TITLE_SIGNAL] = g_signal_new (
          "new-title",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, new_title),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new (
          "new-download",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, new_download),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__OBJECT,
          G_TYPE_BOOLEAN,
          1, G_TYPE_OBJECT);

     module_about_signals[SWITCH_MODULE_SIGNAL] = g_signal_new (
          "switch-module",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, switch_module),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__STRING,
          G_TYPE_BOOLEAN,
          1, G_TYPE_STRING);
}

static void module_about_init (ModuleAbout *obj)
{
     obj->uri = NULL;
     obj->title = NULL;
     obj->status = NULL;
}

/*!
  \fn GtkWidget *module_about_new (void)
  \brief Create a new #ModuleAbout object

  \return A new #ModuleAbout
 */
GtkWidget *module_about_new (void)
{
     ModuleAbout *obj = gtk_type_new (module_about_get_type ());

     obj->settings = webkit_web_settings_new ();
     g_object_set (G_OBJECT (obj->settings), "enable-developer-extras", TRUE, NULL);
     webkit_web_view_set_settings (WEBKIT_WEB_VIEW (obj), obj->settings);

     obj->inspector = webkit_web_view_get_inspector (WEBKIT_WEB_VIEW (obj));
     g_object_set (G_OBJECT (obj->inspector), "javascript-profiling-enabled", TRUE, NULL);
     g_object_connect (G_OBJECT (obj->inspector),
          "signal::inspect-web-view", G_CALLBACK (module_about_cb_create_inspector_win), obj,
          "signal::close-window",     G_CALLBACK (module_about_cb_close_inspector_win),  obj,
          "signal::finished",         G_CALLBACK (module_about_cb_close_inspector_win),  obj,
     NULL);

     g_object_connect (G_OBJECT (obj),
          "signal::title-changed",                          G_CALLBACK (module_about_cb_title_changed),         NULL,
          "signal::load-progress-changed",                  G_CALLBACK (module_about_cb_load_progress_changed), NULL,
          "signal::load-committed",                         G_CALLBACK (module_about_cb_load_committed),        NULL,
          "signal::load-finished",                          G_CALLBACK (module_about_cb_load_finished),         NULL,
          "signal::navigation-requested",                   G_CALLBACK (module_about_cb_navigation),            NULL,
          "signal::mime-type-policy-decision-requested",    G_CALLBACK (module_about_cb_mimetype),              NULL,
          "signal::download-requested",                     G_CALLBACK (module_about_cb_download),              NULL,
          "signal::hovering-over-link",                     G_CALLBACK (module_about_cb_hoverlink),             NULL,
     NULL);

     return GTK_WIDGET (obj);
}

/* WebKit signals */
static WebKitWebView *module_about_cb_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, ModuleAbout *obj)
{
     ModuleAboutPrivate *priv = MODULE_ABOUT_GET_PRIVATE (obj);
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

     g_signal_connect (G_OBJECT (priv->win), "destroy", G_CALLBACK (module_about_cb_destroy_inspector_win), obj);

     return WEBKIT_WEB_VIEW (page);
}

static gboolean module_about_cb_close_inspector_win (WebKitWebInspector *inspector, ModuleAbout *obj)
{
     ModuleAboutPrivate *priv = MODULE_ABOUT_GET_PRIVATE (obj);
     if (priv->win != NULL)
          gtk_widget_destroy (priv->win);
     return TRUE;
}

static void module_about_cb_destroy_inspector_win (GtkWidget *win, ModuleAbout *obj)
{
     ModuleAboutPrivate *priv = MODULE_ABOUT_GET_PRIVATE (obj);
     priv->win = NULL;

#if WEBKIT_CHECK_VERSION (1, 1, 17)
     webkit_web_inspector_close (obj->inspector);
#endif
}

static void module_about_cb_title_changed (ModuleAbout *webview, WebKitWebFrame *frame, gchar *title, gpointer data)
{
     if (webview->title != NULL)
          g_free (webview->title);
     webview->title = g_strdup (title);

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[NEW_TITLE_SIGNAL],
          0,
          webview->title
     );
}

static void module_about_cb_load_progress_changed (ModuleAbout *webview, gint progress, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup_printf ("Transfering data from %s (%d%%)", webview->uri, progress);

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static void module_about_cb_load_committed (ModuleAbout *webview, WebKitWebFrame *frame, gpointer data)
{
     /* update URL */
     if (webview->uri != NULL)
          g_free (webview->uri);
     webview->uri = g_strdup (webkit_web_view_get_uri (WEBKIT_WEB_VIEW (webview)));

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[URI_CHANGED_SIGNAL],
          0,
          webview->uri
     );

     /* update status */
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup_printf ("Waiting reply from %s", webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static void module_about_cb_load_finished (ModuleAbout *webview, WebKitWebFrame *frame, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);
     webview->status = g_strdup (webview->uri);

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}

static WebKitNavigationResponse module_about_cb_navigation (ModuleAbout *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gpointer data)
{
     const gchar *uri = webkit_network_request_get_uri (request);

     if (!g_str_has_prefix (uri, "about:"))
     {
          gboolean ret = FALSE;

          g_signal_emit (
               G_OBJECT (webview),
               module_about_signals[SWITCH_MODULE_SIGNAL],
               0,
               uri,
               &ret
          );

          return (ret ? WEBKIT_NAVIGATION_RESPONSE_IGNORE : WEBKIT_NAVIGATION_RESPONSE_ACCEPT);
     }
     else if (g_str_has_prefix (uri, "javascript:"))
     {
          return WEBKIT_NAVIGATION_RESPONSE_IGNORE;
     }
     else
     {
          return WEBKIT_NAVIGATION_RESPONSE_ACCEPT;
     }
}

static gboolean module_about_cb_mimetype (ModuleAbout *webview, WebKitWebFrame *frame, WebKitNetworkRequest *request, gchar *mimetype, WebKitWebPolicyDecision *decision, gpointer data)
{
     if (webkit_web_view_can_show_mime_type (WEBKIT_WEB_VIEW (webview), mimetype))
          webkit_web_policy_decision_use (decision);
     else
          webkit_web_policy_decision_download (decision);

     return TRUE;
}

static gboolean module_about_cb_download (ModuleAbout *webview, WebKitDownload *download, gpointer data)
{
     gboolean ret = FALSE;

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[NEW_DOWNLOAD_SIGNAL],
          0,
          download,
          &ret
     );

     return ret;
}

static void module_about_cb_hoverlink (ModuleAbout *webview, gchar *title, gchar *link, gpointer data)
{
     if (webview->status != NULL)
          g_free (webview->status);

     if (link == NULL)
          webview->status = g_strdup (webview->uri);
     else
          webview->status = g_strdup_printf ("Link: %s", link);

     g_signal_emit (
          G_OBJECT (webview),
          module_about_signals[STATUS_CHANGED_SIGNAL],
          0,
          webview->status
     );
}


/*!
  \fn void module_about_load_uri (ModuleAbout *view, const gchar *uri)
  \brief Load new URI

  \param view A #ModuleAbout
  \param uri New URI to load
 */
void module_about_load_uri (ModuleAbout *view, const gchar *uri)
{
     gchar **cmd = g_strsplit (&uri[6], "/", -1);
     gint cmdc = g_strv_length (cmd);
     gint i;

     printf ("about:");
     for (i = 0; i < cmdc; ++i)
          printf ("%s/", cmd[i]);
     printf ("\n");

     if (cmdc == 1) /* about:<command> */
     {
          if (g_str_equal (cmd[0], "blank"))           /* about:blank */
          {
               webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), "about:blank");
          }
          else if (g_str_equal (cmd[0], "config"))     /* about:config */
          {
               webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                         "<h1>Config: Not yet implemented</h1>",
                         "text/html",
                         global.cfg.global.encoding,
                         "about:config");
          }
          else
          {
               webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                         "<h1>Error: Unknow command</h1>",
                         "text/html",
                         global.cfg.global.encoding,
                         "about:error");
          }
     }
     else if (cmdc >= 2) /* about:<command>/<actions...> */
     {
          if (g_str_equal (cmd[0], "bookmarks"))       /* about:bookmarks/... */
          {
               if (g_str_equal (cmd[1], "show"))       /* about:bookmarks/show */
               {
                    webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                              "<h1>bookmarks (show): Not yet implemented</h1>",
                              "text/html",
                              global.cfg.global.encoding,
                              "about:bookmarks/show");
               }
               else if (g_str_equal (cmd[1], "add"))   /* about:bookmarks/add */
               {
                    webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                              "<h1>bookmarks (add): Not yet implemented</h1>",
                              "text/html",
                              global.cfg.global.encoding,
                              "about:bookmarks/add");
               }
               else
               {
                    webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                              "<h1>bookmarks: Unknow command</h1>",
                              "text/html",
                              global.cfg.global.encoding,
                              "about:error");
               }
          }
          else
          {
               webkit_web_view_load_string (WEBKIT_WEB_VIEW (view),
                         "<h1>Error: Unknow command</h1>",
                         "text/html",
                         global.cfg.global.encoding,
                         "about:error");
          }
     }
}

/*! @} */
