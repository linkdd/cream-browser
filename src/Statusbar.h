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

#ifndef __CLASS_STATUSBAR_H
#define __CLASS_STATUSBAR_H

/*!
 * \defgroup statusbar Statusbar
 * \ingroup interface
 * Statusbar class definition
 *
 * @{
 */

#include <gtk/gtk.h>

typedef enum
{
     CREAM_MODE_INSERT,
     CREAM_MODE_COMMAND,
     CREAM_MODE_SEARCH,
     CREAM_MODE_EMBED,
     CREAM_MODE_CARET,
     CREAM_MODE_NORMAL
} CreamMode;

G_BEGIN_DECLS

#define CREAM_TYPE_STATUSBAR            (statusbar_get_type ())
#define CREAM_STATUSBAR(obj)            (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_STATUSBAR, Statusbar))
#define CREAM_IS_STATUSBAR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_STATUSBAR))
#define CREAM_STATUSBAR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_STATUSBAR, StatusbarClass))

typedef struct _Statusbar Statusbar;
typedef struct _StatusbarClass StatusbarClass;
typedef struct _StatusbarPrivate StatusbarPrivate;

/*!
 * \class Statusbar
 * Manage statusbar
 */
struct _Statusbar
{
     GtkEventBox parent;

     StatusbarPrivate *priv;
};

struct _StatusbarClass
{
     GtkEventBoxClass parent;
};

GType statusbar_get_type (void);
GtkWidget *statusbar_new (void);

void statusbar_set_state (Statusbar *obj, CreamMode state);
void statusbar_set_link (Statusbar *obj, const gchar *link);
void statusbar_set_history (Statusbar *obj, gboolean can_go_back, gboolean can_go_forward);
void statusbar_set_scroll (Statusbar *obj, gdouble progress);
void statusbar_set_progress (Statusbar *obj, gdouble progress);

G_END_DECLS

/*! @} */

#endif /* __CLASS_STATUSBAR_H */
