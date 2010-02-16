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

/*!
  \file FTPModule.c
  \brief Module FTP definition
  \author David Delassus
 */

#include "FTPModule.h"
#include <marshal.h>
#include <string.h>

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
     SWITCH_MODULE_SIGNAL,
     NB_SIGNALS
};

static guint module_ftp_signals[NB_SIGNALS] = { 0 };

/*!
  \struct FtpFile
  \brief A file from the FTP repository
 */
typedef struct
{
     gchar *path;        /*!< URI of the file */
     size_t size;        /*!< File's size in bytes */
     gboolean is_dir;    /*!< TRUE if the file is a directory */
} FtpFile;

/* Private data */

#define MODULE_FTP_GET_PRIVATE(obj)     (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                         module_ftp_get_type (),                \
                                         ModuleFtpPrivate))

typedef struct _ModuleFtpPrivate ModuleFtpPrivate;

/*! \struct _ModuleFtpPrivate */
struct _ModuleFtpPrivate
{
     GList *files;                 /*!< List of all files on the FTP */
     GtkListStore *list_store;     /*!< Files list for the GtkTreeView object */
     GURI *uri;                    /*!< GURI object, to have the hostname, user and password, etc... */
     CurlClient *curl;             /*!< CURL object */
};

/* End of private data */

static void module_ftp_class_init (ModuleFtpClass *class);
static void module_ftp_init (ModuleFtp *obj);

static gchar **cream_strsplit (gchar *str, gchar *delim);
static void module_ftp_update_model (ModuleFtp *obj);

static void module_ftp_row_activated_cb (ModuleFtp *obj, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);
static void module_ftp_load_finished_cb (CurlClient *curl, ModuleFtp *obj);
static void module_ftp_load_error_cb (CurlClient *curl, gchar *uri, gpointer error, ModuleFtp *obj);
static void module_ftp_progress_changed_cb (CurlClient *curl, gfloat progress, ModuleFtp *obj);
static void module_ftp_uri_changed_cb (CurlClient *curl, gchar *uri, ModuleFtp *obj);
static void module_ftp_status_changed_cb (CurlClient *curl, gchar *status, ModuleFtp *obj);
static gboolean module_ftp_download_requested_cb (CurlClient *curl, gchar *uri, ModuleFtp *obj);

GtkType module_ftp_get_type (void)
{
     static GtkType module_ftp_type = 0;

     if (!module_ftp_type)
     {
          static const GtkTypeInfo module_ftp_info =
          {
               "ModuleFtp",
               sizeof (ModuleFtp),
               sizeof (ModuleFtpClass),
               (GtkClassInitFunc) module_ftp_class_init,
               (GtkObjectInitFunc) module_ftp_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          module_ftp_type = gtk_type_unique (GTK_TYPE_TREE_VIEW, &module_ftp_info);
     }

     return module_ftp_type;
}

static void module_ftp_class_init (ModuleFtpClass *class)
{
     g_type_class_add_private (class, sizeof (ModuleFtpPrivate));

     module_ftp_signals[URI_CHANGED_SIGNAL] = g_signal_new (
          "uri-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, uri_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_ftp_signals[NEW_TITLE_SIGNAL] = g_signal_new (
          "new-title",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, new_title),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_ftp_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_ftp_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new (
          "new-download",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, new_download),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__OBJECT,
          G_TYPE_BOOLEAN,
          1, G_TYPE_OBJECT);

     module_ftp_signals[SWITCH_MODULE_SIGNAL] = g_signal_new (
          "switch-module",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, switch_module),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__STRING,
          G_TYPE_BOOLEAN,
          1, G_TYPE_STRING);
}

static void module_ftp_init (ModuleFtp *obj)
{
     obj->uri = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->view_source_mode = FALSE;
}

