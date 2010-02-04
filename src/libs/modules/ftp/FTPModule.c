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

#include "FTPModule.h"
#include "../../marshal.h"
#include <string.h>

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
     SWITCH_MODULE_SIGNAL,

     LOAD_COMMITTED_SIGNAL,
     LOAD_STARTED_SIGNAL,
     LOAD_PROGRESS_CHANGED_SIGNAL,
     LOAD_FINISHED_SIGNAL,
     LOAD_ERROR_SIGNAL,

     NAVIGATION_REQUESTED,
     POPULATE_POPUP_SIGNAL,

     NB_SIGNALS
};

static guint module_ftp_signals[NB_SIGNALS] = { 0 };

/* Errors */
typedef enum
{
     MODULE_FTP_ERROR_CURL_INIT,
     MODULE_FTP_ERROR_CONNECT,
     MODULE_FTP_ERROR_FAILED
} ModuleFtpError;

#define MODULE_FTP_ERROR      (module_ftp_error_quark ())

static GQuark module_ftp_error_quark (void)
{
     static GQuark module_ftp_error = 0;

     if (!module_ftp_error)
     {
          module_ftp_error = g_quark_from_string ("MODULE_FTP_ERROR");
     }

     return module_ftp_error;
}
/* End of errors */

static GString *curl_content = NULL; /* for CURL */

static void module_ftp_class_init (ModuleFtpClass *class);
static void module_ftp_init (ModuleFtp *obj);

static void module_ftp_item_activated_cb (ModuleFtp *obj, GtkTreePath *path, gpointer data);
static gchar **cream_strsplit (gchar *str, gchar *delim);
static size_t module_ftp_curl_write_data_func (void *buffer, size_t size, size_t nmemb, void *stream);
static int module_ftp_curl_progress_func (ModuleFtp *obj, double dl_total, double dl_now, double up_total, double up_now);
static void module_ftp_split_curl_data (ModuleFtp *obj);
static GtkTreeModel *module_ftp_create_model (ModuleFtp *obj);
static gpointer thread_loading_ftp (gpointer data);

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

          module_ftp_type = gtk_type_unique (GTK_TYPE_ICON_VIEW, &module_ftp_info);
     }

     return module_ftp_type;
}

static void module_ftp_class_init (ModuleFtpClass *class)
{
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

     module_ftp_signals[LOAD_COMMITTED_SIGNAL] = g_signal_new (
          "load-committed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, load_committed),
          NULL, NULL,
          g_cclosure_marshal_VOID__VOID,
          G_TYPE_NONE,
          0, G_TYPE_NONE);

     module_ftp_signals[LOAD_STARTED_SIGNAL] = g_signal_new (
          "load-started",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, load_started),
          NULL, NULL,
          g_cclosure_marshal_VOID__VOID,
          G_TYPE_NONE,
          0, G_TYPE_NONE);

     module_ftp_signals[LOAD_PROGRESS_CHANGED_SIGNAL] = g_signal_new (
          "load-progress-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, load_progress_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__INT,
          G_TYPE_NONE,
          1, G_TYPE_INT);

     module_ftp_signals[LOAD_FINISHED_SIGNAL] = g_signal_new (
          "load-finished",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, load_finished),
          NULL, NULL,
          g_cclosure_marshal_VOID__VOID,
          G_TYPE_NONE,
          0, G_TYPE_NONE);

     module_ftp_signals[LOAD_ERROR_SIGNAL] = g_signal_new (
          "load-error",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleFtpClass, load_error),
          NULL, NULL,
          g_cclosure_user_marshal_VOID__STRING_POINTER,
          G_TYPE_NONE,
          2, G_TYPE_STRING, G_TYPE_POINTER);

}

static void module_ftp_init (ModuleFtp *obj)
{
     obj->uri = NULL;
     obj->user = NULL;
     obj->passwd = NULL;
     obj->host = NULL;
     obj->dir = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->files = NULL;
     obj->load_status = MODULE_FTP_LOAD_PROVISIONAL;
}

