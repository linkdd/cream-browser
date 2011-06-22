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
 * \addtogroup inputbox
 * @{
 */

static void inputbox_activate_cb (Inputbox *obj, gpointer unused);
static gboolean inputbox_keypress_cb (Inputbox *obj, GdkEvent *event, gpointer unused);

G_DEFINE_TYPE (Inputbox, inputbox, GTK_TYPE_ENTRY)

/* Constructors */

/*!
 * \public \memberof Inputbox
 * \fn GtkWidget *inputbox_new (void)
 * @return A new #Inputbox object.
 *
 * Create a new #Inputbox object.
 */
GtkWidget *inputbox_new (void)
{
     Inputbox *obj = g_object_new (CREAM_TYPE_INPUTBOX, NULL);
     g_signal_connect (G_OBJECT (obj), "activate", G_CALLBACK (inputbox_activate_cb), NULL);
     g_signal_connect (G_OBJECT (obj), "key-press-event", G_CALLBACK (inputbox_keypress_cb), NULL);
     return GTK_WIDGET (obj);
}

static void inputbox_class_init (InputboxClass *klass)
{
     return;
}

static void inputbox_init (Inputbox *self)
{
     self->history = NULL;
     self->current = NULL;
}

/* callbacks */

static void inputbox_activate_cb (Inputbox *obj, gpointer unused)
{
     GError *error = NULL;
     gchar *txt;

     g_return_if_fail (gtk_entry_get_text_length (GTK_ENTRY (obj)) > 0);
     txt = g_strdup (gtk_entry_get_text (GTK_ENTRY (obj)));
     gtk_entry_set_text (GTK_ENTRY (obj), "");

     switch (txt[0])
     {
          case ':':
               if (!run_command (txt + 1, &error))
               {
                    gtk_entry_set_text (GTK_ENTRY (obj), error->message);
                    g_error_free (error);
               }

               break;

          case '/':
               /* search forward */
               break;

          case '?':
               /* search backward */
               break;

          default: break;
     }

     obj->history = g_list_prepend (obj->history, txt);
}

static gboolean inputbox_keypress_cb (Inputbox *obj, GdkEvent *event, gpointer unused)
{
     GdkEventKey ekey = event->key;
     gchar *key = gdk_keyval_name (ekey.keyval);
     gboolean ret = TRUE;

     if (g_str_equal (key, "Escape"))
     {
          gtk_entry_set_text (GTK_ENTRY (obj), "");
     }
     else if (g_str_equal (key, "Tab"))
     {
          /* completion */;
     }
     else if (g_str_equal (key, "Up") && obj->history != NULL)
     {
          obj->current = (obj->current == NULL ? obj->history : obj->current->next);

          if (obj->current != NULL)
          {
               gtk_entry_set_text (GTK_ENTRY (obj), (gchar *) obj->current->data);
          }
     }
     else if (g_str_equal (key, "Down") && obj->history != NULL)
     {
          obj->current = (obj->current == NULL ? obj->history : obj->current->prev);

          if (obj->current != NULL)
          {
               gtk_entry_set_text (GTK_ENTRY (obj), (gchar *) obj->current->data);
          }
     }
     else
     {
          ret = FALSE;
     }

     gtk_editable_set_position (GTK_EDITABLE (obj), -1);

     return ret;
}

/* methods */

/*! @} */
