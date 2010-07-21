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

#ifndef __CLASS_CREAM_TABBED_H
#define __CLASS_CREAM_TABBED_H

/*!
  @addtogroup CreamTabbed
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

#include <CreamView.h>
#include "Completion.h"

G_BEGIN_DECLS

#define CREAM_TABBED(obj)          \
     GTK_CHECK_CAST(obj, cream_tabbed_get_type (), CreamTabbed)
#define CREAM_TABBED_CLASS(klass)  \
     GTK_CHECK_CLASS_CAST(klass, cream_tabbed_get_type (), CreamTabbedClass)
#define IS_CREAM_TABBED(obj)       \
     GTK_CHECK_TYPE(obj, cream_tabbed_get_type ())

/*! \typedef struct _CreamTabbed CreamTabbed */
/*! \typedef struct _CreamTabbedClass CreamTabbedClass */

typedef struct _CreamTabbed CreamTabbed;
typedef struct _CreamTabbedClass CreamTabbedClass;

/*!
  \struct _CreamTabbed
  \implements CreamView
  \implements Completion
  \todo Split the view when opening the WebInspector (instead of opening in a new window)
 */
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

     GtkWidget *comp;              /*!< Completion for the inputbox */
     GtkWidget *inputbox;          /*!< Inputbox of the CreamView object */

     GtkAdjustment *adjust_v;
};

/*! \struct _CreamTabbedClass */
struct _CreamTabbedClass
{
     GtkVBoxClass parent_class;    /*!< Parent class */

     void (*update_notebook_title) (CreamTabbed *obj);
     /*!@signal The "update-notebook-title" signal is emitted when the CreamView's title changed */
};

GtkType cream_tabbed_get_type (void);
GtkWidget *cream_tabbed_new (void);

void cream_tabbed_load_uri (CreamTabbed *obj, const gchar *uri);

const gchar *cream_tabbed_get_uri (CreamTabbed *obj);
const gchar *cream_tabbed_get_title (CreamTabbed *obj);
const gchar *cream_tabbed_get_status (CreamTabbed *obj);
GdkPixbuf *cream_tabbed_get_favicon (CreamTabbed *obj);

void echo (CreamTabbed *obj, const gchar *format, ...);

G_END_DECLS

/*! @} */

#endif /* __CLASS_CREAM_TABBED_H */
