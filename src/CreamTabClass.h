#ifndef __CLASS_CREAMTAB_H
#define __CLASS_CREAMTAB_H

/*!
 * \defgroup creamtab CreamTab
 * CreamTan widget definition. Notebook's tabs part.
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
