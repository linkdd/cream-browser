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
  @defgroup FTP Module 'FTP'
  @ingroup modules
  @brief Object which show a FTP repository

  @{
 */

#include "FTPModule.h"
#include <marshal.h>
#include <string.h>
#include <xdg.h>

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
  \var static guint module_ftp_signals[NB_SIGNALS]
  \brief Signal definitions
 */
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
     gchar *uri;                   /*!< URL of the FTP */
     CreamFtp *client;             /*!< Client socket */
};

/* End of private data */

static void module_ftp_class_init (ModuleFtpClass *class);
static void module_ftp_init (ModuleFtp *obj);

static gchar **cream_strsplit (gchar *str, gchar *delim);
static void module_ftp_update_model (ModuleFtp *obj);

static void module_ftp_row_activated_cb (ModuleFtp *obj, GtkTreePath *path, GtkTreeViewColumn *col, gpointer data);

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

/*!
  \fn GtkWidget *module_ftp_new (void)
  \brief Create a new #ModuleFtp object
  \return The new #ModuleFtp object
 */
GtkWidget *module_ftp_new (void)
{
     ModuleFtp *obj = gtk_type_new (module_ftp_get_type ());
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     GtkTreeViewColumn *col;
     GtkCellRenderer *cell;

     /* Init CreamFtp */
     priv->files = NULL;
     priv->uri = NULL;
     priv->client = cream_ftp_new ();

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

     g_signal_connect (G_OBJECT (obj), "row-activated", G_CALLBACK (module_ftp_row_activated_cb), NULL);

     return GTK_WIDGET (obj);
}

/*!
  \fn void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
  \brief Load a new URI

  \param obj ModuleFtp object
  \param uri New URI to load

  \todo Do the same with #CreamFtp
 */
void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
{
     ModuleFtpPrivate *priv = MODULE_FTP_GET_PRIVATE (obj);
     gchar *hostname = uri;
     int port = 21;

     if (priv->uri != NULL)
          g_free (priv->uri);
     priv->uri = g_strdup (uri);

     cream_ftp_connect (priv->client, hostname, port);
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

     file = gdk_pixbuf_new_from_file (find_xdg_file (XDG_TYPE_DATA, "cream-browser/icons/file.png"), NULL);
     folder = gdk_pixbuf_new_from_file (find_xdg_file (XDG_TYPE_DATA, "cream-browser/icons/folder.png"), NULL);

     if (1 /*CURL_LOAD_PROVISIONAL != curl_client_get_load_status (priv->curl)*/)
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

     gtk_tree_model_get_iter (model, &iter, path);
     gtk_tree_model_get (model, &iter, 1, &filepath, 3, &type, -1);

     if (g_str_equal (filepath, ".."))
     {
          gint l = strlen (obj->uri);
          gchar *p;

          if (g_str_equal (strchr (&obj->uri[6], '/'), "/"))
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

/*! @} */