GtkWidget *module_ftp_new (void)
{
     ModuleFtp *obj = gtk_type_new (module_ftp_get_type ());
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     GtkTreeViewColumn *col;
     GtkCellRenderer *cell;

     /* Init CURL */
     priv->files = NULL;
     priv->uri = NULL;
     priv->curl = curl_client_new ();

     /* add files to the iconview */
     priv->list_store = gtk_list_store_new (4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
     gtk_tree_view_set_model (GTK_TREE_VIEW (obj), GTK_TREE_MODEL (priv->list_store));

     cell = gtk_cell_renderer_pixbuf_new ();
     col = gtk_tree_view_column_new_with_attributes ("Icon", cell, "pixbuf", 0, NULL);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj), col);

     cell = gtk_cell_renderer_text_new ();
     col = gtk_tree_view_column_new_with_attributes ("Name", cell, "text", 1, NULL);
     gtk_tree_view_column_set_expand (col, TRUE);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj), col);

     cell = gtk_cell_renderer_text_new ();
     col = gtk_tree_view_column_new_with_attributes ("Size", cell, "text", 2, NULL);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj), col);

     cell = gtk_cell_renderer_text_new ();
     col = gtk_tree_view_column_new_with_attributes ("Type", cell, "text", 3, NULL);
     gtk_tree_view_append_column (GTK_TREE_VIEW (obj), col);

     g_signal_connect (G_OBJECT (obj),        "row-activated", G_CALLBACK (module_ftp_row_activated_cb), NULL);
     g_object_connect (G_OBJECT (priv->curl),
          "signal::load-finished",         G_CALLBACK (module_ftp_load_finished_cb),      obj,
          "signal::load-error",            G_CALLBACK (module_ftp_load_error_cb),         obj,
          "signal::load-progress-changed", G_CALLBACK (module_ftp_progress_changed_cb),   obj,
          "signal::uri-changed",           G_CALLBACK (module_ftp_uri_changed_cb),        obj,
          "signal::status-changed",        G_CALLBACK (module_ftp_status_changed_cb),     obj,
          "signal::download-requested",    G_CALLBACK (module_ftp_download_requested_cb), obj,
     NULL);

     return GTK_WIDGET (obj);
}

void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);

     curl_client_load_uri (priv->curl, uri);
     priv->uri = curl_client_get_guri (priv->curl);
}

static gchar **cream_strsplit (gchar *string, gchar *delim)
{
     gchar **tmp = g_strsplit (string, delim, -1);
     GArray *ret = g_array_new (TRUE, TRUE, sizeof (gchar*));
     int i, l = g_strv_length (tmp);

     for (i = 0; i < l; ++i)
          if (strcmp (tmp[i], ""))
               ret = g_array_append_val (ret, tmp[i]);

     return (gchar **) g_array_free (ret, FALSE);
}

static gchar *cream_bytes (gsize bytes)
{
     gchar *array[] = { "b", "kb", "Mb", "Gb", "Tb" };
     gfloat tmp = (gfloat)bytes / 1024.0;
     gint i = 0;

     while (tmp > 1.0)
     {
          tmp /= 1024.0;
          i++;
     }

     return g_strdup_printf ("%.1f %s", tmp * 1024.0, array[i]);
}

static void module_ftp_update_model (ModuleFtp *obj)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     GdkPixbuf *file, *folder;
     GtkTreeIter iter;
     int i;

     file = gdk_pixbuf_new_from_file (g_build_filename (g_get_home_dir (), ".cream-browser", "icons", "file.png", NULL), NULL);
     folder = gdk_pixbuf_new_from_file (g_build_filename (g_get_home_dir (), ".cream-browser", "icons", "folder.png", NULL), NULL);

     if (CURL_LOAD_PROVISIONAL != curl_client_get_load_status (priv->curl))
     {
          gtk_list_store_clear (priv->list_store);

          gtk_list_store_append (priv->list_store, &iter);
          gtk_list_store_set (priv->list_store, &iter,
                    0, folder,
                    1, "..",
                    2, "0 b",
                    3, "directory",
                    -1);

          for (i = 0; i < g_list_length (priv->files); ++i)
          {
               FtpFile *el = g_list_nth_data (priv->files, i);

               gtk_list_store_append (priv->list_store, &iter);
               gtk_list_store_set (priv->list_store, &iter,
                         0, (el->is_dir ? folder : file),
                         1, el->path,
                         2, cream_bytes (el->size),
                         3, (el->is_dir ? "directory" : "file"),
                         -1);
          }
     }
}

