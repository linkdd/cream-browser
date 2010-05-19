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
  @defgroup FTP Module FTP
  @ingroup modules
  @brief Object which show a FTP repository

  \todo Rewrite FTP Module

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

static void module_ftp_class_init (ModuleFtpClass *class);
static void module_ftp_init (ModuleFtp *obj);

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
     return GTK_WIDGET (obj);
}

/*!
  \fn void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
  \brief Load a new URI

  \param obj ModuleFtp object
  \param uri New URI to load
 */
void module_ftp_load_uri (ModuleFtp *obj, gchar *uri)
{
     ;
}

/*! @} */
