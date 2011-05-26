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
 * \defgroup theme Cream-Browser's theme
 * \ingroup lua
 * Share a structure with lua (as a table in lua) to define
 * theme's options.
 *
 * @{
 */

struct theme_global_box_t
{
     gboolean horizontal;
     gboolean homogeneous;
     gint padding;
};

struct theme_box_t
{
     gboolean pack_start;
     gboolean expand;
     gboolean fill;
     gint padding;
};

/*!
 * \struct theme_t
 * Theme structure to be shared with Lua.
 */
struct theme_t
{
     struct
     {
          gchar *font;
          struct theme_global_box_t box;
     } global;

     struct
     {
          gchar *font;

          struct
          {
               gchar *normal;
               gchar *secure;
          } bg, fg;

          struct theme_box_t box;
     } statusbar;

     struct
     {
          gchar *font;

          struct
          {
               gchar *border;
               gchar *normal;
               gchar *focus;
          } bg, fg;

          struct theme_box_t box;
     } tab;

     struct
     {
          gchar *font;

          struct
          {
               gchar *normal;
               gchar *error;
          } bg, fg;

          struct theme_box_t box;
     } promptbox;

     struct
     {
          struct theme_box_t box;
     } webview;
};

/*! @} */

#endif /* __THEME_H */