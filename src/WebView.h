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

#ifndef __CLASS_WEBVIEW_H
#define __CLASS_WEBVIEW_H

/*!
 * \defgroup webview WebView
 * \ingroup interface
 * WebView widget definition
 * @{
 */

#include <gtk/gtk.h>

#include "modules.h"

G_BEGIN_DECLS

#define CREAM_TYPE_WEBVIEW              (webview_get_type ())
#define CREAM_WEBVIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_WEBVIEW, WebView))
#define CREAM_IS_WEBVIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_WEBVIEW))
#define CREAM_WEBVIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_WEBVIEW, WebViewClass))

typedef struct _WebView WebView;
typedef struct _WebViewClass WebViewClass;

/*!
 * \class WebView
 * This widget is an interface between the \class{GModule} and Cream-Browser.
  */
struct _WebView
{
     /*< private >*/
     GtkScrolledWindow parent;

     gboolean has_focus;
     GObject *mod;
     GtkWidget *child;
     GtkWidget *notebook;

     gchar *uri;
     gchar *title;
     gchar *status;
     gint load_status;
};

struct _WebViewClass
{
     GtkScrolledWindowClass parent;

     void (*uri_changed) (WebView *obj, const gchar *uri);
     void (*title_changed) (WebView *obj, const gchar *title);
     void (*status_changed) (WebView *obj, const gchar *status);
     void (*raise) (WebView *obj);
     gboolean (*download) (WebView *obj, const gchar *file_uri);
     void (*module_changed) (WebView *obj, GObject *module);
     void (*favicon_changed) (WebView *obj, GdkPixbuf *pixbuf);
};

GType webview_get_type (void);
GtkWidget *webview_new (GObject *mod);

GObject *webview_get_module (WebView *w);
void webview_set_module (WebView *w, GObject *mod);
GtkWidget *webview_get_child (WebView *w);

gboolean webview_has_focus (WebView *w);
void webview_raise (WebView *w);

void webview_load_uri (WebView *w, const gchar *uri);
const gchar *webview_get_uri (WebView *w);
const gchar *webview_get_title (WebView *w);
const gchar *webview_get_status (WebView *w);

G_END_DECLS

/*! @} */

/*!
 * \defgroup webview-signals Signals
 * \ingroup webview
 *
 * \b Signals:
 * - \ref w-uri-changed
 * - \ref w-title-changed
 * - \ref w-status-changed
 * - \ref w-raise
 * - \ref w-download
 * - \ref w-module-changed
 * - \ref w-favicon-changed
 *
 * <hr>
 *
 * \section w-uri-changed "uri-changed"
 * \code void uri_changed (WebView *obj, const gchar *uri, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param uri New URI loaded.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when the URI loaded in the #WebView changes.
 *
 * <hr>
 *
 * \section w-title-changed "title-changed"
 * \code void title_changed (WebView *obj, const gchar *title, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param title New page's title.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when the page's title changes.
 *
 * <hr>
 *
 * \section w-status-changed "status-changed"
 * \code void status_changed (WebView *obj, const gchar *status, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param status New page's status.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when the page's status changes.
 *
 * <hr>
 *
 * \section w-raise "raise"
 * \code void raise (WebView *obj, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted in order to raise the #WebView.
 *
 * <hr>
 *
 * \section w-download "download"
 * \code gboolean download (WebView *obj, const gchar *file_uri, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param file_uri URI of the file to download.
 * @param user_data Data to pass to the handler.
 * @return \c TRUE if the signal was handled (will stop all other handlers)
 *
 * This signal is emitted when a download is requested.
 *
 * <hr>
 *
 * \section w-module-changed "module-changed"
 * \code void module_changed (WebView *obj, GObject *module, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param module A #CreamModule object.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when the #WebView change its module (another protocol is
 * requested).
 *
 * <hr>
 *
 * \section w-favicon-changed "favicon-changed"
 * \code void favicon_changed (WebView *obj, GdkPixbuf *pixbuf, gpointer user_data) \endcode
 * @param obj A #WebView object.
 * @param pixbuf The favicon's \class{GdkPixbuf}.
 * @param user_Data Data to pass to the handler.
 *
 * This signal is emitted when the #WebView's favicon is loaded.
 */

#endif /* __CLASS_WEBVIEW_H */
