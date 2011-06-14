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
 * \addtogroup gtkvimsplit
 * @{
 */

G_DEFINE_TYPE (GtkVimSplit, gtk_vim_split, GTK_TYPE_EVENT_BOX)

/* Constructors */

/*!
 * \fn GtkWidget *gtk_vim_split_new (void)
 * @return A new #GtkVimSplit widget.
 *
 * Create a new #GtkVimSplit widget.
 */
GtkWidget *gtk_vim_split_new (void)
{
     return GTK_WIDGET (g_object_new (gtk_vim_split_get_type (), NULL));
}

static void gtk_vim_split_class_init (GtkVimSplitClass *klass)
{
     return;
}

static void gtk_vim_split_init (GtkVimSplit *obj)
{
     obj->focus   = NULL;
     obj->widgets = NULL;
}

/* Methods */

/*!
 * \fn GtkWidget *gtk_vim_split_get_focus (GtkVimSplit *obj)
 * @param obj A #GtkVimSplit widget.
 * @return A #Notebook widget.
 *
 * Return the current focused notebook.
 */
GtkWidget *gtk_vim_split_get_focus (GtkVimSplit *obj)
{
     g_return_val_if_fail (GTK_IS_VIM_SPLIT (obj), NULL);
     return obj->focus;
}

/*!
 * \fn void gtk_vim_split_set_focus (GtkVimSplit *obj, GtkWidget *nb)
 * @param obj A #GtkVimSplit widget.
 * @param nb A #Notebook widget.
 *
 * Set \arg nb as the current focused notebook.
 */
void gtk_vim_split_set_focus (GtkVimSplit *obj, GtkWidget *nb)
{
     g_return_if_fail (GTK_IS_VIM_SPLIT (obj));
     g_return_if_fail (CREAM_IS_NOTEBOOK (nb));
     g_return_if_fail (g_list_find (obj->widgets, nb) != NULL);

     obj->focus = nb;
     gtk_widget_grab_focus (GTK_WIDGET (nb));
}

/*!
 * \fn void gtk_vim_split_add (GtkVimSplit *obj, GtkWidget *child, GtkOrientation o)
 * @param obj A #GtkVimSplit widget.
 * @param child A #Notebook widget.
 * @param o Split orientation.
 *
 * Split the focused widget.
 */
void gtk_vim_split_add (GtkVimSplit *obj, GtkWidget *child, GtkOrientation o)
{
     g_return_if_fail (GTK_IS_VIM_SPLIT (obj));
     g_return_if_fail (CREAM_IS_NOTEBOOK (child));

     obj->widgets = g_list_append (obj->widgets, GTK_WIDGET (g_object_ref (child)));

     if (obj->focus)
     {
          GtkWidget *focus  = obj->focus;
          GtkWidget *parent = gtk_widget_get_parent (focus);
          GtkWidget *paned  = (o == GTK_ORIENTATION_VERTICAL ? gtk_vpaned_new () : gtk_hpaned_new ());

          if (GTK_IS_VIM_SPLIT (parent))
          {
               gtk_container_remove (GTK_CONTAINER (parent), focus);
               gtk_container_add (GTK_CONTAINER (parent), paned);
          }
          else if (GTK_IS_PANED (parent))
          {
               if (focus == gtk_paned_get_child1 (GTK_PANED (parent)))
               {
                    gtk_container_remove (GTK_CONTAINER (parent), focus);
                    gtk_paned_pack1 (GTK_PANED (parent), paned, TRUE, TRUE);
               }
               else
               {
                    gtk_container_remove (GTK_CONTAINER (parent), focus);
                    gtk_paned_pack2 (GTK_PANED (parent), paned, TRUE, TRUE);
               }
          }

          gtk_paned_pack1 (GTK_PANED (paned), focus, TRUE, TRUE);
          gtk_paned_pack2 (GTK_PANED (paned), child, TRUE, TRUE);
     }
     else
     {
          gtk_container_add (GTK_CONTAINER (obj), child);
     }

     obj->focus = child;
}

/*! @} */
