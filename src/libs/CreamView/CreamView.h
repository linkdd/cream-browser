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

#ifndef __CLASS_CREAM_VIEW_H
#define __CLASS_CREAM_VIEW_H

/*!
  @addtogroup CreamView
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <about/AboutModule.h>
#include <ftp/FTPModule.h>
#include <www/WebViewModule.h>
#include <vte/VTEModule.h>

#include "CreamBackwardForwardList.h"

G_BEGIN_DECLS

#define CREAM_VIEW(obj)            \
     GTK_CHECK_CAST(obj, cream_view_get_type (), CreamView)
#define CREAM_VIEW_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST(klass, cream_view_get_type (), CreamViewClass)
#define CREAM_IS_CREAM_VIEW(obj)   \
     GTK_CHECK_TYPE(obj, cream_view_get_type ())

typedef struct _CreamView CreamView;
typedef struct _CreamViewClass CreamViewClass;

/*!
  \struct _CreamView
  \implements CreamBackwardForwardList
  \implements ModuleWebView
  \implements ModuleFtp
  \implements ModuleAbout
 */
struct _CreamView
{
     GtkScrolledWindow scroll;          /*!< Parent instance */
     GtkAdjustment *adjust_h;
     GtkAdjustment *adjust_v;

     GtkWidget *content;                /*!< Content of the CreamView */

     gchar *uri;                        /*!< URI of the loaded page */
     gchar *title;                      /*!< Title of the loaded page */
     gchar *status;                     /*!< Status of the loaded page */
     gboolean view_source_mode;         /*!< Viewing mode (page's source if TRUE) */

     CreamBackwardForwardList *history; /*!< History */
};

/*! \struct _CreamViewClass */
struct _CreamViewClass
{
     GtkScrolledWindowClass parent_class;    /*!< Parent class */

     void (*uri_changed) (CreamView *obj, gchar *url);
     /*!< The "uri-changed" signal is emitted when the module load a new URL
       \param url The new URL
      */

     void (*new_title) (CreamView *obj, gchar *title);
     /*!< The "new-title" signal is emitted when the page change its title
       \param title The new page's title
      */

     void (*status_changed) (CreamView *obj, gchar *status);
     /*!< The "status-changed" signal is emitted when the module send a new message for the statusbar
       \param status The status message
      */

     gboolean (*new_download) (CreamView *obj, WebKitDownload *download);
     /*!< The "new-download" signal is emitted when the module requests a new download
       \param download WebKit object which represent the download request
       \return TRUE if you handle the signal
      */

     gboolean (*new_window) (CreamView *obj, const gchar *uri);
     /*!< The "new-window" signal is emitted when the module requests opening a new window
       \param uri The new URI to load in a new window
       \return TRUE if you handle the signal
      */
};

GtkType cream_view_get_type (void);
GtkWidget *cream_view_new (void);

void cream_view_load_uri (CreamView *obj, const gchar *uri);
GtkWidget *cream_view_get_content (CreamView *obj);
gboolean cream_view_get_view_source_mode (CreamView *obj);
void cream_view_set_view_source_mode (CreamView *obj, gboolean mode);

gboolean cream_view_can_go_back (CreamView *obj);
gboolean cream_view_can_go_forward (CreamView *obj);
void cream_view_go_back (CreamView *obj);
void cream_view_go_forward (CreamView *obj);
CreamBackwardForwardList *cream_view_get_backward_forward_list (CreamView *obj);

GtkAdjustment *cream_view_get_hadjustment (CreamView *obj);
GtkAdjustment *cream_view_get_vadjustment (CreamView *obj);

const gchar *cream_view_get_uri (CreamView *obj);
const gchar *cream_view_get_title (CreamView *obj);
const gchar *cream_view_get_status (CreamView *obj);

GdkPixbuf *cream_view_get_favicon (CreamView *obj);

G_END_DECLS

/*! @} */

#endif /* __CLASS_CREAM_VIEW_H */
