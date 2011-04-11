#include "local.h"

/*!
 * \addtogroup interface
 * @{
 */

/*!
 * \fn GtkWidget *interface_init (void)
 * @return The newly created window.
 *
 * Create a new window instance.
 */
GtkWidget *interface_init (void)
{
     GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
     gtk_window_set_title (GTK_WINDOW (window), PACKAGE);
     g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
     return window;
}

/*!
 * \fn void interface_show (GtkWidget *window)
 * @param window A window (see interface_init()).
 * Show/Redraw a window.
 */
void interface_show (GtkWidget *window)
{
     gtk_widget_show_all (window);
}

/*! @} */

/*!
 * \defgroup lua-interface Graphical User Interface
 * \ingroup lua
 * LUA bindings for the GUI.
 *
 * @{
 */

static int luaL_gui_box_new (lua_State *L)
{
     g_return_val_if_fail (global.theme, 0);

     if (global.theme->global.box.horizontal)
          global.gui.box = gtk_hbox_new (global.theme->global.box.homogeneous, global.theme->global.box.padding);
     else
          global.gui.box = gtk_vbox_new (global.theme->global.box.homogeneous, global.theme->global.box.padding);

     return 0;
}

static int luaL_gui_box_pack (lua_State *L)
{
     gpointer obj;

     g_return_val_if_fail (global.theme, 0);
     luaL_checktype (L, 1, LUA_TUSERDATA);
     if ((obj = luaL_checkudata (L, 1, LUA_TNOTEBOOK)))
     {
          GtkWidget *w = *((GtkWidget **) obj);

          if (global.theme->tab.box.pack_start)
          {
               gtk_box_pack_start (GTK_BOX (global.gui.box), w, global.theme->tab.box.expand,
                                                                global.theme->tab.box.fill,
                                                                global.theme->tab.box.padding);
          }
          else
          {
               gtk_box_pack_end (GTK_BOX (global.gui.box), w, global.theme->tab.box.expand,
                                                              global.theme->tab.box.fill,
                                                              global.theme->tab.box.padding);
          }
     }
     else if ((obj = luaL_checkudata (L, 1, LUA_TVIEWAREA)))
     {
          GtkWidget *w = *((GtkWidget **) obj);

          if (global.theme->webview.box.pack_start)
          {
               gtk_box_pack_start (GTK_BOX (global.gui.box), w, global.theme->webview.box.expand,
                                                                global.theme->webview.box.fill,
                                                                global.theme->webview.box.padding);
          }
          else
          {
               gtk_box_pack_end (GTK_BOX (global.gui.box), w, global.theme->webview.box.expand,
                                                              global.theme->webview.box.fill,
                                                              global.theme->webview.box.padding);
          }
     }
     else if ((obj = luaL_checkudata (L, 1, LUA_TSTATUSBAR)))
     {
          GtkWidget *w = *((GtkWidget **) obj);

          if (global.theme->statusbar.box.pack_start)
          {
               gtk_box_pack_start (GTK_BOX (global.gui.box), w, global.theme->statusbar.box.expand,
                                                                global.theme->statusbar.box.fill,
                                                                global.theme->statusbar.box.padding);
          }
          else
          {
               gtk_box_pack_end (GTK_BOX (global.gui.box), w, global.theme->statusbar.box.expand,
                                                              global.theme->statusbar.box.fill,
                                                              global.theme->statusbar.box.padding);
          }

     }
     else if ((obj = luaL_checkudata (L, 1, LUA_TPROMPTBOX)))
     {
          GtkWidget *w = *((GtkWidget **) obj);

          if (global.theme->promptbox.box.pack_start)
          {
               gtk_box_pack_start (GTK_BOX (global.gui.box), w, global.theme->promptbox.box.expand,
                                                                global.theme->promptbox.box.fill,
                                                                global.theme->promptbox.box.padding);
          }
          else
          {
               gtk_box_pack_end (GTK_BOX (global.gui.box), w, global.theme->promptbox.box.expand,
                                                              global.theme->promptbox.box.fill,
                                                              global.theme->promptbox.box.padding);
          }
     }
     else
          luaL_typerror (L, 1, "userdata");

     return 0;
}

/*! @} */
