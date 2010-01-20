#include "CreamView.h"
#include <webkit/webkit.h>
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
     GtkObjectClass *obj_class;

     cream_view_signals[URL_CHANGED_SIGNAL] = gtk_signal_new ("url-changed",
               GTK_RUN_FIRST,
               obj_class->type,
               GTK_SIGNAL_OFFSET (CreamViewClass, url_changed),
               gtk_signal_default_marshaller, GTK_ARG_NONE, 0);
     class->url_changed = cream_view_url_changed;

     cream_view_signals[TITLE_CHANGED_SIGNAL] = gtk_signal_new ("title-changed",
               GTK_RUN_FIRST,
               obj_class->type,
               GTK_SIGNAL_OFFSET (CreamViewClass, title_changed),
               gtk_signal_default_marshaller, GTK_ARG_NONE, 0);
     class->title_changed = cream_view_title_changed;

     cream_view_signals[STATUS_CHANGED_SIGNAL] = gtk_signal_new ("status-changed",
               GTK_RUN_FIRST,
               obj_class->type,
               GTK_SIGNAL_OFFSET (CreamViewClass, status_changed),
               gtk_signal_default_marshaller, GTK_ARG_NONE, 0);
     class->status_changed = cream_view_status_changed;

     gtk_object_class_add_signals (object_class, cream_view_signals, NB_SIGNALS);
}

static void cream_view_init (CreamView *obj)
{
     view->content = NULL;
     view->url = NULL;
     view->title = NULL;
     view->status = NULL;
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
     else if (!strncmp (url, "http://", 7)
               || !strncmp (url, "file://", 7)
               || url[0] == '/')
     {
          view->content = webkit_web_view_new ();

          if (url[0] == '/')
               webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view->content), g_strdup_printf ("file://%s", url));
          else
               webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view->content), url);

          view->title = webkit_web_view_get_title (WEBKIT_WEB_VIEW (view->content));
     }
     else
     {
          view->content = webkit_web_view_new ();
          webkit_web_view_load_uri (WEBKIT_WEB_VIEW (view->content), g_strdup_printf ("http://%s", url));
          view->title = webkit_web_view_get_title (WEBKIT_WEB_VIEW (view->content));
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


