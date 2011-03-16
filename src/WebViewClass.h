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
 * \signal{load-commit,load_commit, void load_commit (WebView *obj\, const char *uri)}
 * \signalendsection
 *
 * \propertysection
 * \property{uri,gchar*,READ_ONLY}
 * \property{title,gchar*,READ_ONLY}
 * \property{load-status,gint,READ_ONLY}
 * \property{module,CreamModule,READ_ONLY}
 * \propertyendsection
 */
struct _WebView
{
     /*< private >*/
     GtkScrolledWindow parent;

     gboolean has_focus;
     CreamModule *mod;
     GtkWidget *child;
};

struct _WebViewClass
{
     /*< private >*/
     GtkScrolledWindowClass parent;

     /*< public >*/

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void load_commit (WebView *obj, const char *uri)
      * @param obj The #WebView object from which the signal was emitted.
      * @param uri New URI to be loaded.
      *
      * Signal emitted on new URI request.
      */
     void (*load_commit) (WebView *obj, const char *uri);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void load_changed (WebView *obj, gint load)
      * @param obj The #WebView object from which the signal was emitted.
      * @param load Percent of loading.
      *
      * Signal emitted on new load changes.
      */
     void (*load_changed) (WebView *obj, gint load);

     /*!
      * \public \memberof WebView
      * \signalof{WebView}
      * \fn void load_finished (WebView *obj)
      * @param obj The #WebView object from which the signal was emitted.
      *
      * Signal emitted when the load is ended.
      */
     void (*load_finished) (WebView *obj);

     void (*uri_changed) (WebView *obj, const gchar *uri);
     void (*title_changed) (WebView *obj, const gchar *title);
     void (*status_changed) (WebView *obj, const gchar *status);

     void (*raise) (WebView *obj);

};

GtkType webview_get_type (void);
GtkWidget *webview_new (CreamModule *mod);

CreamModule *webview_get_module (WebView *w);
void webview_set_module (WebView *w, CreamModule *mod);
GtkWidget *webview_get_child (WebView *w);

gboolean webview_has_focus (WebView *w);
void webview_raise (WebView *w);

/*! @} */

#endif /* __WEBVIEW_CLASS_H */
