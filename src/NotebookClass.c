#include "local.h"

/*!
 * \addtogroup notebook
 * @{
 */

G_DEFINE_TYPE (Notebook, notebook, GTK_TYPE_NOTEBOOK);

/*!
 * \public \memberof Notebook
 * \fn GtkWidget *notebook_new (void)
 * @return A new #Notebook object.
 *
 * Create a new #Notebook object.
 */
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

/* methods */

/*!
 * \public \memberof Notebook
 * \fn GtkWidget *notebook_get_focus (Notebook *obj)
 * @param obj A #Notebook object.
 * @return A #WebView object.
 *
 * Return the current focused webview.
 */
GtkWidget *notebook_get_focus (Notebook *obj)
{
     return (obj != NULL ? obj->focus : NULL);
}



/*! @} */
