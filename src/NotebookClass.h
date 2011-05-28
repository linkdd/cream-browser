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

#ifndef __NOTEBOOK_CLASS_H
#define __NOTEBOOK_CLASS_H

/*!
 * \defgroup notebook Notebook
 * Notebook class definition
 * @{
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define NOTEBOOK(obj)              G_TYPE_CHECK_INSTANCE_CAST(obj, notebook_get_type (), Notebook)
#define NOTEBOOK_CLASS(klass)      G_TYPE_CHECK_CLASS_CAST(klass, notebook_get_type (), NotebookClass)
#define IS_NOTEBOOK(obj)           G_TYPE_CHECK_INSTANCE_TYPE(obj, notebook_get_type ())

typedef struct _Notebook Notebook;
typedef struct _NotebookClass NotebookClass;

/*!
 * \class Notebook
 */
struct _Notebook
{
     /*< private >*/
     GtkNotebook parent;

     GtkWidget *focus;
     GList *webviews;
};

struct _NotebookClass
{
     /*< private >*/
     GtkNotebookClass parent;
};

GtkWidget *notebook_new (void);

#endif /* __NOTEBOOK_CLASS_H */
