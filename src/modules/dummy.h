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

#ifndef __MOD_DUMMY_H
#define __MOD_DUMMY_H

#include <glib.h>
#include "../modules.h"
#include "../Statusbar.h"

/*!
 * \defgroup mod-dummy Module Dummy
 * \ingroup modules
 *
 * Manage protocol <code>dummy://</code>.
 *
 * @{
 */

G_BEGIN_DECLS

#define CREAM_MODULE_TYPE_DUMMY         (cream_module_dummy_get_type ())
#define CREAM_MODULE_DUMMY(obj)         (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_MODULE_TYPE_DUMMY, CreamModuleDummy))
#define CREAM_MODULE_IS_DUMMY(obj)      (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_MODULE_TYPE_DUMMY))
#define CREAM_MODULE_DUMMY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_MODULE_TYPE_DUMMY, CreamModuleDummyClass))

typedef struct _CreamModuleDummy CreamModuleDummy;
typedef struct _CreamModuleDummyClass CreamModuleDummyClass;

/*!
 * \class CreamModuleDummy
 * \implements CreamModule
 * Manage protocol <code>dummy://</code>
 */
struct _CreamModuleDummy
{
     GObject parent;
     gchar *name;
};

struct _CreamModuleDummyClass
{
     GObjectClass parent;

     void (*uri_changed) (CreamModuleDummy *self, GtkWidget *webview, const gchar *uri);
     void (*title_changed) (CreamModuleDummy *self, GtkWidget *webview, const gchar *title);
     void (*progress_changed) (CreamModuleDummy *self, GtkWidget *webview, gdouble progress);
     void (*state_changed) (CreamModuleDummy *self, GtkWidget *webview, CreamMode state);
     gboolean (*download) (CreamModuleDummy *self, GtkWidget *webview, const gchar *file);
     void (*favicon_changed) (CreamModuleDummy *self, GtkWidget *webview, GdkPixbuf *pixbuf);
};

GType cream_module_dummy_get_type (void);

G_END_DECLS

/*! @} */

#endif /* __MOD_DUMMY_H */
