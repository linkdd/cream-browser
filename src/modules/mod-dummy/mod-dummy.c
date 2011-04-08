#include "mod-dummy.h"

/* Initialize function */
static void dummy_init (gpointer data)
{
     printf ("Initialization of dummy.\n");
}

/* Unload function */
static void dummy_unload (gpointer data)
{
     printf ("Unloading of dummy.\n");
}

/* WebView new function */
static GtkWidget *dummy_webview_new (gpointer data)
{
     GtkWidget *ret = gtk_label_new ("dummy module");
     return ret;
}

/* A function callable with the call() function */
static void dummy_load_uri (GList *args, gpointer ret, gpointer data)
{
     GtkWidget *label = NULL;
     UriScheme *uri = NULL;

     g_return_if_fail (g_list_length (args) >= 2);

     label = (GtkWidget *) g_list_nth_data (args, 1);
     uri   = (UriScheme *) g_list_nth_data (args, 2);

     gchar *markup = g_markup_printf_escaped ("<span style=\"bold\">%s</span>\n<span style=\"italic\">%s</span>\n%s",
                                              uri->scheme, uri->userinfo, uri->hostname);

     gtk_label_set_markup (GTK_LABEL (label), markup);

     g_free (markup);
}

/* main program */
void module_main (void)
{
     set_init_func (dummy_init, NULL);
     set_unload_func (dummy_unload, NULL);
     set_webview_new_func (dummy_webview_new, NULL);

     add_call_function ("load-uri", dummy_load_uri, NULL);
}
