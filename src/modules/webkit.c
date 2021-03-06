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
 * \addtogroup mod-webkit
 *
 * @{
 */

static void cream_module_webkit_notify_uri_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self);
static void cream_module_webkit_notify_title_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self);
static void cream_module_webkit_notify_favicon_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self);
static void cream_module_webkit_notify_progress_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self);
static gboolean cream_module_webkit_button_press_event_cb (WebKitWebView *webview, GdkEventButton *event, CreamModuleWebKit *self);
static gboolean cream_module_webkit_signal_download_cb (WebKitWebView *webview, WebKitDownload *download, CreamModuleWebKit *self);

CREAM_DEFINE_MODULE (CreamModuleWebKit, cream_module_webkit)

/*!
 * @param self The #CreamModuleWebKit structure.
 * Initialize #CreamModuleWebKit.
 */
static void cream_module_webkit_init (CreamModuleWebKit *self)
{
     self->wsession  = webkit_get_default_session ();
     self->wsettings = webkit_web_settings_new ();
     self->wfavicons = webkit_get_icon_database ();

     g_object_set (G_OBJECT (self->wsettings), "enable-developer-extras", TRUE, NULL);

     cream_browser_add_protocol (app, "http",  G_OBJECT (self));
     cream_browser_add_protocol (app, "https", G_OBJECT (self));
     cream_browser_add_protocol (app, "about", G_OBJECT (self));
     cream_browser_add_protocol (app, "file",  G_OBJECT (self));
}

/*! @} */

/* methods */

static GtkWidget *cream_module_webkit_webview_new (CreamModule *self)
{
     CreamModuleWebKit *mod = CREAM_MODULE_WEBKIT (self);
     WebKitWebView *webview = WEBKIT_WEB_VIEW (webkit_web_view_new ());

     webkit_web_view_set_settings (webview, mod->wsettings);

     g_signal_connect (G_OBJECT (webview), "notify::uri",
                       G_CALLBACK (cream_module_webkit_notify_uri_cb),
                       self);

     g_signal_connect (G_OBJECT (webview), "notify::title",
                       G_CALLBACK (cream_module_webkit_notify_title_cb),
                       self);

     g_signal_connect (G_OBJECT (webview), "notify::icon-uri",
                       G_CALLBACK (cream_module_webkit_notify_favicon_cb),
                       self);

     g_signal_connect (G_OBJECT (webview), "notify::progress",
                       G_CALLBACK (cream_module_webkit_notify_progress_cb),
                       self);

     g_signal_connect (G_OBJECT (webview), "button-press-event",
                       G_CALLBACK (cream_module_webkit_button_press_event_cb),
                       self);

     g_signal_connect (G_OBJECT (webview), "download-requested",
                       G_CALLBACK (cream_module_webkit_signal_download_cb),
                       self);

     return GTK_WIDGET (webview);
}

static void cream_module_webkit_load_uri (CreamModule *self, GtkWidget *webview, UriScheme *uri)
{
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webview), uri->string);
}

static void cream_module_webkit_reload (CreamModule *self, GtkWidget *webview)
{
     webkit_web_view_reload (WEBKIT_WEB_VIEW (webview));
}

static gboolean cream_module_webkit_can_go_back (CreamModule *self, GtkWidget *webview)
{
     return webkit_web_view_can_go_back (WEBKIT_WEB_VIEW (webview));
}

static void cream_module_webkit_backward (CreamModule *self, GtkWidget *webview)
{
     const char *uri;

     if (webkit_web_view_can_go_back (WEBKIT_WEB_VIEW (webview)))
          webkit_web_view_go_back (WEBKIT_WEB_VIEW (webview));

     uri = webkit_web_view_get_uri (WEBKIT_WEB_VIEW (webview));
}

static gboolean cream_module_webkit_can_go_forward (CreamModule *self, GtkWidget *webview)
{
     return webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW (webview));
}

static void cream_module_webkit_forward (CreamModule *self, GtkWidget *webview)
{
     const char *uri;

     if (webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW (webview)))
          webkit_web_view_go_forward (WEBKIT_WEB_VIEW (webview));

     uri = webkit_web_view_get_uri (WEBKIT_WEB_VIEW (webview));
}

static gboolean cream_module_webkit_search (CreamModule *self, GtkWidget *webview, const gchar *text, gboolean forward)
{
     return webkit_web_view_search_text (WEBKIT_WEB_VIEW (webview), text, FALSE, forward, TRUE);
}

