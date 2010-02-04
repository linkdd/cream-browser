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
#include "marshal.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void cream_view_class_init (CreamViewClass *class);
static void cream_view_init (CreamView *view);
static void cream_view_load_content (CreamView *view);

static void cream_view_uri_changed_cb (GtkWidget *w, gchar *uri, gpointer data);
static void cream_view_new_title_cb (GtkWidget *w, gchar *title, gpointer data);
static void cream_view_status_changed_cb (GtkWidget *w, gchar *status, gpointer data);
static void cream_view_jsmsg_changed_cb (GtkWidget *w, gchar *jsmsg, gpointer data);
static gboolean cream_view_new_download_cb (GtkWidget *w, WebKitDownload *download, gpointer data);
static gboolean cream_view_switch_module_cb (GtkWidget *w, gchar *new_uri, gpointer data);

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
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
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CreamViewClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1,G_TYPE_STRING);

     cream_view_signals[NEW_TITLE_SIGNAL] = g_signal_new ("new-title",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CreamViewClass, new_title),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     cream_view_signals[STATUS_CHANGED_SIGNAL] = g_signal_new ("status-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CreamViewClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     cream_view_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new ("new-download",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CreamViewClass, new_download),
               NULL, NULL,
               g_cclosure_user_marshal_BOOLEAN__OBJECT,
               G_TYPE_BOOLEAN,
               1, G_TYPE_OBJECT);
}

static void cream_view_init (CreamView *obj)
{
     GtkWidget *scroll_h = gtk_hscrollbar_new (NULL);
     GtkWidget *scroll_v = gtk_vscrollbar_new (NULL);

     obj->adjust_h = gtk_range_get_adjustment (GTK_RANGE (scroll_h));
     obj->adjust_v = gtk_range_get_adjustment (GTK_RANGE (scroll_v));

     gtk_scrolled_window_set_hadjustment (GTK_SCROLLED_WINDOW (obj), obj->adjust_h);
     gtk_scrolled_window_set_vadjustment (GTK_SCROLLED_WINDOW (obj), obj->adjust_v);

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

     /* send an email */
     if (g_str_has_prefix (uri, "mailto:"))
     {
          if (fork ())
          {
               if (fork ())
               {
                    execl ("/bin/sh", "sh", "-c", g_strconcat ("xdg-open ", uri, NULL), (char *) NULL);
                    exit (EXIT_SUCCESS);
               }
               exit (EXIT_SUCCESS);
          }
     }
     /* information page */
     else if (g_str_has_prefix (uri, "about:"))
     {
          view->content = module_web_view_new ();
          module_web_view_load_uri (MODULE_WEB_VIEW (view->content), uri);
     }
     /* FTP repository */
     else if (g_str_has_prefix (uri, "ftp://"))
     {
          view->content = module_ftp_new ();
          module_ftp_load_uri (MODULE_FTP (view->content), uri);
     }
     /* Web Page */
     else
     {
          view->content = module_web_view_new ();

          module_web_view_set_view_source_mode (MODULE_WEB_VIEW (view->content), view->view_source_mode);
          if (g_str_has_prefix (uri, "http://")
               || g_str_has_prefix (uri, "file://")
               || g_str_has_prefix (uri, "https://"))
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

          g_object_connect (G_OBJECT (view->content),
               "signal::jsmsg-changed",  G_CALLBACK (cream_view_jsmsg_changed_cb),  view,
          NULL);
     }

     g_object_connect (G_OBJECT (view->content),
          "signal::uri-changed",    G_CALLBACK (cream_view_uri_changed_cb),    view,
          "signal::new-title",      G_CALLBACK (cream_view_new_title_cb),      view,
          "signal::status-changed", G_CALLBACK (cream_view_status_changed_cb), view,
          "signal::new-download",   G_CALLBACK (cream_view_new_download_cb),   view,
          "signal::switch-module",  G_CALLBACK (cream_view_switch_module_cb),  view,
     NULL);
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

GtkAdjustment *cream_view_get_hadjustment (CreamView *obj)
{
     return obj->adjust_h;
}

GtkAdjustment *cream_view_get_vadjustment (CreamView *obj)
{
     return obj->adjust_v;
}

/* signals */
static void cream_view_uri_changed_cb (GtkWidget *w, gchar *uri, gpointer data)
{
     CreamView *view = (CreamView *) data;

     if (view->uri)
          g_free (view->uri);
     view->uri = g_strdup (uri);

     g_signal_emit (
          G_OBJECT (view),
          cream_view_signals[URI_CHANGED_SIGNAL],
          0, uri
     );
}

static void cream_view_new_title_cb (GtkWidget *w, gchar *title, gpointer data)
{
     CreamView *view = (CreamView *) data;

     if (view->title)
          g_free (view->title);
     view->title = g_strdup (title);

     g_signal_emit (
          G_OBJECT (view),
          cream_view_signals[NEW_TITLE_SIGNAL],
          0, title
     );
}

static void cream_view_status_changed_cb (GtkWidget *w, gchar *status, gpointer data)
{
     CreamView *view = (CreamView *) data;

     if (view->status)
          g_free (view->status);
     view->status = g_strdup (status);

     g_signal_emit (
          G_OBJECT (view),
          cream_view_signals[STATUS_CHANGED_SIGNAL],
          0, status
     );
}

static void cream_view_jsmsg_changed_cb (GtkWidget *w, gchar *jsmsg, gpointer data)
{
     /* TODO */;
}

static gboolean cream_view_new_download_cb (GtkWidget *w, WebKitDownload *download, gpointer data)
{
     CreamView *view = (CreamView *) data;
     gboolean ret = FALSE;

     g_signal_emit (
          G_OBJECT (view),
          cream_view_signals[NEW_DOWNLOAD_SIGNAL],
          0, download,
          &ret
     );

     return ret;
}

static gboolean cream_view_switch_module_cb (GtkWidget *w, gchar *new_uri, gpointer data)
{
     cream_view_load_uri ((CreamView *) data, new_uri);
     return TRUE;
}
