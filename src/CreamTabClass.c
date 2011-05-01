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
 * \addtogroup creamtab
 * @{
 */

static Tab *tab_new (WebView *w)
{
     Tab *ret = g_malloc (sizeof (Tab));
     ret->w = w;
     ret->lbl = gtk_label_new (webview_get_title (w));
     ret->has_focus = FALSE;
     return ret;
}

static gint tablist_find (Tab *t, WebView *w)
{
     return (t->w == w ? 0 : (t->w < w ? -1 : 1));
}

G_DEFINE_TYPE (CreamTab, creamtab, GTK_TYPE_HBOX)

/* Signals */

static void creamtab_webview_signal_title_changed (WebView *w, const gchar *title, Tab *t)
{
     g_return_if_fail (t);

     if (t->has_focus)
     {
          gchar *markup = g_markup_printf_escaped ("<span style=\"bold\">%s</span>", (title ? title : "about:blank"));
          gtk_label_set_markup (GTK_LABEL (t->lbl), markup);
          g_free (markup);
     }
     else
          gtk_label_set_text (GTK_LABEL (t->lbl), (title ? title : "about:blank"));
}

static void creamtab_webview_signal_raise (WebView *w, CreamTab *t)
{
     creamtab_set_focus (t, w);
}

static void creamtab_webview_signal_destroy (WebView *w, CreamTab *t)
{
     creamtab_del_webview (t, w);
}

/* Methods */

gboolean creamtab_set_focus (CreamTab *t, WebView *w)
{
     g_return_val_if_fail (t && w, FALSE);
     g_return_val_if_fail (t->focus && t->tabs, FALSE);

     if (t->focus->w != w)
     {
          GList *tmp = NULL;

          /* set the current tab to default style, will use Lua */
          gtk_label_set_text (GTK_LABEL (t->focus->lbl), webview_get_title (t->focus->w));
          t->focus->has_focus = FALSE;

          for (tmp = t->tabs; tmp != NULL; tmp = tmp->next)
          {
               Tab *el = (Tab *) tmp->data;

               if (el->w == w)
               {
                    /* change label style, will use Lua */
                    gchar *markup = g_markup_printf_escaped ("<span style=\"bold\">%s</span>", webview_get_title (w));
                    gtk_label_set_markup (GTK_LABEL (el->lbl), markup);
                    g_free (markup);
                    el->has_focus = TRUE;
                    break;
               }
          }
     }

     return TRUE;
}

WebView *creamtab_get_focus (CreamTab *t)
{
     return (t && t->focus ? t->focus->w : NULL);
}

void creamtab_add_webview (CreamTab *t, WebView *w)
{
     GList *tmp = NULL;
     Tab *el = NULL;

     g_return_if_fail (t && w);

     for (tmp = t->tabs; tmp != NULL; tmp = tmp->next)
     {
          el = (Tab *) tmp->data;
          g_return_if_fail (el->w != w);
     }

     el = tab_new (w);
     t->tabs = g_list_append (t->tabs, el);
     gtk_box_pack_start (GTK_BOX (t), GTK_WIDGET (el->lbl), TRUE, TRUE, 0);

     g_signal_connect (G_OBJECT (w), "title-changed", G_CALLBACK (creamtab_webview_signal_title_changed), el);
     g_signal_connect (G_OBJECT (w), "raise",         G_CALLBACK (creamtab_webview_signal_raise),         t);
     g_signal_connect (G_OBJECT (w), "destroy",       G_CALLBACK (creamtab_webview_signal_destroy),       t);
}

void creamtab_del_webview (CreamTab *t, WebView *w)
{
     GList *found = NULL;

     g_return_if_fail (t && w);

     for (found = g_list_find_custom (t->tabs, w, (GCompareFunc) tablist_find); found != NULL; found = found->next)
     {
          Tab *el = (Tab *) found->data;

          t->tabs = g_list_remove (t->tabs, el);
          gtk_container_remove (GTK_CONTAINER (t), el->lbl);
          gtk_widget_destroy (el->lbl);

          if (el->has_focus)
               creamtab_set_focus (t, ((Tab *) t->tabs->data)->w);

          g_free (el);
     }
}

/* Constructors */

GtkWidget *creamtab_new (void)
{
     return GTK_WIDGET (g_object_new (creamtab_get_type (), NULL));
}

static void creamtab_class_init (CreamTabClass *klass)
{
     return;
}

static void creamtab_init (CreamTab *obj)
{
     obj->tabs  = NULL;
     obj->focus = NULL;

     gtk_box_set_homogeneous (GTK_BOX (obj), TRUE);
     gtk_box_set_spacing (GTK_BOX (obj), 1);
}


/*! @} */
