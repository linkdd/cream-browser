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
 * \addtogroup split
 * @{
 */

/*!
 * \public \memberof SplittedWindow
 * \fn SplittedWindow *splittedwindow_new (void)
 * @return A #SplittedWindow object.
 *
 * Create a new #SplittedWindow object.
 */
SplittedWindow *splittedwindow_new (void)
{
     SplittedWindow *sw = g_malloc (sizeof (SplittedWindow));

     sw->root         = g_malloc (sizeof (SplittedWindowObject));
     sw->root->parent = NULL;
     sw->root->c1     = NULL;
     sw->root->c2     = NULL;
     sw->root->widget = notebook_new ();
     sw->focus        = sw->root;

     return sw;
}

/*!
 * \public \memberof SplittedWindow
 * \fn void splittedwindow_vsplit (SplittedWindow *sw, GtkWidget *nb)
 * @param sw A #SplittedWindow object.
 * @param nb A #Notebook object.
 *
 * Make a vertical split.
 */
void splittedwindow_vsplit (SplittedWindow *sw, GtkWidget *nb)
{
     SplittedWindowObject *c1 = g_malloc (sizeof (SplittedWindowObject));
     SplittedWindowObject *c2 = g_malloc (sizeof (SplittedWindowObject));

     c1->widget = nb;
     c1->parent = sw->focus;
     c1->c1     = NULL;
     c1->c2     = NULL;

     c2->widget = sw->focus->widget;
     c2->parent = sw->focus;
     c2->c1     = NULL;
     c2->c2     = NULL;

     sw->focus->c1 = c1;
     sw->focus->c2 = c2;

     sw->focus->widget = gtk_hpaned_new ();
     gtk_paned_add1 (GTK_PANED (sw->focus->widget), c1->widget);
     gtk_paned_add2 (GTK_PANED (sw->focus->widget), c2->widget);

     sw->focus = c2;
}

/*!
 * \public \memberof SplittedWindow
 * \fn void splittedwindow_split (SplittedWindow *sw, GtkWidget *nb)
 * @param sw A #SplittedWindow object.
 * @param nb A #Notebook object.
 *
 * Make a horizontal split.
 */
void splittedwindow_split (SplittedWindow *sw, GtkWidget *nb)
{
     SplittedWindowObject *c1 = g_malloc (sizeof (SplittedWindowObject));
     SplittedWindowObject *c2 = g_malloc (sizeof (SplittedWindowObject));

     c1->widget = nb;
     c1->parent = sw->focus;
     c1->c1     = NULL;
     c1->c2     = NULL;

     c2->widget = sw->focus->widget;
     c2->parent = sw->focus;
     c2->c1     = NULL;
     c2->c2     = NULL;

     sw->focus->c1 = c1;
     sw->focus->c2 = c2;

     sw->focus->widget = gtk_vpaned_new ();
     gtk_paned_add1 (GTK_PANED (sw->focus->widget), c1->widget);
     gtk_paned_add2 (GTK_PANED (sw->focus->widget), c2->widget);

     sw->focus = c2;
}

/*!
 * \public \memberof SplittedWindow
 * \fn void splittedwindow_close (SplittedWindow *sw)
 * @param sw A #SplittedWindow object.
 *
 * Close the current focus window.
 */
void splittedwindow_close (SplittedWindow *sw)
{
     SplittedWindowObject *parent = sw->focus->parent;

     if (parent == NULL)
          return;

     gtk_widget_destroy (parent->widget);

     if (parent->c1 == sw->focus)
          parent->widget = parent->c2->widget;
     else
          parent->widget = parent->c1->widget;

     gtk_widget_destroy (sw->focus->widget);

     g_free (parent->c1), parent->c1 = NULL;
     g_free (parent->c2), parent->c2 = NULL;
     g_free (sw->focus),  sw->focus  = parent;
}

/*!
 * \public \memberof SplittedWindow
 * \fn void splittedwindow_show (SplittedWindow *sw)
 * @param sw A #SplittedWindow object.
 *
 * Show the window and its childs.
 */
void splittedwindow_show (SplittedWindow *sw)
{
     gtk_container_remove (GTK_CONTAINER (global.gui.box), global.gui.splittedwindow);
     gtk_widget_destroy (global.gui.splittedwindow);

     global.gui.splittedwindow = sw->root->widget;
     gtk_box_pack_start (GTK_BOX (global.gui.box), global.gui.splittedwindow, TRUE, TRUE, 0);
     gtk_widget_show_all (global.gui.window);
}

/*! @} */
