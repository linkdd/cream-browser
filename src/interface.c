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
 * \addtogroup interface
 * @{
 */

static void window_destroy (GtkWidget *window, gpointer data)
{
     exit (EXIT_SUCCESS);
}

/*!
 * \fn void ui_init (void)
 * Create the main window.
 */
void ui_init (void)
{
     global.gui.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (global.gui.window), PACKAGE);
     g_signal_connect (G_OBJECT (global.gui.window), "destroy", G_CALLBACK (window_destroy), NULL);

     global.gui.sw  = splittedwindow_new ();
     global.gui.box = gtk_vbox_new (FALSE, 0);

     global.gui.splittedwindow = global.gui.sw->root->widget;
     gtk_box_pack_start (GTK_BOX (global.gui.box), global.gui.splittedwindow, TRUE, TRUE, 0);

     gtk_container_add (GTK_CONTAINER (global.gui.window), global.gui.box);
}

/*!
 * \fn void ui_show (void)
 * Show/Redraw the main window.
 */
void ui_show (void)
{
     gtk_widget_show_all (global.gui.window);
}

/*! @} */
