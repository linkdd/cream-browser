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

#ifndef __CLASS_WEB_VIEW_MODULE_H
#define __CLASS_WEB_VIEW_MODULE_H

/*!
  @addtogroup WebView
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include <webkit/webkit.h>
#include <JavaScriptCore/JavaScript.h>
#include "Favicon.h"

G_BEGIN_DECLS

#define MODULE_WEB_VIEW(obj)            \
     GTK_CHECK_CAST (obj, module_web_view_get_type (), ModuleWebView)
#define MODULE_WEB_VIEW_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (klass, module_web_view_get_type (), ModuleWebViewClass)
#define MODULE_IS_WEB_VIEW(obj)         \
     GTK_CHECK_TYPE (obj, module_web_view_get_type ())

/*! \typedef struct _ModuleWebView ModuleWebView */
/*! \typedef struct _ModuleWebViewClass ModuleWebViewClass */

typedef struct _ModuleWebView ModuleWebView;
typedef struct _ModuleWebViewClass ModuleWebViewClass;

/*!
  \struct _ModuleWebView
  \implements WebKit
  \implements Favicon
 */
struct _ModuleWebView
{
     WebKitWebView parent_widget;       /*!< Parent instance */

     gchar *uri;                        /*!< URI loaded by WebKitWebView */
     gchar *title;                      /*!< Title of the page */
     gchar *status;                     /*!< Status of the page */
     gchar *jsmsg;                      /*!< JavaScript message sended by the page (when execute a JavaScript scrip) */

     WebKitWebInspector *inspector;     /*!< DOM Inspector */
     WebKitWebSettings *settings;       /*!< Settings of the WebKitWebView */
     Favicon *ico;                      /*!< Favicon of the page */
};

/*! \struct _ModuleWebViewClass */
struct _ModuleWebViewClass
{
     WebKitWebViewClass parent_class;   /*!< Parent class */

     void (*uri_changed) (ModuleWebView *obj, gchar *url);
     void (*new_title) (ModuleWebView *obj, gchar *title);
     void (*status_changed) (ModuleWebView *obj, gchar *status);
     gboolean (*new_download) (ModuleWebView *obj, WebKitDownload *download);
     gboolean (*switch_module) (ModuleWebView *obj, gchar *new_uri);
};

GtkType module_web_view_get_type (void);
GtkWidget *module_web_view_new (void);

void module_web_view_load_uri (ModuleWebView *view, const gchar *uri);
void module_web_view_js_script_execute (ModuleWebView *view, const gchar *script);

const gchar *module_web_view_get_uri (ModuleWebView *view);
const gchar *module_web_view_get_title (ModuleWebView *view);
const gchar *module_web_view_get_status (ModuleWebView *view);
const gchar *module_web_view_get_jsmsg (ModuleWebView *view);

gboolean module_web_view_get_view_source_mode (ModuleWebView *view);
void module_web_view_set_view_source_mode (ModuleWebView *view, gboolean mode);

gfloat module_web_view_get_zoom_level (ModuleWebView *view);
void module_web_view_zoom_in (ModuleWebView *view);
void module_web_view_zoom_out (ModuleWebView *view);
void module_web_view_set_zoom_level (ModuleWebView *view, gfloat zoom);

WebKitWebInspector *module_web_view_get_web_inspector (ModuleWebView *view);
WebKitWebSettings *module_web_view_get_settings (ModuleWebView *view);
Favicon *module_web_view_get_favicon (ModuleWebView *view);

G_END_DECLS

/*! @} */

#endif /* __CLASS_WEB_VIEW_MODULE_H */
