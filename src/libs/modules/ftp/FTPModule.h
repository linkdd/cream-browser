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
  \file FTPModule.h
  \brief Module FTP header
  \author David Delassus
 */

#ifndef __CLASS_FTP_MODULE_H
#define __CLASS_FTP_MODULE_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <webkit/webkit.h>

G_BEGIN_DECLS

#define MODULE_FTP(obj)            \
     GTK_CHECK_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_FTP_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_IS_FTP(obj)         \
     GTK_CHECK_TYPE (obj, module_ftp_get_type ())

typedef enum
{
     MODULE_FTP_LOAD_PROVISIONAL,
     MODULE_FTP_LOAD_COMMITTED,
     MODULE_FTP_LOAD_STARTED,
     MODULE_FTP_LOAD_FINISHED,
     MODULE_FTP_LOAD_FAILED
} ModuleFtpLoadStatus;

typedef struct
{
     gchar *path;
     size_t size;
     gboolean is_dir;
} FtpFile;

typedef struct _ModuleFtp ModuleFtp;
typedef struct _ModuleFtpClass ModuleFtpClass;

struct _ModuleFtp
{
     GtkIconView parent;

     gchar *uri;
     gchar *user;
     gchar *passwd;
     gchar *host;
     gchar *dir;

     gchar *title;
     gchar *status;

     GList *files;
     ModuleFtpLoadStatus load_status;
     gint progress;
};

struct _ModuleFtpClass
{
     GtkIconViewClass parent_class;

     void (*uri_changed) (ModuleFtp *obj, gchar *uri);
     void (*new_title) (ModuleFtp *obj, gchar *title);
     void (*status_changed) (ModuleFtp *obj, gchar *status);
     gboolean (*new_download) (ModuleFtp *obj, WebKitDownload *download);
     gboolean (*switch_module) (ModuleFtp *obj, gchar *new_uri);

     void (*load_committed) (ModuleFtp *obj);
     void (*load_started) (ModuleFtp *obj);
     void (*load_progress_changed) (ModuleFtp *obj, gint progress);
     void (*load_finished) (ModuleFtp *obj);
     void (*load_error) (ModuleFtp *obj, gchar *uri, gpointer error);
};

GtkType module_ftp_get_type (void);
GtkWidget *module_ftp_new (void);

void module_ftp_load_uri (ModuleFtp *obj, gchar *uri);

G_END_DECLS

#endif /* __CLASS_FTP_MODULE_H */
