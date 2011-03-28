#ifndef __CLASS_VIEWAREA_H
#define __CLASS_VIEWAREA_H

/*!
 * \defgroup viewarea ViewArea
 * ViewArea widget definition
 * @{
 */

#include <gtk/gtk.h>

#include "WebViewClass.h"

G_BEGIN_DECLS

#define VIEW_AREA(obj)        G_TYPE_CHECK_INSTANCE_CAST(obj, viewarea_get_type (), ViewArea)
#define VIEW_AREA_CLASS(obj)  G_TYPE_CHECK_CLASS_CAST(obj, viewarea_get_type (), ViewAreaClass)
#define IS_VIEW_AREA(obj)     G_TYPE_CHECK_INSTANCE_TYPE(obj, viewarea_get_type ())

typedef struct _ViewArea ViewArea;
typedef struct _ViewAreaClass ViewAreaClass;

/*!
 * \class ViewArea
 * \implements WebView
 * This widget contains the list of all #WebView openned and
 * show only the one which has focus.
 */
struct _ViewArea
{
     /*< private >*/
     GtkBin parent;

     GList *webviews;
     GList *gcallbacks;

     WebView *focus;
};

struct _ViewAreaClass
{
     /*< private >*/
     GtkBinClass parent;
};

GType viewarea_get_type (void);
GtkWidget *viewarea_new (void);

gboolean viewarea_set_focus (ViewArea *v, WebView *w);
WebView *viewarea_get_focus (ViewArea *v);
void viewarea_add_webview (ViewArea *v, WebView *w);
void viewarea_del_webview (ViewArea *v, WebView *w);
void viewarea_signal_connect (ViewArea *v, const gchar *signal_name, GCallback cb, gpointer data);

/*! @} */

#endif /* __CLASS_VIEWAREA_H */