static void cream_module_webkit_proxy (CreamModule *self, const gchar *uri)
{
     CreamModuleWebKit *mod = CREAM_MODULE_WEBKIT (self);
     SoupURI *proxy = NULL;

     if (g_str_has_prefix (uri, "http://"))
     {
          proxy = soup_uri_new (uri);
     }
     else
     {
          proxy = soup_uri_new (g_strconcat ("http://", uri, NULL));
     }

     if (proxy)
          g_object_set (G_OBJECT (mod->wsession), "proxy-uri", proxy, NULL);
}

static void cream_module_webkit_useragent (CreamModule *self, const gchar *ua)
{
     CreamModuleWebKit *mod = CREAM_MODULE_WEBKIT (self);
     g_object_set (G_OBJECT (mod->wsettings), "user-agent", ua, NULL);
}

/*!
 * \defgroup mod-webkit-signals Signals
 * \ingroup mod-webkit
 * \class{WebKitWebView} signal handlers.
 *
 * @{
 */

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param pspec \class{GObject} parameter specification.
 * @param self A #CreamModuleWebKit object.
 *
 * This function handles the signal "notify::uri" which is emitted when the webview
 * loads a new URI.
 * This handlers emit the signal \ref mod-uri-changed.
 */
static void cream_module_webkit_notify_uri_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self)
{
     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_URI_CHANGED],
                    0, webview, webkit_web_view_get_uri (webview)
     );
}

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param pspec \class{GObject} parameter specification.
 * @param self A #CreamModuleWebKit object.
 *
 * This function handles the signal "notify::title" which is emitted when the webview's
 * page got a new title.
 * This handlers emit the signal \ref mod-title-changed.
 */
static void cream_module_webkit_notify_title_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self)
{
     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_TITLE_CHANGED],
                    0, webview, webkit_web_view_get_title (webview)
     );
}

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param pspec \class{GObject} parameter specification.
 * @param self A #CreamModuleWebKit object.
 *
 * This function handles the signal "notify::icon-uri" which is emitted when the favicon
 * of the webview's page is loaded.
 * This handlers emit the signal \ref mod-favicon-changed.
 */
static void cream_module_webkit_notify_favicon_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self)
{
     /* check favicon */
     GdkPixbuf *favicon = webkit_icon_database_get_icon_pixbuf (self->wfavicons, webkit_web_view_get_uri (webview));

     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_FAVICON_CHANGED],
                    0, webview, favicon
     );
}

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param pspec \class{GObject} parameter specification.
 * @param self A #CreamModuleWebKit object.
 *
 * This function handles the signal "notify::progress" which is emitted when the webview
 * is loading.
 * This handlers emit the signal \ref mod-progress-changed.
 */
static void cream_module_webkit_notify_progress_cb (WebKitWebView *webview, GParamSpec *pspec, CreamModuleWebKit *self)
{
     gdouble progress = 0.0;

     g_object_get (G_OBJECT (webview), "progress", &progress, NULL);

     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_PROGRESS_CHANGED],
                    0, webview, progress
     );
}

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param event The mouse event.
 * @param self A #CreamModuleWebKit object.
 * @return \c TRUE if the signal was handled (will stop all other handlers).
 *
 * This function handles the signal "button-press-event" which is emitted when
 * the user click on the webview.
 * If the click happened on an editable content, this handlers will set the
 * browser's state to #CREAM_MODE_INSERT, otherwise, it will set it to #CREAM_MODE_NORMAL.
 * This handler emit the signal \ref mod-state-changed.
 */
static gboolean cream_module_webkit_button_press_event_cb (WebKitWebView *webview, GdkEventButton *event, CreamModuleWebKit *self)
{
     WebKitHitTestResult *result = webkit_web_view_get_hit_test_result (webview, event);
     WebKitHitTestResultContext context;

     g_object_get (result, "context", &context, NULL);

     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_STATE_CHANGED],
                    0, webview, ((context & WEBKIT_HIT_TEST_RESULT_CONTEXT_EDITABLE) ? CREAM_MODE_INSERT : CREAM_MODE_NORMAL)
     );

     return FALSE;
}

/*!
 * @param webview A \class{WebKitWebView} object.
 * @param download A \class{WebKitDownload} object.
 * @param self A #CreamModuleWebKit object.
 * @return \c TRUE if the signal was handled (will stop all other handlers).
 *
 * This function handles the signal "download-requested" which is emitted when the
 * webview requests a download.
 * This handlers emit the signal \ref mod-download.
 */
static gboolean cream_module_webkit_signal_download_cb (WebKitWebView *webview, WebKitDownload *download, CreamModuleWebKit *self)
{
     gboolean ret = FALSE;

     g_signal_emit (G_OBJECT (self),
                    cream_module_webkit_signals[SIGNAL_DOWNLOAD],
                    0, webview, webkit_download_get_uri (download),
                    &ret
     );

     return ret;
}

/*! @} */
