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
 * \addtogroup viewarea
 * @{
 */

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
     return (a->cb == b ? 0 : 1);
}

/* }}} */

G_DEFINE_TYPE (ViewArea, viewarea, GTK_TYPE_BIN)

/* signals */

static void viewarea_webview_signal_raise (WebView *w, ViewArea *v)
{
     viewarea_set_focus (v, w);
}

static void viewarea_webview_signal_destroy (WebView *w, ViewArea *v)
{
     viewarea_del_webview (v, w);
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
          gulong handler;

          handler = g_signal_connect (G_OBJECT (w), "raise", G_CALLBACK (viewarea_webview_signal_raise), v);
          el->gcallbacks = g_list_append (el->gcallbacks, (gpointer) handler);

          handler = g_signal_connect (G_OBJECT (w), "destroy", G_CALLBACK (viewarea_webview_signal_destroy), v);
          el->gcallbacks = g_list_append (el->gcallbacks, (gpointer) handler);

          for (found = v->gcallbacks; found != NULL; found = found->next)
          {
               ListGCallback *cb = (ListGCallback *) found->data;
               handler = g_signal_connect (G_OBJECT (w), cb->name, cb->cb, cb->data);
               el->gcallbacks = g_list_append (el->gcallbacks, (gpointer) handler);
          }

          v->webviews = g_list_append (v->webviews, el);
     }
}

/*!
 * \public \memberof ViewArea
 * \fn void viewarea_del_webview (ViewArea *v, WebView *w)
 * @param v A #ViewArea object.
 * @param w A #WebView object.
 *
 * Delete the #WebView object from the #ViewArea.
 */
void viewarea_del_webview (ViewArea *v, WebView *w)
{
     GList *found = NULL;

     g_return_if_fail (v && w);

     for (found = g_list_find_custom (v->webviews, w, (GCompareFunc) webviewlist_find); found != NULL; found = found->next)
     {
          ListWebView *el = (ListWebView *) found->data;
          GList *tmp;

          v->webviews = g_list_remove (v->webviews, el);

          if (v->focus == w)
               viewarea_set_focus (v, (WebView *) v->webviews->data);

          for (tmp = el->gcallbacks; tmp != NULL; tmp = tmp->next)
          {
               gulong handler = (gulong) tmp->data;
               if (g_signal_handler_is_connected (G_OBJECT (w), handler))
                    g_signal_handler_disconnect (G_OBJECT (w), handler);
          }
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

     tmp = g_list_find_custom (v->gcallbacks, cb, (GCompareFunc) callbacklist_find);
     if (!tmp)
     {
          ListGCallback *el = listgcallback_new (signal_name, cb, data);
          v->gcallbacks = g_list_append (v->gcallbacks, el);

          for (tmp = v->webviews; tmp != NULL; tmp = tmp->next)
          {
               ListWebView *el = (ListWebView *) tmp->data;
               gulong handler = g_signal_connect (G_OBJECT (el->w), signal_name, cb, data);
               el->gcallbacks = g_list_append (el->gcallbacks, (gpointer) handler);
          }
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
     return GTK_WIDGET (g_object_new (viewarea_get_type (), NULL));
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
