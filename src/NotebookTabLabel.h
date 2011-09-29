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

#ifndef __CLASS_NOTEBOOK_TAB_LABEL_H
#define __CLASS_NOTEBOOK_TAB_LABEL_H

/*!
 * \defgroup notebook-tab-label NotebookTabLabel
 * \ingroup notebook
 * NotebookTabLabel class definition
 * @{
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CREAM_TYPE_NOTEBOOK_TAB_LABEL             (notebook_tab_label_get_type ())
#define CREAM_NOTEBOOK_TAB_LABEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_NOTEBOOK_TAB_LABEL, NotebookTabLabel))
#define CREAM_IS_NOTEBOOK_TAB_LABEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_NOTEBOOK_TAB_LABEL))
#define CREAM_NOTEBOOK_TAB_LABEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_NOTEBOOK_TAB_LABEL, NotebookTabLabelClass))

typedef struct _NotebookTabLabel NotebookTabLabel;
typedef struct _NotebookTabLabelClass NotebookTabLabelClass;

/*!
 * \class NotebookTabLabel
 * Label of a notebook's page.
 */
struct _NotebookTabLabel
{
     GtkHBox parent;

     GtkWidget *img;
     GtkWidget *lbl;
};

struct _NotebookTabLabelClass
{
     GtkHBoxClass parent;
};

GType notebook_tab_label_get_type (void);

void notebook_tab_label_set_pixbuf (NotebookTabLabel *obj, GdkPixbuf *pixbuf);
void notebook_tab_label_set_text (NotebookTabLabel *obj, const char *text);

G_END_DECLS

/*! @} */

#endif /* __CLASS_NOTEBOOK_TAB_LABEL_H */
