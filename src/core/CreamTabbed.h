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
  \file CreamTabbed.h
  \brief CreamTabbed object header
  \author David Delassus
 */

#ifndef __CLASS_CREAM_TABBED_H
#define __CLASS_CREAM_TABBED_H

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <CreamView.h>

G_BEGIN_DECLS

#define CREAM_TABBED(obj)          \
     GTK_CHECK_CAST(obj, cream_tabbed_get_type (), CreamTabbed)
#define CREAM_TABBED_CLASS(klass)  \
     GTK_CHECK_CLASS_CAST(klass, cream_tabbed_get_type (), CreamTabbedClass)
#define IS_CREAM_TABBED(obj)       \
     GTK_CHECK_TYPE(obj, cream_tabbed_get_type ());

typedef struct _CreamTabbed CreamTabbed;
typedef struct _CreamTabbedClass CreamTabbedClass;

/*! \struct _CreamTabbed */
struct _CreamTabbed
{
     GtkVBox parent;               /*!< Parent instance */

     GtkWidget *creamview;         /*!< CreamView object */
     struct
     {
          GtkWidget *eventbox;
          GtkWidget *box;
          GtkWidget *url;
          GtkWidget *state;
     } statusbar;                  /*!< Statusbar of the CreamView object */

     GtkWidget *inputbox;          /*!< Inputbox of the CreamView object */

     GtkAdjustment *adjust_v;
};

/*! \struct _CreamTabbedClass */
struct _CreamTabbedClass
{
     GtkVBoxClass parent_class;    /*!< Parent class */

     void (*update_notebook_title) (CreamTabbed *obj);
     /*!< The "update-notebook-title" signal is emitted when the CreamView's title changed */
};

GtkType cream_tabbed_get_type (void);
GtkWidget *cream_tabbed_new (void);

void cream_tabbed_load_uri (CreamTabbed *obj, const gchar *uri);

const gchar *cream_tabbed_get_uri (CreamTabbed *obj);
const gchar *cream_tabbed_get_title (CreamTabbed *obj);
const gchar *cream_tabbed_get_status (CreamTabbed *obj);
GtkWidget *cream_tabbed_get_favicon (CreamTabbed *obj);

void echo (CreamTabbed *obj, const gchar *format, ...);

G_END_DECLS

#endif /* __CLASS_CREAM_TABBED_H */
