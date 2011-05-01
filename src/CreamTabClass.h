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

#ifndef __CLASS_CREAMTAB_H
#define __CLASS_CREAMTAB_H

/*!
 * \defgroup creamtab CreamTab
 * CreamTab widget definition. Notebook's tabs part.
 * @{
 */

#include <gtk/gtk.h>

#include "WebViewClass.h"

G_BEGIN_DECLS

#define CREAM_TAB(obj)        G_TYPE_CHECK_INSTANCE_CAST(obj, creamtab_get_type (), CreamTab)
#define CREAM_TAB_CLASS(obj)  G_TYPE_CHECK_CLASS_CAST(obj, creamtab_get_type (), CreamTabClass)
#define IS_CREAM_TAB(obj)     G_TYPE_CHECK_INSTANCE_TYPE(obj, creamtab_get_type ())

/*!
 * \struct Tab
 * A tab.
 */
typedef struct
{
     WebView *w;         /*!< The associated #WebView */
     GtkWidget *lbl;     /*!< A \class{GtkLabel} */
     gboolean has_focus; /*!< if <code>TRUE</code>, this #Tab has the focus */
} Tab;


typedef struct _CreamTab CreamTab;
typedef struct _CreamTabClass CreamTabClass;

/*!
 * \class CreamTab
 * \implements Tab
 * This widget contains the list of tabs openned.
 * Each tabs is associated to a #WebView object.
 * With the #ViewArea widget, it acts like a \class{GtkNotebook}.
 */
struct _CreamTab
{
     /*< private >*/
     GtkHBox parent;

     GList *tabs;

     Tab *focus;
};

struct _CreamTabClass
{
     /*< private >*/
     GtkHBoxClass parent;
};

GType creamtab_get_type (void);
GtkWidget *creamtab_new (void);

gboolean creamtab_set_focus (CreamTab *t, WebView *w);
WebView *wcreamtab_get_focus (CreamTab *t);

void creamtab_add_webview (CreamTab *t, WebView *w);
void creamtab_del_webview (CreamTab *t, WebView *w);

G_END_DECLS

/*! @} */

#endif /* __CLASS_CREAMTAB_H */
