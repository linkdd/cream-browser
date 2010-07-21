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

#ifndef __CLASS_COMPLETION_H
#define __CLASS_COMPLETION_H

/*!
  @addtogroup Completion
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define COMPLETION(obj)            \
     GTK_CHECK_CAST(obj, completion_get_type (), Completion)
#define COMPLETION_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST(klass, completion_get_type (), Completion)
#define IS_COMPLETION(obj)         \
     GTK_CHECK_TYPE(obj, completion_get_type ())

/*! \typedef struct _Completion Completion */
/*! \typedef struct _CompletionClass CompletionClass */

typedef struct _Completion Completion;
typedef struct _CompletionClass CompletionClass;

/*!
  \struct _Completion
  \implements GCompletion
 */
struct _Completion
{
     GtkVBox parent;          /*!< Parent instance */
     GList *widgets;          /*!< List of widgets, one widget for one item in completion list */

     GCompletion *comp;       /*!< Object for completion */
};

/*! \struct _CompletionClass */
struct _CompletionClass
{
     GtkVBoxClass parent_class;
};

GtkType completion_get_type (void);
GtkWidget *completion_new (void);

void completion_update (Completion *obj, GList *items, gchar *prefix);

/*! @} */

#endif /* __CLASS_COMPLETION_H */
