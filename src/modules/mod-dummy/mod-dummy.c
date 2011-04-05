#include "mod-dummy.h"

/* Initialize function */
static void dummy_init (gpointer data)
{
     printf ("Initialization of dummy.\n");
     printf ("Data: %p - %s\n", data, (char *) data);
}

/* Unload function */
static void dummy_unload (gpointer data)
{
     printf ("Unloading of dummy.\n");
     printf ("Data: %p - %s\n", data, (char *) data);
}

/* WebView new function */
static GtkWidget *dummy_webview_new (gpointer data)
{
     GtkWidget *ret = gtk_label_new ("dummy module");
     printf ("Data: %p - %s\n", data, (char *) data);
     return ret;
}

/* A function callable with the call() function */
static void dummy_set (GList *args, gpointer ret, gpointer data)
{
     g_return_if_fail (g_list_length (args) >= 2);

     GtkWidget *lbl  = (GtkWidget *) g_list_nth_data (args, 1);
     const char *str = (const char*) g_list_nth_data (args, 2);

     gtk_label_set_text (GTK_LABEL (lbl), str);

     printf ("Data: %p - %s\n", data, (char *) data);

     if (ret)
          *((char **) ret) = (char *) data;
}

/* main program */
void module_main (void)
{
     char *str = ":)";

     set_init_func (dummy_init, str);
     set_unload_func (dummy_unload, str);
     set_webview_new_func (dummy_webview_new, str);

     add_call_function ("set", dummy_set, str);
}