GtkWidget *module_ftp_new (void)
{
     return gtk_type_new (module_ftp_get_type ());
}

void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
{
     GThread *th_loading;

     obj->load_status = MODULE_FTP_LOAD_COMMITTED;
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[LOAD_COMMITTED_SIGNAL],
          0
     );

     if (obj->uri)
          g_free (obj->uri);
     obj->uri = g_strdup (uri);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[URI_CHANGED_SIGNAL],
          0, obj->uri
     );

     th_loading = g_thread_create (thread_loading_ftp, obj, FALSE, NULL);

     g_signal_connect (G_OBJECT (obj), "item-activated", G_CALLBACK (module_ftp_item_activated_cb), NULL);
}

/* signals */
static void module_ftp_item_activated_cb (ModuleFtp *obj, GtkTreePath *path, gpointer data)
{
     GtkTreeIter iter;
     GtkTreeModel *model = gtk_icon_view_get_model (GTK_ICON_VIEW (obj));
     gchar *filepath;

     gtk_tree_model_get_iter (model, &iter, path);
     gtk_tree_model_get (model, &iter, 0, &filepath, -1);

     module_ftp_load_uri (obj, g_strconcat (obj->uri, "/", filepath, NULL));
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

static size_t module_ftp_curl_write_data_func (void *buffer, size_t size, size_t nmemb, void *stream)
{
     if (curl_content)
          curl_content = g_string_append (curl_content, (gchar *) buffer);
     else
          curl_content = g_string_new ((gchar *) buffer);

     return (size_t) (size * nmemb);
}

static int module_ftp_curl_progress_func (ModuleFtp *obj, double dl_total, double dl_now, double up_total, double up_now)
{
     obj->progress = (gint) (dl_now * 100.0 / dl_total);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[LOAD_PROGRESS_CHANGED_SIGNAL],
          0, obj->progress
     );

     return 0;
}

static void module_ftp_split_curl_data (ModuleFtp *obj)
{
     gchar *tmp = g_string_free (curl_content, FALSE);
     gchar **lines = cream_strsplit (tmp, "\n");
     int i, len = g_strv_length (lines);

     /* lines format :
      * <access> <number of files> <user owner> <group owner> <size> <month> <day> <year> <file>
      */
     g_list_free (obj->files);
     for (i = 0; i < len; ++i)
     {
          FtpFile *el = g_malloc (sizeof (FtpFile));
          gchar **cols = cream_strsplit (lines[i], " ");

          el->is_dir = (cols[0][0] == 'd');
          el->size = (size_t) g_ascii_strtoll (cols[4], NULL, 10);
          el->path = g_strdup (cols[8]);

          /* add to the list */
          obj->files = g_list_append (obj->files, el);
     }

     g_strfreev (lines);
}

static GtkTreeModel *module_ftp_create_model (ModuleFtp *obj)
{
     GtkListStore *list_store;
     GdkPixbuf *file, *folder;
     GtkTreeIter iter;
     int i;

     file = gdk_pixbuf_new_from_file (g_build_filename (g_get_home_dir (), ".cream-browser", "icons", "file.png", NULL), NULL);
     folder = gdk_pixbuf_new_from_file (g_build_filename (g_get_home_dir (), ".cream-browser", "icons", "folder.png", NULL), NULL);

     list_store = gtk_list_store_new (2, G_TYPE_STRING, GDK_TYPE_PIXBUF);

     for (i = 0; i < g_list_length (obj->files); ++i)
     {
          FtpFile *el = g_list_nth_data (obj->files, i);

          gtk_list_store_append (list_store, &iter);
          gtk_list_store_set (list_store, &iter,
                    0, el->path,
                    1, (el->is_dir ? folder : file),
                    -1);
     }

     return GTK_TREE_MODEL (list_store);

}

