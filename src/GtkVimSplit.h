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

#ifndef __CLASS_GTK_VIM_SPLIT_H
#define __CLASS_GTK_VIM_SPLIT_H

/*!
 * \defgroup gtkvimsplit GtkVimSplit
 * \ingroup interface
 * Vim split widget.
 *
 * @{
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_TYPE_VIM_SPLIT              (gtk_vim_split_get_type ())
#define GTK_VIM_SPLIT(obj)              (G_TYPE_CHECK_INSTANCE_CAST (obj, GTK_TYPE_VIM_SPLIT, GtkVimSplit))
#define GTK_IS_VIM_SPLIT(obj)           (G_TYPE_CHECK_INSTANCE_TYPE (obj, GTK_TYPE_VIM_SPLIT))
#define GTK_VIM_SPLIT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST (klass, GTK_TYPE_VIM_SPLIT, GtkVimSplitClass))

typedef struct _GtkVimSplit GtkVimSplit;
typedef struct _GtkVimSplitClass GtkVimSplitClass;

/*!
 * \class GtkVimSplit
 * #GtkVimSplit object.
 */
struct _GtkVimSplit
{
     /*<private>*/
     GtkEventBox parent;

     GtkWidget *focus;             /*!< Focused widget */
     GList *widgets;               /*!< List of current widgets */
};

struct _GtkVimSplitClass
{
     GtkEventBoxClass parent;
};

GType gtk_vim_split_get_type (void);
GtkWidget *gtk_vim_split_new (void);

GtkWidget *gtk_vim_split_get_focus (GtkVimSplit *obj);
void gtk_vim_split_set_focus (GtkVimSplit *obj, GtkWidget *nb);

void gtk_vim_split_add (GtkVimSplit *obj, GtkWidget *child, GtkOrientation o);

G_END_DECLS

/*! @} */

#endif /* __CLASS_GTK_VIM_SPLIT_H */
