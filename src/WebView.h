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
 * \implements CreamModule
 * This widget is an interface between the \class{GModule} and Cream-Browser.
 *
 * \signalsection
 * \signal{uri-changed, signal_uri_changed, void uri_changed (WebView *obj\, const gchar *uri)}
 * \signal{title-changed, signal_title_changed, void title_changed (WebView *obj\, const gchar *title)}
 * \signal{favicon-changed, signal_favicon_changed, void favicon_changed (WebView *obj\, GdkPixbuf *pixbuf)}
 * \signal{status-changed, signal_status_changed, void status_changed (WebView *obj\, const gchar *status)}
 * \signal{raise, signal_raise, void raise (WebView *obj)}
 * \signal{download, signal_download, gboolean download (WebView *obj\, const gchar *file_uri)}
 * \signal{module-changed, signal_module_changed, void module_changed (WebView *obj\, GObject *module)}
 * \signalendsection
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
     /*< private >*/
     GtkScrolledWindowClass parent;

     /*< public >*/

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_uri_changed (WebView *obj, const gchar *uri)
      * @param obj The #WebView object from which the signal was emitted.
      * @param uri New URI loaded.
      *
      * Signal emitted when the URI changes.
      */
     void (*uri_changed) (WebView *obj, const gchar *uri);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_title_changed (WebView *obj, const gchar *title)
      * @param obj The #WebView object from which the signal was emitted.
      * @param title The new page's title.
      *
      * Signal emitted when the page's title changes.
      */
     void (*title_changed) (WebView *obj, const gchar *title);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_status_changed (WebView *obj, const gchar *status)
      * @param obj The #WebView object from which the signal was emitted.
      * @param status The new status message.
      *
      * Signal emitted when the status message changes.
      */
     void (*status_changed) (WebView *obj, const gchar *status);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_raise (WebView *obj);
      * @param obj The #WebView object from which the signal was emitted.
      *
      * Signal emitted when the #WebView is raised.
      */
     void (*raise) (WebView *obj);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn gboolean signal_download (WebView *obj, const gchar *file_uri)
      * @param obj The #WebView object from which the signal was emitted.
      * @param file_uri URI of the file to download.
      * @return <code>TRUE</code> if the signal is handled, <code>FALSE</code> otherwise.
      *
      * Signal emitted when a download is requested.
      */
     gboolean (*download) (WebView *obj, const gchar *file_uri);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_module_changed (WebView *obj, GObject *module)
      * @param obj The #WebView object from which the signal was emitted.
      * @param module The new #CreamModule object used by the webview.
      *
      * Signal emitted when the webview uses a new module.
      */
     void (*module_changed) (WebView *obj, GObject *module);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_favicon_changed (WebView *obj, GdkPixbuf *pixbuf)
      * @param obj The #WebView object from which the signal was emitted.
      * @param pixbuf The new favicon's pixbuf.
      *
      * Signal emitted when the webview's favicon is loaded.
      */
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

#endif /* __CLASS_WEBVIEW_H */
