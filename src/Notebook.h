/*
* Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __CLASS_NOTEBOOK_H
#define __CLASS_NOTEBOOK_H

/*!
 * \defgroup notebook Notebook
 * \ingroup interface
 * Notebook class definition
 * @{
 */

#include <gtk/gtk.h>

#include "NotebookTabLabel.h"
#include "Statusbar.h"

G_BEGIN_DECLS

#define CREAM_TYPE_NOTEBOOK             (notebook_get_type ())
#define CREAM_NOTEBOOK(obj)             (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_NOTEBOOK, Notebook))
#define CREAM_IS_NOTEBOOK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_NOTEBOOK))
#define CREAM_NOTEBOOK_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_NOTEBOOK, NotebookClass))

typedef struct _Notebook Notebook;
typedef struct _NotebookClass NotebookClass;

/*!
 * \class Notebook
 * Manage notebook.
 */
struct _Notebook
{
     /*< private >*/
     GtkNotebook parent;

     GtkWidget *focus;   /*!< Focused widget */
     GList *webviews;    /*!< List of webviews opened */
};

struct _NotebookClass
{
     /*< private >*/
     GtkNotebookClass parent;
};

GType notebook_get_type (void);
GtkWidget *notebook_new (void);

GtkWidget *notebook_get_focus (Notebook *obj);
void notebook_open (Notebook *obj, const gchar *url);
void notebook_tabopen (Notebook *obj, const gchar *url);
void notebook_close (Notebook *obj, gint page);

G_END_DECLS

/*! @} */

#endif /* __CLASS_NOTEBOOK_H */
