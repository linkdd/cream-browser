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

#include "local.h"

/*!
 * \addtogroup notebook-tab-label
 * @{
 */

G_DEFINE_TYPE (NotebookTabLabel, notebook_tab_label, GTK_TYPE_HBOX);

static void notebook_tab_label_class_init (NotebookTabLabelClass *klass)
{
     return;
}

static void notebook_tab_label_init (NotebookTabLabel *self)
{
     gtk_box_set_homogeneous (GTK_BOX (self), FALSE);
     gtk_box_set_spacing (GTK_BOX (self), 2);

     self->img = gtk_image_new_from_stock (GTK_STOCK_MISSING_IMAGE, GTK_ICON_SIZE_MENU);
     self->lbl = gtk_label_new ("");

     gtk_box_pack_start (GTK_BOX (self), self->img, FALSE, TRUE, 0);
     gtk_box_pack_start (GTK_BOX (self), self->lbl, TRUE, TRUE, 5);
}

/* methods */

/*!
 * \public \memberof NotebookTabLabel
 * \fn void notebook_tab_label_set_pixbuf (NotebookTabLabel *obj, GdkPixbuf *pixbuf)
 * @param obj A #NotebookTabLabel object.
 * @param pixbuf New pixbuf to set as favicon.
 *
 * Set a new pixbuf as favicon.
 */
void notebook_tab_label_set_pixbuf (NotebookTabLabel *obj, GdkPixbuf *pixbuf)
{
     g_return_if_fail (CREAM_IS_NOTEBOOK_TAB_LABEL (obj));
     gtk_image_set_from_pixbuf (GTK_IMAGE (obj->img), gdk_pixbuf_scale_simple (pixbuf, 12, 12, GDK_INTERP_BILINEAR));
}

/*!
 * \public \memberof NotebookTabLabel
 * \fn void notebook_tab_label_set_text (NotebookTabLabel *obj, const char *text)
 * @param obj A #NotebookTabLabel object.
 * @param text Text to set in #NotebookTabLabel.
 *
 * Set a new text into the #NotebookTabLabel,
 */
void notebook_tab_label_set_text (NotebookTabLabel *obj, const char *text)
{
     g_return_if_fail (CREAM_IS_NOTEBOOK_TAB_LABEL (obj));
     gtk_label_set_text (GTK_LABEL (obj->lbl), text);
}

/*! @} */
