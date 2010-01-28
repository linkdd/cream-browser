#include "local.h"

GtkWidget *cream_interface_init (void)
{
     GdkGeometry hints = { 1, 1 }; /* GDK_HINT_MIN_SIZE for main_window */
     GtkWidget *main_window;
     GtkWidget *box;
     GtkWidget *image;
     GtkWidget *view;

     /* create main window */
     main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (main_window), "Cream-Browser");
     gtk_window_set_default_size (GTK_WINDOW (main_window), 800, 600);
     gtk_window_set_geometry_hints (GTK_WINDOW (main_window), NULL, &hints, GDK_HINT_MIN_SIZE);

     box = gtk_vbox_new (FALSE, 0);
     gtk_container_add (GTK_CONTAINER (main_window), box);

     view = cream_view_new ();
     gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);

     image = module_web_view_get_favicon (MODULE_WEB_VIEW (cream_view_get_content (CREAM_VIEW (view))))->img;
     gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
     /* restore cookies */
     global.browser.cookies = soup_cookie_jar_text_new (CREAM_FILE ("cookies.txt"), FALSE);
     soup_session_add_feature (webkit_get_default_session (), SOUP_SESSION_FEATURE (global.browser.cookies));

     /* connect signals */
     g_signal_connect (G_OBJECT (main_window), "destroy", G_CALLBACK (cb_cream_destroy), NULL);

     return main_window;
}
