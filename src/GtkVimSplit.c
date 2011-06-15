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

/* signals */

enum
{
     GTK_VIM_SPLIT_NO_MORE_SPLIT_SIGNAL,
     GTK_VIM_SPLIT_NB_SIGNALS
};

static guint gtk_vim_split_signals[GTK_VIM_SPLIT_NB_SIGNALS] = { 0 };

G_DEFINE_TYPE (GtkVimSplit, gtk_vim_split, GTK_TYPE_EVENT_BOX)

/* Constructors */

/*!
 * \public \memberof GtkVimSplit
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
     /* signals */
     gtk_vim_split_signals[GTK_VIM_SPLIT_NO_MORE_SPLIT_SIGNAL] = g_signal_new (
               "no-more-split",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (GtkVimSplitClass, no_more_split),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0);
}

static void gtk_vim_split_init (GtkVimSplit *obj)
{
     obj->focus   = NULL;
     obj->widgets = NULL;
}

/* Methods */

/*!
 * \public \memberof GtkVimSplit
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
 * \public \memberof GtkVimSplit
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
 * \public \memberof GtkVimSplit
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
     g_return_if_fail (g_list_find (obj->widgets, child) == NULL);

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

/*!
 * \public \memberof GtkVimSplit
 * \fn void gtk_vim_split_close (GtkVimSplit *obj)
 * @param obj A #GtkVimSplit widget.
 *
 * Close the focused notebook.
 */
void gtk_vim_split_close (GtkVimSplit *obj)
{
     GtkWidget *parent;

     g_return_if_fail (GTK_IS_VIM_SPLIT (obj));
     g_return_if_fail (obj->focus != NULL);

     parent = gtk_widget_get_parent (obj->focus);
     if (parent == GTK_WIDGET (obj))
     {
          g_signal_emit (G_OBJECT (obj), gtk_vim_split_signals[GTK_VIM_SPLIT_NO_MORE_SPLIT_SIGNAL], 0);
     }
     else
     {
          GtkWidget *new_parent = gtk_widget_get_parent (parent);
          GtkWidget *reparent;

          /* Here we are in a GtkPaned with two widgets,
           * obj->focus and reparent.
           *
           * We are closing obj->focus and going to set the
           * GtkPaned's parent as the parent widget of reparent.
           */
          if (obj->focus == gtk_paned_get_child1 (GTK_PANED (parent)))
               reparent = gtk_paned_get_child2 (GTK_PANED (parent));
          else
               reparent = gtk_paned_get_child1 (GTK_PANED (parent));

          /* remove widgets from container */
          obj->widgets = g_list_remove (obj->widgets, obj->focus);
          gtk_container_remove (GTK_CONTAINER (parent), obj->focus);
          gtk_container_remove (GTK_CONTAINER (parent), reparent);

          if (GTK_IS_VIM_SPLIT (new_parent))
          {
               gtk_container_remove (GTK_CONTAINER (new_parent), parent);
               gtk_container_add (GTK_CONTAINER (new_parent), reparent);
          }
          else if (GTK_IS_PANED (new_parent))
          {
               /* if the GtkPaned's parent is a GtkPaned too, check in
                * which side we are
                */
               if (parent == gtk_paned_get_child1 (GTK_PANED (new_parent)))
               {
                    gtk_container_remove (GTK_CONTAINER (new_parent), parent);
                    gtk_paned_pack1 (GTK_PANED (new_parent), reparent, TRUE, TRUE);
               }
               else
               {
                    gtk_container_remove (GTK_CONTAINER (new_parent), parent);
                    gtk_paned_pack2 (GTK_PANED (new_parent), reparent, TRUE, TRUE);
               }
          }

          obj->focus = GTK_WIDGET (g_list_nth_data (obj->widgets, 0));
     }
}

/*! @} */
