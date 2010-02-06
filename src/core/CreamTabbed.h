#ifndef __CLASS_CREAM_TABBED_H
#define __CLASS_CREAM_TABBED_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <CreamView.h>

G_BEGIN_DECLS

#define CREAM_TABBED(obj)          \
     GTK_CHECK_CAST(obj, cream_tabbed_get_type (), CreamTabbed)
#define CREAM_TABBED_CLASS(klass)  \
     GTK_CHECK_CLASS_CAST(klass, cream_tabbed_get_type (), CreamTabbedClass)
#define IS_CREAM_TABBED(obj)       \
     GTK_CHECK_TYPE(obj, cream_tabbed_get_type ());

typedef struct _CreamTabbed CreamTabbed;
typedef struct _CreamTabbedClass CreamTabbedClass;

struct _CreamTabbed
{
     GtkVBox parent;

     GtkWidget *creamview;
     struct
     {
          GtkWidget *eventbox;
          GtkWidget *box;
          GtkWidget *url;
          GtkWidget *state;
     } statusbar;

     GtkWidget *inputbox;

     GtkAdjustment *adjust_v;
};

struct _CreamTabbedClass
{
     GtkVBoxClass parent_class;

     void (*update_notebook_title) (CreamTabbed *obj);
};

GtkType cream_tabbed_get_type (void);
GtkWidget *cream_tabbed_new (void);

void cream_tabbed_load_uri (CreamTabbed *obj, gchar *uri);

const gchar *cream_tabbed_get_uri (CreamTabbed *obj);
const gchar *cream_tabbed_get_title (CreamTabbed *obj);
const gchar *cream_tabbed_get_status (CreamTabbed *obj);

G_END_DECLS

#endif /* __CLASS_CREAM_TABBED_H */
