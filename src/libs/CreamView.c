#include "CreamView.h"
#include "modules/www/WebViewModule.h"
#include <string.h>

static void cream_view_class_init (CreamViewClass *class);
static void cream_view_init (CreamView *view);
static void cream_view_load_content (CreamView *view);

enum
{
     URL_CHANGED_SIGNAL,
     TITLE_CHANGED_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NB_SIGNALS
};

static gint cream_view_signals[NB_SIGNALS] = { 0 };

GtkType cream_view_get_type (void)
{
     static GtkType cream_view_type = 0;

     if (!cream_view_type)
     {
          static const GtkTypeInfo cream_view_info =
          {
               "CreamView",
               sizeof (CreamView),
               sizeof (CreamViewClass),
               (GtkClassInitFunc) cream_view_class_init,
               (GtkObjectInitFunc) cream_view_init,
               NULL,
               NULL,
               (GtkClassInitFunc) NULL
          };

          cream_view_type = gtk_type_unique (gtk_scrolled_window_get_type (), &cream_view_info);
     }

     return cream_view_type;
}

static void cream_view_class_init (CreamViewClass *class)
{
     cream_view_signals[URL_CHANGED_SIGNAL] = g_signal_new ("url-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, url_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE, 0);
     class->url_changed = NULL;

     cream_view_signals[TITLE_CHANGED_SIGNAL] = g_signal_new ("title-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, title_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE, 0);
     class->title_changed = NULL;

     cream_view_signals[STATUS_CHANGED_SIGNAL] = g_signal_new ("status-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE, 0);
     class->status_changed = NULL;
}

static void cream_view_init (CreamView *obj)
{
     obj->content = NULL;
     obj->url = NULL;
     obj->title = NULL;
     obj->status = NULL;
}

static void cream_view_load_content (CreamView *view)
{
     gchar *url = view->url;

     /* information page */
     if (!strncmp (url, "about:", 6))
     {
          view->content = gtk_label_new (url + 6);
     }
     /* FTP repository */
     else if (!strncmp (url, "ftp://", 6))
     {
          view->content = gtk_label_new ("FTP: not yet implemented");
     }
     /* Web Page */
     else
     {
          if (!strncmp (url, "http://", 7)
               || !strncmp (url, "file://", 7))
          {
               view->content = module_web_view_new (url);
          }
          else if (url[0] == '/')
          {
               view->content = module_web_view_new (g_strdup_printf ("file://%s", url));
          }
          else
          {
               view->content = module_web_view_new (g_strdup_printf ("http://%s", url));
          }

          view->title = g_strdup (module_web_view_get_title (MODULE_WEB_VIEW (view->content)));
          view->status = g_strdup (module_web_view_get_status (MODULE_WEB_VIEW (view->content)));
     }
}

GtkWidget *cream_view_new (const gchar *url)
{
     CreamView *view = gtk_type_new (cream_view_get_type ());

     g_return_val_if_fail (view != NULL, NULL);

     if (url == NULL)
          view->url = g_strdup ("about:blank");
     else
          view->url = g_strdup (url);

     cream_view_load_content (view);
     gtk_container_add (GTK_CONTAINER (view), view->content);

     return GTK_WIDGET (view);
}

/* signals */
