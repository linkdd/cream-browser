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

#define WEB_VIEW(obj)         GTK_CHECK_CAST(obj, web_view_get_type (), WebView)
#define WEB_VIEW_CLASS(obj)   GTK_CHECK_CLASS_CAST(klass, web_view_get_type (), WebViewClass)
#define IS_WEB_VIEW(obj)      GTK_CHECK_TYPE(obj, web_view_get_type ())

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
};

/*! @} */

#endif /* __WEBVIEW_CLASS_H */
