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

#include <CurlClient.h>
#include <webkit/webkit.h>
#include <gnet.h>

G_BEGIN_DECLS

#define MODULE_FTP(obj)            \
     GTK_CHECK_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_FTP_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_IS_FTP(obj)         \
     GTK_CHECK_TYPE (obj, module_ftp_get_type ())

typedef struct _ModuleFtp ModuleFtp;
typedef struct _ModuleFtpClass ModuleFtpClass;

/*! \struct _ModuleFtp */
struct _ModuleFtp
{
     GtkTreeView parent;           /*!< Parent instance */

     gchar *uri;                   /*!< URI loaded by CurlClient */
     gchar *title;                 /*!< Title of the loaded URI */
     gchar *status;                /*!< Status message of the loaded URI */

     gboolean view_source_mode;    /*!< TRUE if you want to see the page's source */
};

/*! \struct _ModuleFtpClass */
struct _ModuleFtpClass
{
     GtkTreeViewClass parent_class;     /*!< Parent class */

     void (*uri_changed) (ModuleFtp *obj, gchar *uri);
     void (*new_title) (ModuleFtp *obj, gchar *title);
     void (*status_changed) (ModuleFtp *obj, gchar *status);
     gboolean (*new_download) (ModuleFtp *obj, WebKitDownload *download);
     gboolean (*switch_module) (ModuleFtp *obj, gchar *new_uri);
};

GtkType module_ftp_get_type (void);
GtkWidget *module_ftp_new (void);

void module_ftp_load_uri (ModuleFtp *obj, gchar *uri);

G_END_DECLS

#endif /* __CLASS_FTP_MODULE_H */
