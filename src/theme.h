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