/* signals */
static void module_ftp_row_activated_cb (ModuleFtp *obj, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data)
{
     GtkTreeIter iter;
     GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (obj));
     gchar *filepath;
     gchar *type;
     GURI *tmp = gnet_uri_new (obj->uri);

     gtk_tree_model_get_iter (model, &iter, path);
     gtk_tree_model_get (model, &iter, 1, &filepath, 3, &type, -1);

     if (g_str_equal (filepath, ".."))
     {
          gint l = strlen (obj->uri);
          gchar *p;

          if (g_str_equal (tmp->path, "/"))
               return;

          obj->uri[l - 1] = 0;
          p = strrchr (obj->uri, '/');
          if (p != NULL) *p = 0;

          module_ftp_load_uri (obj, g_strconcat (obj->uri, "/", NULL));
          return;
     }

     if (g_str_equal (type, "directory"))
          module_ftp_load_uri (obj, g_strconcat (obj->uri, filepath, "/", NULL));
     else
     {
          WebKitNetworkRequest *tmp = webkit_network_request_new (g_strconcat (obj->uri, filepath, NULL));
          WebKitDownload *dl = webkit_download_new (tmp);
          gboolean ret = FALSE;

          g_signal_emit (
               G_OBJECT (obj),
               module_ftp_signals[NEW_DOWNLOAD_SIGNAL],
               0, dl,
               &ret
          );

          if (!ret)
               printf ("Download requested: %s\n", g_strconcat (obj->uri, filepath, NULL));
     }
}

static void module_ftp_load_finished_cb (CurlClient *curl, ModuleFtp *obj)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     gchar *tmp = g_strdup (curl_client_get_content (curl));
     gchar **lines = cream_strsplit (tmp, "\n");
     gint i, l = g_strv_length (lines);

     g_list_free (priv->files);
     priv->files = NULL;
     for (i = 0; i < l; ++i)
     {
          FtpFile *el = g_malloc (sizeof (FtpFile));
          gchar **cols = cream_strsplit (lines[i], " ");

          if (cols[0][0] == 'd')
               el->is_dir = TRUE;

          el->size = g_ascii_strtoll (cols[4], NULL, 10);
          el->path = g_strdup (cols[8]);

          priv->files = g_list_append (priv->files, el);
     }

     module_ftp_update_model (obj);
     gtk_widget_show_all (GTK_WIDGET (obj));

     priv->uri = curl_client_get_guri (curl);
     g_free (obj->uri);
     obj->uri = g_strdup (gnet_uri_get_string (priv->uri));

     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[URI_CHANGED_SIGNAL],
          0, obj->uri
     );

     obj->status = g_strdup (obj->uri);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );

     obj->title = g_strdup_printf ("Index of %s", priv->uri->path);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[NEW_TITLE_SIGNAL],
          0, obj->title
     );
}

static void module_ftp_load_error_cb (CurlClient *curl, gchar *uri, gpointer error, ModuleFtp *obj)
{
     printf ("Error '%s' : %s\n", uri, ((GError *) error)->message);
}

static void module_ftp_progress_changed_cb (CurlClient *curl, gfloat progress, ModuleFtp *obj)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);

     g_free (obj->status);
     obj->status = g_strdup_printf ("Transfering data from %s ...", priv->uri->hostname);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );
}

static void module_ftp_uri_changed_cb (CurlClient *curl, gchar *uri, ModuleFtp *obj)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     priv->uri = curl_client_get_guri (curl);

     g_free (obj->uri);
     obj->uri = g_strdup (uri);

     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[URI_CHANGED_SIGNAL],
          0, obj->uri
     );
}

static void module_ftp_status_changed_cb (CurlClient *curl, gchar *status, ModuleFtp *obj)
{
     g_free (obj->status);
     obj->status = g_strdup (status);

     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );
}

static gboolean module_ftp_download_requested_cb (CurlClient *curl, gchar *uri, ModuleFtp *obj)
{
     WebKitNetworkRequest *tmp = webkit_network_request_new (uri);
     WebKitDownload *dl = webkit_download_new (tmp);
     gboolean ret = FALSE;

     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[NEW_DOWNLOAD_SIGNAL],
          0, dl,
          &ret
     );

     return ret;
}


