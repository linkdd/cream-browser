/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
 */

/*!
  \file WebViewModule.h
  \brief WebKit integration
  \author David Delassus
 */

#ifndef __CLASS_WEB_VIEW_MODULE_H
#define __CLASS_WEB_VIEW_MODULE_H

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

typedef struct _ModuleWebView ModuleWebView;
typedef struct _ModuleWebViewClass ModuleWebViewClass;

/*! \struct _ModuleWebView */
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

#endif /* __CLASS_WEB_VIEW_MODULE_H */
