#ifndef __WEBVIEW_CLASS_H
#define __WEBVIEW_CLASS_H

/*!
 * \defgroup webview WebView
 * WebView widget definition
 * @{
 */

#include <gtk/gtk.h>

#include "modules.h"

G_BEGIN_DECLS

#define WEB_VIEW(obj)         GTK_CHECK_CAST(obj, webview_get_type (), WebView)
#define WEB_VIEW_CLASS(obj)   GTK_CHECK_CLASS_CAST(klass, webview_get_type (), WebViewClass)
#define IS_WEB_VIEW(obj)      GTK_CHECK_TYPE(obj, webview_get_type ())

typedef struct _WebView WebView;
typedef struct _WebViewClass WebViewClass;

/*!
 * \class WebView
 * \implements CreamModule
 * This widget is an interface between the \class{GModule} and Cream-Browser.
 *
 * \signalsection
 * \signal{load-commit, signal_load_commit, void load_commit (WebView *obj\, const char *uri)}
 * \signal{load-changed, signal_load_changed, void load_changed (WebView *obj\, gint load)}
 * \signal{load-finished, signal_load_finished, void load_finished (WebView *obj)}
 * \signal{uri-changed, signal_uri_changed, void uri_changed (WebView *obj\, const gchar *uri)}
 * \signal{title-changed, signal_title_changed, void title_changed (WebView *obj\, const gchar *title)}
 * \signal{status-changed, signal_status_changed, void status_changed (WebView *obj\, const gchar *status)}
 * \signal{raise, signal_raise, void raise (WebView *obj)}
 * \signalendsection
 */
struct _WebView
{
     /*< private >*/
     GtkScrolledWindow parent;

     gboolean has_focus;
     CreamModule *mod;
     GtkWidget *child;

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
      * \fn void signal_load_commit (WebView *obj, const char *uri)
      * @param obj The #WebView object from which the signal was emitted.
      * @param uri New URI to be loaded.
      *
      * Signal emitted on new URI request.
      */
     void (*load_commit) (WebView *obj, const char *uri);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_load_changed (WebView *obj, gint load)
      * @param obj The #WebView object from which the signal was emitted.
      * @param load Percent of loading.
      *
      * Signal emitted on new load changes.
      */
     void (*load_changed) (WebView *obj, gint load);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void signal_load_finished (WebView *obj)
      * @param obj The #WebView object from which the signal was emitted.
      *
      * Signal emitted when the load is ended.
      */
     void (*load_finished) (WebView *obj);

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
      * \fn gboolean signal_download (WebView *obj, const gchar *file_uri);
      * @param obj The #WebView object from which the signal was emitted.
      * @param file_uri URI of the file to download.
      * @return <code>TRUE</code> if the signal is handled, <code>FALSE</code> otherwise.
      *
      * Signal emitted when a download is requested.
      */
     gboolean (*download) (WebView *obj, const gchar *file_uri);

};

GtkType webview_get_type (void);
GtkWidget *webview_new (CreamModule *mod);

CreamModule *webview_get_module (WebView *w);
void webview_set_module (WebView *w, CreamModule *mod);
GtkWidget *webview_get_child (WebView *w);

gboolean webview_has_focus (WebView *w);
void webview_raise (WebView *w);

void webview_load_uri (WebView *w, const gchar *uri);
const gchar *webview_get_uri (WebView *w);
const gchar *webview_get_title (WebView *w);
const gchar *webview_get_status (WebView *w);

/*! @} */

#endif /* __WEBVIEW_CLASS_H */
