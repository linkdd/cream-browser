#include "WebViewModule.h"

enum
{
     URL_CHANGED_SIGNAL,
     TITLE_CHANGED_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NB_SIGNALS
};

static guint module_web_view_signals[NB_SIGNALS] = { 0 };

static void module_web_view_class_init (ModuleWebViewClass *class);
static void module_web_view_init (ModuleWebView *obj);

static WebKitWebView *module_web_view_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, gpointer data);

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
     module_web_view_signals[URL_CHANGED_SIGNAL] = g_signal_new (
          "url-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
          G_STRUCT_OFFSET (ModuleWebViewClass, url_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__CHAR,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_web_view_signals[TITLE_CHANGED_SIGNAL] = g_signal_new (
          "title-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
          G_STRUCT_OFFSET (ModuleWebViewClass, title_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__CHAR,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_web_view_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
          G_STRUCT_OFFSET (ModuleWebViewClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__CHAR,
          G_TYPE_NONE,
          1, G_TYPE_STRING);
}

static void module_web_view_init (ModuleWebView *obj)
{
     obj->url = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->inspector = NULL;
     obj->settings = NULL;
     obj->ico = NULL;
}

GtkWidget *module_web_view_new (void)
{
     ModuleWebView *obj = gtk_type_new (module_web_view_get_type ());

     obj->settings = webkit_web_settings_new ();
     g_object_set (G_OBJECT (obj->settings), "enable-developer-extras", TRUE, NULL);
     webkit_web_view_set_settings (WEBKIT_WEB_VIEW (obj), obj->settings);

     obj->inspector = webkit_web_view_get_inspector (WEBKIT_WEB_VIEW (obj));
     g_object_set (G_OBJECT (obj->inspector), "javascript-profiling-enabled", TRUE, NULL);
     g_signal_connect (G_OBJECT (obj->inspector), "inspect-web-view", G_CALLBACK (module_web_view_create_inspector_win), obj);

     return GTK_WIDGET (obj);
}

void module_web_view_load_uri (ModuleWebView *view, const gchar *uri)
{
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view), uri);

     view->url = g_strdup (webkit_web_view_get_uri (WEBKIT_WEB_VIEW (view)));
     view->title = g_strdup (webkit_web_view_get_title (WEBKIT_WEB_VIEW (view)));
     view->ico = favicon_new (webkit_web_view_get_icon_uri (WEBKIT_WEB_VIEW (view)));
}

static WebKitWebView *module_web_view_create_inspector_win (WebKitWebInspector *inspector, WebKitWebView *view, gpointer data)
{
     GtkWidget *win;
     GtkWidget *scroll;
     GtkWidget *page;

     win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (win), g_strconcat (
          "WebInspector - ", webkit_web_view_get_uri (view), NULL)
     );

     scroll = gtk_scrolled_window_new (NULL, NULL);
     gtk_container_add (GTK_CONTAINER (win), scroll);

     page = webkit_web_view_new ();
     gtk_container_add (GTK_CONTAINER (scroll), page);

     gtk_widget_show_all (win);

     return WEBKIT_WEB_VIEW (page);
}

const gchar *module_web_view_get_uri (ModuleWebView *view)
{
     return (const gchar *) view->url;
}

const gchar *module_web_view_get_title (ModuleWebView *view)
{
     return (const gchar *) view->title;
}

const gchar *module_web_view_get_status (ModuleWebView *view)
{
     return (const gchar *) view->status;
}

gboolean module_web_view_get_view_source_mode (ModuleWebView *view)
{
     return webkit_web_view_get_view_source_mode (WEBKIT_WEB_VIEW (view));
}

void module_web_view_set_view_source_mode (ModuleWebView *view, gboolean mode)
{
     webkit_web_view_set_view_source_mode (WEBKIT_WEB_VIEW (view), mode);
}

gfloat module_web_view_get_zoom_level (ModuleWebView *view)
{
     return webkit_web_view_get_zoom_level (WEBKIT_WEB_VIEW (view));
}

void module_web_view_zoom_in (ModuleWebView *view)
{
     webkit_web_view_zoom_in (WEBKIT_WEB_VIEW (view));
}

void module_web_view_zoom_out (ModuleWebView *view)
{
     webkit_web_view_zoom_out (WEBKIT_WEB_VIEW (view));
}

void module_web_view_set_zoom_level (ModuleWebView *view, gfloat zoom)
{
     webkit_web_view_set_zoom_level (WEBKIT_WEB_VIEW (view), zoom);
}

WebKitWebInspector *module_web_view_get_web_inspector (ModuleWebView *view)
{
     return view->inspector;
}

WebKitWebSettings *module_web_view_get_settings (ModuleWebView *view)
{
     return view->settings;
}

Favicon *module_web_view_get_favicon (ModuleWebView *view)
{
     return view->ico;
}
