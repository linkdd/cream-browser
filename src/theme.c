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
 * \addtogroup theme
 * @{
 */

G_DEFINE_TYPE (Theme, theme, G_TYPE_OBJECT)

static void theme_class_init (ThemeClass *klass)
{
     return;
}

static void theme_init (Theme *self)
{
     GdkDisplay *dpy   = gdk_display_get_default ();
     GdkScreen *screen = gdk_display_get_default_screen (dpy);

     self->engine = gtk_css_provider_new ();
     gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (self->engine), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

     if ((self->path = find_file (FILE_TYPE_CONFIG, "theme.css")))
     {
          GError *error = NULL;
          if (!gtk_css_provider_load_from_path (self->engine, self->path, &error))
               CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, error);
     }
}

/*! @} */
