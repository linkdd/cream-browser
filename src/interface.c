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

static void window_destroy (GtkWindow *window)
{
     gtk_application_remove_window (GTK_APPLICATION (app), window);
}

static void window_update (GtkVimSplit *obj)
{
     GtkWidget *webview = notebook_get_focus (CREAM_NOTEBOOK (gtk_vim_split_get_focus (obj)));

     cream_browser_set_focused_webview (app, webview);

     gchar *title = g_strdup_printf ("%s - %s", PACKAGE, webview_get_title (CREAM_WEBVIEW (webview)));
     gtk_window_set_title (GTK_WINDOW (app->gui.window), title);
     g_free (title);

     statusbar_set_link (CREAM_STATUSBAR (app->gui.statusbar), webview_get_uri (CREAM_WEBVIEW (webview)));

     ui_show ();
}

/*!
 * \fn void ui_init (void)
 * Create the main window
 */
void ui_init (void)
{
     GError *error = NULL;

     app->gui.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_default_size (GTK_WINDOW (app->gui.window), 800, 600);
     gtk_window_set_title (GTK_WINDOW (app->gui.window), PACKAGE);
     gtk_window_set_wmclass (GTK_WINDOW (app->gui.window), PACKAGE, "Navigator");

     if (!gtk_window_set_icon_from_file (GTK_WINDOW (app->gui.window), find_file (FILE_TYPE_DATA, "cream-browser.png"), &error) && error != NULL)
          CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);

     app->gui.box       = gtk_vbox_new (FALSE, 0);
     app->gui.vimsplit  = gtk_vim_split_new ();
     app->gui.inputbox  = inputbox_new ();
     app->gui.statusbar = statusbar_new ();

     statusbar_set_state (CREAM_STATUSBAR (app->gui.statusbar), CREAM_MODE_NORMAL);

     gtk_box_pack_start (GTK_BOX (app->gui.box), app->gui.vimsplit, TRUE, TRUE, 0);
     gtk_box_pack_end (GTK_BOX (app->gui.box), app->gui.inputbox, FALSE, TRUE, 0);
     gtk_box_pack_end (GTK_BOX (app->gui.box), app->gui.statusbar, FALSE, TRUE, 0);
     gtk_container_add (GTK_CONTAINER (app->gui.window), app->gui.box);

     g_signal_connect (G_OBJECT (app->gui.window),   "destroy",       G_CALLBACK (window_destroy),   NULL);
     g_signal_connect (G_OBJECT (app->gui.vimsplit), "no-more-split", G_CALLBACK (window_destroy),   NULL);
     g_signal_connect (G_OBJECT (app->gui.vimsplit), "focus-changed", G_CALLBACK (window_update),    NULL);

     gtk_application_add_window (GTK_APPLICATION (app), GTK_WINDOW (app->gui.window));
}

/*!
 * \fn void ui_show (void)
 * Show/Redraw the main window.
 */
void ui_show (void)
{
     gtk_widget_show_all (app->gui.window);
}

/*! @} */
