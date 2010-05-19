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

#ifndef __CLASS_FTP_MODULE_H
#define __CLASS_FTP_MODULE_H

/*!
  @addtogroup FTP
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <CreamFtp.h>
#include <webkit/webkit.h>

G_BEGIN_DECLS

#define MODULE_FTP(obj)            \
     GTK_CHECK_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_FTP_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (obj, module_ftp_get_type (), ModuleFtp)
#define MODULE_IS_FTP(obj)         \
     GTK_CHECK_TYPE (obj, module_ftp_get_type ())

typedef struct _ModuleFtp ModuleFtp;
typedef struct _ModuleFtpClass ModuleFtpClass;

/*!
  \struct _ModuleFtp
  \implements CreamFtp
 */
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

/*! @} */

#endif /* __CLASS_FTP_MODULE_H */
