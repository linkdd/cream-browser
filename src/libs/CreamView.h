#ifndef __CLASS_CREAM_VIEW_H
#define __CLASS_CREAM_VIEW_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#ifdef __cplusplus
extern "C" {
#endif

#define CREAM_VIEW(obj)            \
     GTK_CHECK_CAST(obj, cream_view_get_type (), CreamView)
#define CREAM_VIEW_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST(klass, cream_view_get_type (), CreamViewClass)
#define CREAM_IS_CREAM_VIEW(obj)   \
     GTK_CHECK_TYPE(obj, cream_view_get_type ())

typedef struct _CreamView CreamView;
typedef struct _CreamViewClass CreamViewClass;

struct _CreamView
{
     GtkScrolledWindow scroll;

     GtkWidget *content;

     gchar *url;
     gchar *title;
     gchar *status;
};

struct _CreamViewClass
{
     GtkScrolledWindowClass parent_class;

     void (*url_changed) (CreamView *obj);
     void (*title_changed) (CreamView *obj);
     void (*status_changed) (CreamView *obj);
};

GtkType cream_view_get_type (void);
GtkWidget *cream_view_new (const gchar *url);

#ifdef __cplusplus
} /* extern "C" */
#endif

G_END_DECLS

#endif /* __CLASS_CREAM_VIEW_H */
