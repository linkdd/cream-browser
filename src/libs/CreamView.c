/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CreamView.h"
#include <string.h>

static void cream_view_class_init (CreamViewClass *class);
static void cream_view_init (CreamView *view);
static void cream_view_load_content (CreamView *view);

static void cream_view_uri_changed_cb (GtkWidget *w, gchar *uri, gpointer data);

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NB_SIGNALS
};

static guint cream_view_signals[NB_SIGNALS] = { 0 };

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
     cream_view_signals[URI_CHANGED_SIGNAL] = g_signal_new ("uri-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1,G_TYPE_STRING);

     cream_view_signals[NEW_TITLE_SIGNAL] = g_signal_new ("new-title",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, new_title),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     cream_view_signals[STATUS_CHANGED_SIGNAL] = g_signal_new ("status-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (CreamViewClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);
}

static void cream_view_init (CreamView *obj)
{
     obj->content = NULL;
     obj->uri = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->view_source_mode = FALSE;
     obj->history = cream_backward_forward_list_new ();
}

static void cream_view_load_content (CreamView *view)
{
     gchar *uri = view->uri;

     /* information page */
     if (!strncmp (uri, "about:", 6))
     {
          view->content = module_web_view_new ();
          module_web_view_load_uri (MODULE_WEB_VIEW (view->content), uri);
     }
     /* FTP repository */
     else if (!strncmp (uri, "ftp://", 6))
     {
          view->content = gtk_label_new ("FTP: not yet implemented");
     }
     /* Web Page */
     else
     {
          view->content = module_web_view_new ();

          module_web_view_set_view_source_mode (MODULE_WEB_VIEW (view->content), view->view_source_mode);
          if (!strncmp (uri, "http://", 7)
               || !strncmp (uri, "file://", 7)
               || !strncmp (uri, "https://", 8))
          {
               module_web_view_load_uri (MODULE_WEB_VIEW (view->content), uri);
          }
          else if (uri[0] == '/')
          {
               module_web_view_load_uri (MODULE_WEB_VIEW (view->content), g_strdup_printf ("file://%s", uri));
          }
          else
          {
               module_web_view_load_uri (MODULE_WEB_VIEW (view->content), g_strdup_printf ("http://%s", uri));
          }

          g_signal_connect (G_OBJECT (view->content), "uri-changed", G_CALLBACK (cream_view_uri_changed_cb), view);
     }
}

GtkWidget *cream_view_new (void)
{
     CreamView *view = gtk_type_new (cream_view_get_type ());

     g_return_val_if_fail (view != NULL, NULL);

     view->uri = g_strdup ("about:blank");
     cream_view_load_content (view);
     gtk_container_add (GTK_CONTAINER (view), view->content);

     return GTK_WIDGET (view);
}

/* methods */
void cream_view_load_uri (CreamView *obj, const gchar *uri)
{
     if (uri != NULL)
          obj->uri = g_strdup (uri);
     else
          obj->uri = g_strdup ("about:blank");

     gtk_container_remove (GTK_CONTAINER (obj), obj->content);
     cream_view_load_content (obj);
     gtk_container_add (GTK_CONTAINER (obj), obj->content);
}

GtkWidget *cream_view_get_content (CreamView *obj)
{
     return obj->content;
}

gboolean cream_view_get_view_source_mode (CreamView *obj)
{
     return obj->view_source_mode;
}

void cream_view_set_view_source_mode (CreamView *obj, gboolean mode)
{
     obj->view_source_mode = mode;
}

gboolean cream_view_can_go_back (CreamView *obj)
{
     return FALSE; /* TODO */
}

gboolean cream_view_can_go_forward (CreamView *obj)
{
     return FALSE; /* TODO */
}

void cream_view_go_back (CreamView *obj)
{
     /* TODO */;
}

void cream_view_go_forward (CreamView *obj)
{
     /* TODO */;
}

CreamBackwardForwardList *cream_view_get_backward_forward_list (CreamView *obj)
{
     return obj->history;
}

/* signals */
static void cream_view_uri_changed_cb (GtkWidget *w, gchar *uri, gpointer data)
{
     CreamView *view = (CreamView *) data;

     view->uri = g_strdup (uri);

     g_signal_emit (
          G_OBJECT (view),
          cream_view_signals[URI_CHANGED_SIGNAL],
          0, uri
     );
}
