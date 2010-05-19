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

#ifndef __CLASS_ABOUT_MODULE_H
#define __CLASS_ABOUT_MODULE_H

/*!
  @addtogroup About
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include <webkit/webkit.h>

G_BEGIN_DECLS

#define MODULE_ABOUT(obj)            \
     GTK_CHECK_CAST (obj, module_about_get_type (), ModuleAbout)
#define MODULE_ABOUT_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (klass, module_about_get_type (), ModuleAboutClass)
#define MODULE_IS_ABOUT(obj)         \
     GTK_CHECK_TYPE (obj, module_about_get_type ())

/*! \typedef struct _ModuleAbout ModuleAbout */
/*! \typedef struct _ModuleAboutClass ModuleAboutClass */

typedef struct _ModuleAbout ModuleAbout;
typedef struct _ModuleAboutClass ModuleAboutClass;

/*!
  \struct _ModuleAbout
  \implements WebKit
 */
struct _ModuleAbout
{
     WebKitWebView parent;    /*!< Parent instance */

     gchar *uri;
     gchar *title;
     gchar *status;

     WebKitWebInspector *inspector;
     WebKitWebSettings *settings;
};

/*! \struct _ModuleAboutClass */
struct _ModuleAboutClass
{
     WebKitWebViewClass parent_class; /*!< Parent class */

     void (*uri_changed) (ModuleAbout *obj, gchar *url);
     void (*new_title) (ModuleAbout *obj, gchar *title);
     void (*status_changed) (ModuleAbout *obj, gchar *status);
     gboolean (*new_download) (ModuleAbout *obj, WebKitDownload *download);
     gboolean (*switch_module) (ModuleAbout *obj, gchar *new_uri);
};

GtkType module_about_get_type (void);
GtkWidget *module_about_new (void);

void module_about_load_uri (ModuleAbout *view, const gchar *uri);

/*! @} */

#endif /* __CLASS_ABOUT_MODULE_H */

