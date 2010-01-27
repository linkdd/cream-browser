#ifndef __CLASS_WEB_VIEW_MODULE_H
#define __CLASS_WEB_VIEW_MODULE_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include <webkit/webkit.h>
#include "Favicon.h"

G_BEGIN_DECLS

#ifdef __cplusplus
extern "C" {
#endif

#define MODULE_WEB_VIEW(obj)            \
     GTK_CHECK_CAST (obj, module_web_view_get_type (), ModuleWebView)
#define MODULE_WEB_VIEW_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (klass, module_web_view_get_type (), ModuleWebViewClass)
#define MODULE_IS_WEB_VIEW(obj)         \
     GTK_CHECK_TYPE (obj, module_web_view_get_type ())

typedef struct _ModuleWebView ModuleWebView;
typedef struct _ModuleWebViewClass ModuleWebViewClass;

struct _ModuleWebView
{
     WebKitWebView parent_widget;

     gchar *url;
     gchar *title;
     gchar *status;

     WebKitWebInspector *inspector;
     WebKitWebSettings *settings;
     Favicon *ico;
};

struct _ModuleWebViewClass
{
     WebKitWebViewClass parent_class;

     void (*url_changed) (ModuleWebView *obj, gchar *url);
     void (*new_title) (ModuleWebView *obj, gchar *title);
     void (*status_changed) (ModuleWebView *obj, gchar *status);
     gboolean (*new_download) (ModuleWebView *obj, WebKitDownload *download);
};

GtkType module_web_view_get_type (void);
GtkWidget *module_web_view_new (void);

const gchar *module_web_view_get_uri (ModuleWebView *view);
const gchar *module_web_view_get_title (ModuleWebView *view);
const gchar *module_web_view_get_status (ModuleWebView *view);

void module_web_view_load_uri (ModuleWebView *view, const gchar *uri);

gboolean module_web_view_get_view_source_mode (ModuleWebView *view);
void module_web_view_set_view_source_mode (ModuleWebView *view, gboolean mode);

gfloat module_web_view_get_zoom_level (ModuleWebView *view);
void module_web_view_zoom_in (ModuleWebView *view);
void module_web_view_zoom_out (ModuleWebView *view);
void module_web_view_set_zoom_level (ModuleWebView *view, gfloat zoom);

WebKitWebInspector *module_web_view_get_web_inspector (ModuleWebView *view);
WebKitWebSettings *module_web_view_get_settings (ModuleWebView *view);
Favicon *module_web_view_get_favicon (ModuleWebView *view);

#ifdef __cplusplus
} /* extern "C" */
#endif

G_END_DECLS

#endif /* __CLASS_WEB_VIEW_MODULE_H */
