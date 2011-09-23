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

#ifndef __THEME_H
#define __THEME_H

/*!
 * \defgroup theme Theming Cream-Browser
 * \ingroup interface
 * Theme class definition.
 *
 * @{
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CREAM_TYPE_THEME           (theme_get_type ())
#define CREAM_THEME(obj)           (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_THEME, Theme))
#define CREAM_IS_THEME(obj)        (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_THEME))
#define CREAM_THEME_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_THEME, ThemeClass))

typedef struct _Theme Theme;
typedef struct _ThemeClass ThemeClass;

struct _Theme
{
     GObject parent;

     GtkCssProvider *engine;
     gchar *path;
};

struct _ThemeClass
{
     GObjectClass parent;
};

GType theme_get_type (void);

G_END_DECLS


/*! @} */

#endif /* __THEME_H */
