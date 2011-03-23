#include "local.h"

/*!
 * \addtogroup viewarea
 * @{
 */

static void viewarea_class_init (ViewAreaClass *klass);
static void viewarea_init (ViewArea *obj);

/* {{{ WebView list */
typedef struct
{
     WebView *w;
     GList *gcallbacks;
} ListWebView;

static ListWebView *listwebview_new (WebView *w)
{
     ListWebView *ret = g_malloc (sizeof (ListWebView));
     ret->w = w;
     ret->gcallbacks = NULL;
     return ret;
}

/* used for g_list_find_custom() */
static gint webviewlist_find (ListWebView *a, WebView *b)
{
     return (a->w == b ? 0 : (a->w < b ? -1 : 1));
}

/* }}} */

/* {{{ GCallback list */

typedef struct
{
     gchar *name;
     GCallback cb;
     gpointer data;
} ListGCallback;

static ListGCallback *listgcallback_new (const gchar *name, GCallback cb, gpointer data)
{
     ListGCallback *ret = g_malloc (sizeof (ListGCallback));
     ret->name = g_strdup (name);
     ret->cb   = cb;
     ret->data = data;
     return ret;
}

/* used for g_list_find_custom() */
static gint callbacklist_find (ListGCallback *a, GCallback b)
{
     return (a->cb == b ? 0 : (a->cb < b ? -1 : 1));
}

/* }}} */

/*!
 * \public \memberof ViewArea
 * \fn GtkType viewarea_get_type (void)
 * @return A \class{GtkType} object.
 *
 * Return informations about the object's type #ViewArea.
 */
GtkType viewarea_get_type (void)
{
     static GtkType viewarea_type = 0;

     if (!viewarea_type)
     {
          static const GtkTypeInfo viewarea_type_info = {
               "ViewArea",
               sizeof (ViewArea),
               sizeof (ViewAreaClass),
               (GtkClassInitFunc)  viewarea_class_init,
               (GtkObjectInitFunc) viewarea_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };
          viewarea_type = gtk_type_unique (GTK_TYPE_BIN, &viewarea_type_info);
     }

     return viewarea_type;
}

static void viewarea_webview_signal_raise (WebView *w, ViewArea *v)
{
     viewarea_set_focus (v, w);
}

/* Methods */

/*!
 * \public \memberof ViewArea
 * \fn gboolean viewarea_set_focus (ViewArea *v, WebView *w)
 * @param v A #ViewArea object.
 * @param w A #WebView object.
 * @return <code>TRUE</code> on success, <code>FALSE</code> otherwise.
 *
 * Give focus to the specified #WebView. Nothing is done if the
 * #WebView wasn't added to the #ViewArea.
 */
gboolean viewarea_set_focus (ViewArea *v, WebView *w)
{
     GList *found = NULL;

     g_return_val_if_fail (v && w, FALSE);

     found = g_list_find_custom (v->webviews, w, (GCompareFunc) webviewlist_find);
     if (found)
     {
          v->focus->has_focus = FALSE;
          gtk_container_remove (GTK_CONTAINER (v), GTK_WIDGET (v->focus));
          v->focus = w;
          v->focus->has_focus = TRUE;
          gtk_container_add (GTK_CONTAINER (v), GTK_WIDGET (v->focus));
     }

     return (found != NULL);
}

/*!
 * \public \memberof ViewArea
 * \fn WebView *viewarea_get_focus (ViewArea *v)
 * @param v A #ViewArea object.
 * @return A #WebView object.
 *
 * Get the focused #WebView.
 */
WebView *viewarea_get_focus (ViewArea *v)
{
     return (v ? v->focus : NULL);
}

/*!
 * \public \memberof ViewArea
 * \fn void viewarea_add_webview (ViewArea *v, WebView *w)
 * @param v A #ViewArea object.
 * @param w A #WebView object.
 *
 * Add the #WebView object to the #ViewArea.
 * If the #WebView is already in the list, just give it the focus.
 */
