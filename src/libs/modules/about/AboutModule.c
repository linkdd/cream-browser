/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  @defgroup About Module About
  @ingroup modules
  @brief Internal browser protocole 'about:' to show history/bookmarks/etc...

  @{
 */

#include "AboutModule.h"
#include <marshal.h>
#include "local.h"

/*! \struct about_t */
struct about_t
{
     gchar *name;                       /*!< Name of the URL (for example: about:config, config is the name) */
     void (*func) (ModuleAbout *obj);   /*!< Function to call when the URL is loaded */
};

static void module_about_blank_callback (ModuleAbout *obj);
static void module_about_config_callback (ModuleAbout *obj);
static void module_about_bookmarks_callback (ModuleAbout *obj);
static void module_about_error_callback (ModuleAbout *obj);

/*!
  \var struct about_t module_about_callbacks[]
  \brief Callbacks for every URL
 */
struct about_t module_about_callbacks[] =
{
     { "blank",     module_about_blank_callback },
     { "config",    module_about_config_callback },
     { "bookmarks", module_about_bookmarks_callback },
     { "error",     module_about_error_callback },
     { NULL,        NULL }
};

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
     SWITCH_MODULE_SIGNAL,
     NB_SIGNALS
};

/*!
  \var static guint module_about_signals[NB_SIGNALS]
  \brief Signal definitions
 */
static guint module_about_signals[NB_SIGNALS];

static void module_about_class_init (ModuleAboutClass *class);
static void module_about_init (ModuleAbout *obj);

GtkType module_about_get_type (void)
{
     static GtkType module_about_type = 0;

     if (!module_about_type)
     {
          static const GtkTypeInfo module_about_info =
          {
               "ModuleAbout",
               sizeof (ModuleAbout),
               sizeof (ModuleAboutClass),
               (GtkClassInitFunc) module_about_class_init,
               (GtkObjectInitFunc) module_about_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          module_about_type = gtk_type_unique (gtk_viewport_get_type (), &module_about_info);
     }

     return module_about_type;
}

static void module_about_class_init (ModuleAboutClass *class)
{
     module_about_signals[URI_CHANGED_SIGNAL] = g_signal_new (
          "uri-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, uri_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[NEW_TITLE_SIGNAL] = g_signal_new (
          "new-title",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, new_title),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_about_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new (
          "new-download",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, new_download),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__OBJECT,
          G_TYPE_BOOLEAN,
          1, G_TYPE_OBJECT);


     module_about_signals[SWITCH_MODULE_SIGNAL] = g_signal_new (
          "switch-module",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleAboutClass, switch_module),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__STRING,
          G_TYPE_BOOLEAN,
          1, G_TYPE_STRING);
}

static void module_about_init (ModuleAbout *obj)
{
     obj->uri    = NULL;
     obj->title  = NULL;
     obj->child  = NULL;
}

/*!
  \fn GtkWidget *module_about_new (void)
  \brief Create a new #ModuleAbout object

  \return A new #ModuleAbout
 */
GtkWidget *module_about_new (void)
{
     ModuleAbout *obj = gtk_type_new (module_about_get_type ());

     return GTK_WIDGET (obj);
}

/*!
  \fn void module_about_load_uri (ModuleAbout *view, const gchar *uri)
  \brief Load new URI

  \param view A #ModuleAbout
  \param uri New URI to load
 */
void module_about_load_uri (ModuleAbout *view, const gchar *uri)
{
     const gchar *cmd = &uri[6];
     gboolean success = FALSE;
     int i;

     g_return_if_fail (view != NULL);

     if (view->child)
     {
          gtk_container_remove (GTK_CONTAINER (view), view->child);
          gtk_widget_destroy (view->child);
          view->child = NULL;
     }

     for (i = 0; module_about_callbacks[i].name != NULL; ++i)
     {
          if (g_str_equal (cmd, module_about_callbacks[i].name))
          {
               if (module_about_callbacks[i].func)
               {
                    module_about_callbacks[i].func (view);
                    success = TRUE;

                    if (view->uri)
                         g_free (view->uri);
                    view->uri = g_strdup_printf ("about:%s", cmd);

                    if (view->title)
                         g_free (view->title);
                    view->title = g_strdup (cmd);
               }
               break;
          }
     }

     if (!success)
     {
          module_about_error_callback (view);
          if (view->uri)
               g_free (view->uri);
          view->uri = g_strdup ("about:error");

          if (view->title)
               g_free (view->title);
          view->title = g_strdup (cmd);
     }

     if (view->child)
          gtk_container_add (GTK_CONTAINER (view), view->child);

     gtk_widget_show_all (GTK_WIDGET (view));

     g_signal_emit (
          G_OBJECT (view),
          module_about_signals[URI_CHANGED_SIGNAL],
          0, view->uri
     );

     g_signal_emit (
          G_OBJECT (view),
          module_about_signals[NEW_TITLE_SIGNAL],
          0, view->title
     );
}

static void module_about_blank_callback (ModuleAbout *obj)
{
     obj->child = webkit_web_view_new ();
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (obj->child), "about:blank");
}

static void module_about_config_callback (ModuleAbout *obj)
{
     obj->child = webkit_web_view_new ();
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (obj->child), "about:blank");
}

static void module_about_bookmarks_callback (ModuleAbout *obj)
{
     GtkTreeStore *store;
     GtkTreeViewColumn *col;
     GtkCellRenderer *render;
     int i;

     store = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_STRING);

     for (i = 0; i < g_slist_length (global.browser.bookmarks); ++i)
     {
          struct bookmark_t *bm = (struct bookmark_t *) g_slist_nth_data (global.browser.bookmarks, i);
          GtkTreeIter iter;

          gtk_tree_store_append (store, &iter, NULL);

          if (bm->title)
          {
               gtk_tree_store_set (store, &iter,
                         0, bm->uri,
                         1, bm->title,
                         -1);
          }
          else
          {
               gtk_tree_store_set (store, &iter,
                         0, bm->uri,
                         1, bm->uri,
                         -1);
          }
     }

     obj->child = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

     render = gtk_cell_renderer_text_new ();
     col = gtk_tree_view_column_new_with_attributes ("URL", render, "text", 0, NULL);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj->child), col);

     col = gtk_tree_view_column_new_with_attributes ("Title", render, "text", 1, NULL);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj->child), col);
}

static void module_about_error_callback (ModuleAbout *obj)
{
     obj->child = webkit_web_view_new ();
     webkit_web_view_load_uri (WEBKIT_WEB_VIEW (obj->child), "about:error");
}

/*! @} */