static gpointer thread_loading_ftp (gpointer data)
{
     ModuleFtp *obj = (ModuleFtp *) data;
     gchar *tmp, *at, *point, *slash;
     GError *error = NULL;
     CURL *curl;
     CURLcode res;

     g_return_val_if_fail (MODULE_IS_FTP (obj), NULL);

     /* Parse URL to get information (username, password, hostname, folder)
      * ftp://[username[:password]@]hostname.domain/directory
      */
     tmp = obj->uri + strlen ("ftp://");
     if ((at = strchr (tmp, '@')))
     {
          *at = 0;
          if ((point = strchr (tmp, ':')))
          {
               *point = 0;
               obj->user = g_strdup (tmp);
               obj->passwd = g_strdup (point + 1);
          }

          tmp = at + 1;
     }

     if ((slash = strchr (tmp, '/')))
     {
          *slash = 0;
          obj->host = g_strdup (tmp);
          *slash = '/';
          obj->dir = g_strdup (slash);
     }
     else
     {
          obj->host = g_strdup (tmp);
     }

     /* Change object properties */
     obj->title = g_strdup_printf ("%s - %s", obj->host, obj->dir);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[NEW_TITLE_SIGNAL],
          0, obj->title
     );

     obj->status = g_strdup_printf ("Connecting to %s ...", obj->host);
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );

     /* Start loading */
     obj->load_status = MODULE_FTP_LOAD_STARTED;
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[LOAD_STARTED_SIGNAL],
          0
     );

     /* connect to FTP with CURL */
     curl_global_init (CURL_GLOBAL_DEFAULT);
     curl = curl_easy_init ();
     if (curl)
     {
          curl_easy_setopt (curl, CURLOPT_URL, g_strconcat ("ftp://", obj->host, (obj->dir ? obj->dir : "/"), NULL));
          if (obj->user)
               curl_easy_setopt (curl, CURLOPT_USERNAME, obj->user);
          if (obj->passwd)
               curl_easy_setopt (curl, CURLOPT_PASSWORD, obj->passwd);

          curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, module_ftp_curl_write_data_func);
          curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0L);
          curl_easy_setopt (curl, CURLOPT_PROGRESSFUNCTION, module_ftp_curl_progress_func);
          curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, obj);

          res = curl_easy_perform (curl);
          curl_easy_cleanup (curl);

          if (CURLE_OK != res)
          {
               error = g_error_new (MODULE_FTP_ERROR, MODULE_FTP_ERROR_CONNECT, "Error %d: Failed to open connection with %s", res, obj->host);
               obj->load_status = MODULE_FTP_LOAD_FAILED;
               g_signal_emit (
                    G_OBJECT (obj),
                    module_ftp_signals[LOAD_ERROR_SIGNAL],
                    0, obj->uri, error
               );
               return NULL;
          }
          else
          {
               module_ftp_split_curl_data (obj);
          }
     }
     else
     {
          error = g_error_new (MODULE_FTP_ERROR, MODULE_FTP_ERROR_CONNECT, "Failed to init CURL");
          obj->load_status = MODULE_FTP_LOAD_FAILED;
          g_signal_emit (
               G_OBJECT (obj),
               module_ftp_signals[LOAD_ERROR_SIGNAL],
               0, obj->uri, error
          );
          return NULL;
     }

     curl_global_cleanup ();

     /* add files to the iconview */
     gtk_icon_view_set_model (GTK_ICON_VIEW (obj), module_ftp_create_model (obj));
     gtk_icon_view_set_text_column (GTK_ICON_VIEW (obj), 0);
     gtk_icon_view_set_pixbuf_column (GTK_ICON_VIEW (obj), 1);

     obj->load_status = MODULE_FTP_LOAD_FINISHED;
     g_signal_emit (
          G_OBJECT (obj),
          module_ftp_signals[LOAD_FINISHED_SIGNAL],
          0
     );

     return NULL;
}