void viewarea_add_webview (ViewArea *v, WebView *w)
{
     GList *found = NULL;

     g_return_if_fail (v && w);

     found = g_list_find_custom (v->webviews, w, (GCompareFunc) webviewlist_find);
     if (!found)
     {
          ListWebView *el = listwebview_new (w);

          el->gcallbacks = g_list_append (el->gcallbacks, G_CALLBACK (viewarea_webview_signal_raise));
          el->gcallbacks = g_list_concat (el->gcallbacks, v->gcallbacks);

          v->webviews = g_list_append (v->webviews, el);

          g_signal_connect (G_OBJECT (w), "raise", G_CALLBACK (viewarea_webview_signal_raise), v);

          for (found = v->gcallbacks; found != NULL; found = found->next)
          {
               ListGCallback *el = (ListGCallback *) found->data;
               g_signal_connect (G_OBJECT (w), el->name, el->cb, el->data);
          }
     }

     viewarea_set_focus (v, w);
}

/*!
 * \public \memberof ViewArea
 * \fn void viewarea_del_webview (ViewArea *v, WebView *w, gboolean freemem)
 * @param v A #ViewArea object.
 * @param w A #WebView object.
 * @param freemem If <code>TRUE</code>, free the memory used by \a w.
 *
 * Delete the #WebView object from the #ViewArea.
 */
void viewarea_del_webview (ViewArea *v, WebView *w, gboolean freemem)
{
     GList *found = NULL;

     g_return_if_fail (v && w);

     for (found = g_list_find_custom (v->webviews, w, (GCompareFunc) webviewlist_find); found != NULL; found = found->next)
     {
          v->webviews = g_list_remove (v->webviews, w);

          if (v->focus == w)
               viewarea_set_focus (v, (WebView *) v->webviews->data);

          if (freemem) gtk_widget_destroy (GTK_WIDGET (w));
     }
}

/*!
 * \public \memberof ViewArea
 * \fn void viewarea_signal_connect (ViewArea *v, const gchar *signal_name, GCallback cb, gpointer data)
 * @param v A #ViewArea object.
 * @param signal_name A string of the form <code>"signal-name::detail"</code>.
 * @param cb The \class{GCallback} to connect.
 * @param data Data to pass to \a cb calls.
 *
 * Add a callback which will be connected to #WebView objects
 * registered into the #ViewArea object.
 */
void viewarea_signal_connect (ViewArea *v, const gchar *signal_name, GCallback cb, gpointer data)
{
     GList *tmp;

     g_return_if_fail (v && cb);

     for (tmp = v->webviews; tmp != NULL; tmp = tmp->next)
     {
          ListWebView *el = (ListWebView *) tmp->data;
          GList *found = NULL;

          found = g_list_find (el->gcallbacks, cb);
          if (!found)
          {
               el->gcallbacks = g_list_append (el->gcallbacks, cb);
               g_signal_connect (G_OBJECT (el->w), signal_name, cb, data);
          }
     }

     tmp = g_list_find_custom (v->gcallbacks, cb, (GCompareFunc) callbacklist_find);
     if (!tmp)
     {
          ListGCallback *el = listgcallback_new (signal_name, cb, data);
          v->gcallbacks = g_list_append (v->gcallbacks, el);
     }

}

/* Constructors */

/*!
 * \public \memberof ViewArea
 * \fn GtkWidget *viewarea_new (void);
 * @return A #ViewArea object.
 *
 * Create a new #ViewArea.
 */
GtkWidget *viewarea_new (void)
{
     return GTK_WIDGET (gtk_type_new (viewarea_get_type ()));
}

static void viewarea_class_init (ViewAreaClass *klass)
{
     return;
}

static void viewarea_init (ViewArea *obj)
{
     obj->webviews   = NULL;
     obj->gcallbacks = NULL;
     obj->focus      = NULL;
}

/*! @} */
