#include "local.h"

static int luaA_capi_go_back (lua_State *L)
{
     CreamModule *mod = webview_get_module (viewarea_get_focus (VIEW_AREA (global.gui.viewarea)));
     GtkWidget *child = webview_get_child (viewarea_get_focus (VIEW_AREA (global.gui.viewarea)));

     mod->call ("go-back", child);
     return 0;
}
static int luaA_capi_go_forward (lua_State *L)
{
     CreamModule *mod = webview_get_module (viewarea_get_focus (VIEW_AREA (global.gui.viewarea)));
     GtkWidget *child = webview_get_child (viewarea_get_focus (VIEW_AREA (global.gui.viewarea)));

     mod->call ("go-forward", child);
     return 0;
}
static int luaA_capi_history_get (lua_State *L) { return 0; }

static int luaA_capi_tab_viewprev (lua_State *L) { return 0; }
static int luaA_capi_tab_viewnext (lua_State *L) { return 0; }
static int luaA_capi_tab_goto (lua_State *L) { return 0; }
static int luaA_capi_tab_close (lua_State *L) { return 0; }
static int luaA_capi_tab_new (lua_State *L) { return 0; }

static int luaA_capi_widget_box (lua_State *L)
{
     gboolean horizontal = FALSE, homogeneous = FALSE;
     gint padding = 5;

     if (lua_gettop (L) >= 1)
     {
          lua_argcheck_table (L, 1);

          lua_pushstring (L, "horizontal");
          lua_gettable (L, -2);
          horizontal = lua_toboolean (L, -1);
          lua_pop (L, 1);

          lua_pushstring (L, "padding");
          lua_gettable (L, -2);
          padding = lua_tonumber (L, -1);
          lua_pop (L, 1);

          lua_pushstring (L, "homogeneous");
          lua_gettable (L, -2);
          homogeneous = lua_toboolean (L, -1);
          lua_pop (L, 1);

          if (horizontal)
               global.gui.box = gtk_hbox_new (homogeneous, padding);
          else
               global.gui.box = gtk_vbox_new (homogeneous, padding);
     }
     else
          lua_pusherror (L, "widget_box: Too few arguments");

     return 0;
}

static int luaA_capi_widget_box_pack (lua_State *L)
{
     if (lua_gettop (L) >= 1)
     {
          gboolean pack_start = TRUE, expand = FALSE, fill = FALSE;
          guint padding = 0;
          GtkWidget *w;

          lua_argcheck_table (L, 1);

          lua_pushstring (L, "objptr");
          lua_gettable (L, -2);
          w = (GtkWidget *) GINT_TO_POINTER (lua_tonumber (L, -1));
          lua_pop (L, 1);

          /* theme = {} */
          lua_pushstring (L, "theme");
          lua_gettable (L, -2);

               /* theme.box */
               lua_pushstring (L, "box");
               lua_gettable (L, -2);

                    /* theme.box.pack_start */
                    lua_pushstring (L, "pack_start");
                    lua_gettable (L, -2);
                    pack_start = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.expand */
                    lua_pushstring (L, "expand");
                    lua_gettable (L, -2);
                    expand = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.fill */
                    lua_pushstring (L, "fill");
                    lua_gettable (L, -2);
                    fill = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.padding */
                    lua_pushstring (L, "padding");
                    lua_gettable (L, -2);
                    padding = lua_tonumber (L, -1);
                    lua_pop (L, 1);

               lua_pop (L, 1);
          lua_pop (L, 1);

          if (pack_start)
               gtk_box_pack_start (GTK_BOX (global.gui.box), w, expand, fill, padding);
          else
               gtk_box_pack_end (GTK_BOX (global.gui.box), w, expand, fill, padding);
     }
     else
          lua_pusherror (L, "widget_box_pack: Too few arguments");

     return 0;
}

static int luaA_capi_widget_notebook (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.notebook);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_notebook_set_buttons (lua_State *L) { return 0; }

static int luaA_capi_widget_webview (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.viewarea);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_statusbar (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.statusbar);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_promptbox (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.promptbox);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_promptbox_set_text (lua_State *L)
{
     const gchar *text;

     if (lua_gettop (L) >= 1)
     {
          lua_argcheck_string (L, 1);
          text = lua_tostring (L, 1);

          gtk_entry_set_text (GTK_ENTRY (global.gui.promptbox), text);
     }
     else
          lua_pusherror (L, "promptbox_set_text: Too few arguments");

     return 0;
}

static int luaA_capi_widget_connect (lua_State *L)
{
     if (lua_gettop (L) >= 3)
     {
          GObject *obj;
          const gchar *signame;
          GCallback callback;

          lua_argcheck_table (L, 1);
          lua_argcheck_string (L, 2);
          lua_argcheck_function (L, 3);

          lua_pushstring (L, "objptr");
          lua_gettable (L, -2);
          obj = (GObject *) GINT_TO_POINTER (lua_tonumber (L, -1));
          lua_pop (L, 1);

          signame = lua_tostring (L, 2);
          /* callback = */

          if (IS_VIEW_AREA (obj))
               viewarea_signal_connect (VIEW_AREA (global.gui.viewarea), signame, callback, L);
          else
               g_signal_connect (obj, signame, callback, L);
     }
     else
          lua_pusherror (L, "widget_connect: Too few arguments");

     return 0;
}

const struct luaL_reg cream_capi_libs[] =
{
     { "go_back", luaA_capi_go_back },
     { "go_forward", luaA_capi_go_forward },
     { "history_get", luaA_capi_history_get },
     { "tab_viewprev", luaA_capi_tab_viewprev },
     { "tab_viewnext", luaA_capi_tab_viewnext },
     { "tab_goto", luaA_capi_tab_goto },
     { "tab_close", luaA_capi_tab_close },
     { "tab_new", luaA_capi_tab_new },
     { "widget_box", luaA_capi_widget_box },
     { "widget_box_pack", luaA_capi_widget_box_pack },
     { "widget_notebook", luaA_capi_widget_notebook },
     { "widget_notebook_set_buttons", luaA_capi_widget_notebook_set_buttons },
     { "widget_webview", luaA_capi_widget_webview },
     { "widget_statusbar", luaA_capi_widget_statusbar },
     { "widget_promptbox", luaA_capi_widget_promptbox },
     { "promptbox_set_text", luaA_capi_promptbox_set_text },
     { "widget_connect", luaA_capi_widget_connect },
     { NULL, NULL }
};
