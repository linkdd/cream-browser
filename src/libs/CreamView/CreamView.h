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

#ifndef __CLASS_CREAM_VIEW_H
#define __CLASS_CREAM_VIEW_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <about/AboutModule.h>
#include <ftp/FTPModule.h>
#include <www/WebViewModule.h>

#include "CreamBackwardForwardList.h"

G_BEGIN_DECLS

#define CREAM_VIEW(obj)            \
     GTK_CHECK_CAST(obj, cream_view_get_type (), CreamView)
#define CREAM_VIEW_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST(klass, cream_view_get_type (), CreamViewClass)
#define CREAM_IS_CREAM_VIEW(obj)   \
     GTK_CHECK_TYPE(obj, cream_view_get_type ())

typedef struct _CreamView CreamView;
typedef struct _CreamViewClass CreamViewClass;

struct _CreamView
{
     GtkScrolledWindow scroll;

     GtkWidget *content;
     GtkAdjustment *adjust_h;
     GtkAdjustment *adjust_v;

     gchar *uri;
     gchar *title;
     gchar *status;
     gboolean view_source_mode;

     CreamBackwardForwardList *history;
};

struct _CreamViewClass
{
     GtkScrolledWindowClass parent_class;

     void (*uri_changed) (CreamView *obj, gchar *url);
     void (*new_title) (CreamView *obj, gchar *title);
     void (*status_changed) (CreamView *obj, gchar *status);
     gboolean (*new_download) (CreamView *obj, WebKitDownload *download);
};

GtkType cream_view_get_type (void);
GtkWidget *cream_view_new (void);

void cream_view_load_uri (CreamView *obj, const gchar *uri);
GtkWidget *cream_view_get_content (CreamView *obj);
gboolean cream_view_get_view_source_mode (CreamView *obj);
void cream_view_set_view_source_mode (CreamView *obj, gboolean mode);

gboolean cream_view_can_go_back (CreamView *obj);
gboolean cream_view_can_go_forward (CreamView *obj);
void cream_view_go_back (CreamView *obj);
void cream_view_go_forward (CreamView *obj);
CreamBackwardForwardList *cream_view_get_backward_forward_list (CreamView *obj);

GtkAdjustment *cream_view_get_hadjustment (CreamView *obj);
GtkAdjustment *cream_view_get_vadjustment (CreamView *obj);

const gchar *cream_view_get_uri (CreamView *obj);
const gchar *cream_view_get_title (CreamView *obj);
const gchar *cream_view_get_status (CreamView *obj);

G_END_DECLS

#endif /* __CLASS_CREAM_VIEW_H */
