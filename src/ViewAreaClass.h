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

G_END_DECLS

/*! @} */

#endif /* __CLASS_VIEWAREA_H */
