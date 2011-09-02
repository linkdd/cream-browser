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
 * \addtogroup statusbar
 *
 * @{
 */

#define CREAM_STATUSBAR_GET_PRIVATE(obj)          (G_TYPE_INSTANCE_GET_PRIVATE (obj, CREAM_TYPE_STATUSBAR, StatusbarPrivate))

struct _StatusbarPrivate
{
     GtkWidget *hbox;
     GtkWidget *lstate;
     GtkWidget *llink;
     GtkWidget *lhistory;
     GtkWidget *lscroll;
};

G_DEFINE_TYPE (Statusbar, statusbar, GTK_TYPE_EVENT_BOX)

/* Constructors */

/*!
 * \public \memberof Statusbar
 * \fn GtkWidget *statusbar_new (void)
 * @return A #Statusbar object
 *
 * Create a new #Statusbar object.
 */
GtkWidget *statusbar_new (void)
{
     Statusbar *obj = g_object_new (CREAM_TYPE_STATUSBAR, NULL);
     return GTK_WIDGET (obj);
}

static void statusbar_class_init (StatusbarClass *klass)
{
     g_type_class_add_private (klass, sizeof (StatusbarPrivate));
}

static void statusbar_init (Statusbar *self)
{
     StatusbarPrivate *priv = CREAM_STATUSBAR_GET_PRIVATE (self);

     priv->hbox     = gtk_hbox_new (FALSE, 0);
     priv->lstate   = gtk_label_new (NULL);
     priv->llink    = gtk_label_new (NULL);
     priv->lhistory = gtk_label_new (NULL);
     priv->lscroll  = gtk_label_new (NULL);

     gtk_label_set_selectable (GTK_LABEL (priv->llink), TRUE);

     gtk_box_pack_start (GTK_BOX (priv->hbox), priv->lstate, FALSE, FALSE, 2);
     gtk_box_pack_start (GTK_BOX (priv->hbox), priv->llink, FALSE, FALSE, 2);
     gtk_box_pack_start (GTK_BOX (priv->hbox), priv->lhistory, FALSE, FALSE, 2);
     gtk_box_pack_end (GTK_BOX (priv->hbox), priv->lscroll, FALSE, FALSE, 2);
     gtk_container_add (GTK_CONTAINER (self), priv->hbox);

     self->priv = priv;
}

/* methods */

void statusbar_set_state (Statusbar *obj, CreamMode state)
{
     StatusbarPrivate *priv;
     g_return_if_fail (CREAM_IS_STATUSBAR (obj));
     priv = CREAM_STATUSBAR_GET_PRIVATE (obj);

     switch (state)
     {
          case CREAM_MODE_INSERT:
               gtk_label_set_text (GTK_LABEL (priv->lstate), _("-- INSERT --"));
               break;

          case CREAM_MODE_COMMAND:
               gtk_label_set_text (GTK_LABEL (priv->lstate), _("-- CMD --"));
               break;

          case CREAM_MODE_EMBED:
               gtk_label_set_text (GTK_LABEL (priv->lstate), _("-- EMBED --"));
               break;

          case CREAM_MODE_CARET:
               gtk_label_set_text (GTK_LABEL (priv->lstate), _("-- CARET --"));
               break;

          case CREAM_MODE_NORMAL:
          default:
               gtk_label_set_text (GTK_LABEL (priv->lstate), " ");
               break;
     }
}

void statusbar_set_link (Statusbar *obj, const gchar *link)
{
     StatusbarPrivate *priv;
     g_return_if_fail (CREAM_IS_STATUSBAR (obj));
     priv = CREAM_STATUSBAR_GET_PRIVATE (obj);
     gtk_label_set_text (GTK_LABEL (priv->llink), link);
}

void statusbar_set_history (Statusbar *obj, gboolean can_go_back, gboolean can_go_forward)
{
     StatusbarPrivate *priv;
     gchar *txt;

     g_return_if_fail (CREAM_IS_STATUSBAR (obj));
     priv = CREAM_STATUSBAR_GET_PRIVATE (obj);

     txt = g_strdup_printf ("[%s%s]", (can_go_back ? "-" : ""), (can_go_forward ? "+" : ""));
     gtk_label_set_text (GTK_LABEL (priv->lhistory), txt);
     g_free (txt);
}

void statusbar_set_scroll (Statusbar *obj, gdouble progress)
{
     StatusbarPrivate *priv;
     gchar *txt;

     g_return_if_fail (CREAM_IS_STATUSBAR (obj));
     priv = CREAM_STATUSBAR_GET_PRIVATE (obj);

     txt = g_strdup_printf ("%02d%%", (int) (progress * 100.0));
     gtk_label_set_text (GTK_LABEL (priv->lscroll), txt);
     g_free (txt);
}
