/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  @defgroup Completion Completion
  @ingroup CreamTabbed
  @brief Object which manage the inputbox completion

  @{
 */

#include "Completion.h"
#include "local.h"

static void completion_class_init (CompletionClass *class);
static void completion_init (Completion *obj);

GtkType completion_get_type (void)
{
     static GtkType completion_type = 0;

     if (!completion_type)
     {
          static const GtkTypeInfo completion_info =
          {
               "Completion",
               sizeof (Completion),
               sizeof (CompletionClass),
               (GtkClassInitFunc) completion_class_init,
               (GtkObjectInitFunc) completion_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          completion_type = gtk_type_unique (gtk_vbox_get_type (), &completion_info);
     }

     return completion_type;
}

static void completion_class_init (CompletionClass *class)
{
     return;
}

static void completion_init (Completion *obj)
{
     obj->widgets = NULL;
     obj->comp = NULL;
}

/*!
  \fn GtkWidget *completion_new (void)
  \brief Create a new #Completion object which can be added to a #CreamTabbed
  \return A new #Completion object
 */
GtkWidget *completion_new (void)
{
     Completion *obj = gtk_type_new (completion_get_type ());
     obj->comp = g_completion_new (NULL);
     return GTK_WIDGET (obj);
}

/*!
  \fn void completion_update (Completion *obj, GList *items, gchar *prefix)
  \brief Update the completion list

  \param obj A #Completion object
  \param items List of strings used for the completion
  \param prefix the prefix string, typically typed by the user, which is compared with each of the items.
 */
void completion_update (Completion *obj, GList *items, gchar *prefix)
{
     GList *tmp = NULL;
     gchar *new_prefix;

     g_completion_clear_items (obj->comp);
     g_completion_add_items (obj->comp, items);

     tmp = g_completion_complete (obj->comp, prefix, &new_prefix);
}
