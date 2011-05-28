#include "local.h"

G_DEFINE_TYPE (Notebook, notebook, GTK_TYPE_NOTEBOOK);

GtkWidget *notebook_new (void)
{
     Notebook *n = g_object_new (notebook_get_type (), NULL);

     g_return_val_if_fail (n != NULL, NULL);

     return GTK_WIDGET (n);
}

static void notebook_class_init (NotebookClass *klass)
{
     return;
}

static void notebook_init (Notebook *obj)
{
     obj->focus = NULL;
     obj->webviews = NULL;
}
