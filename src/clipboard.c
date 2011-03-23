#include "local.h"

/*!
 * \defgroup clipboard Clipboard
 * \ingroup util
 * \class{GtkClipboard} bindings for lua.
 * @{
 */

typedef struct
{
     GtkClipboard *clip;
     GdkAtom atom;
} luaL_Clipboard;

static guint nclips = 0;

static int luaL_clipboard_new (lua_State *L);
static int luaL_clipboard_set (lua_State *L);
static int luaL_clipboard_get (lua_State *L);

static const luaL_reg cream_clipboard_methods[] =
{
     { "new", luaL_clipboard_new },
     { "set", luaL_clipboard_set },
     { "get", luaL_clipboard_get },
     { NULL, NULL }
};

static int luaL_clipboard_tostring (lua_State *L);

static const luaL_reg cream_clipboard_meta[] =
{
     { "__tostring", luaL_clipboard_tostring },
     { NULL, NULL }
};

static luaL_Clipboard *lua_cast_clipboard (lua_State *L, int index)
{
     luaL_Clipboard *ret = (luaL_Clipboard *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TCLIPBOARD);
     return ret;
}

static luaL_Clipboard *lua_check_clipboard (lua_State *L, int index)
{
     luaL_Clipboard *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_Clipboard *) luaL_checkudata (L, index, LUA_TCLIPBOARD);
     if (!ret) luaL_typerror (L, index, LUA_TCLIPBOARD);
     return ret;
}

static luaL_Clipboard *lua_pushclipboard (lua_State *L)
{
     luaL_Clipboard *ret = (luaL_Clipboard *) lua_newuserdata (L, sizeof (luaL_Clipboard));
     luaL_getmetatable (L, LUA_TCLIPBOARD);
     lua_setmetatable (L, -2);
     return ret;
}

/* methods */

/*!
 * \fn static int luaL_clipboard_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_clipboard_new (lua_State *L)
{
     luaL_Clipboard *c = lua_pushclipboard (L);

     if (lua_gettop (L) == 1 && lua_isboolean (L, 1))
     {
          if (lua_toboolean (L, 1))
               c->atom = GDK_SELECTION_PRIMARY;
          else
               c->atom = GDK_NONE;
     }
     else
     {
         gchar *tmp = g_strdup_printf ("__CREAM_BROWSER_CLIPBOARD%d", nclips++);
         c->atom = gdk_atom_intern (tmp, FALSE);
     }


     c->clip = gtk_clipboard_get (c->atom);
     return 1;
}

/*!
 * \fn static int luaL_clipboard_set (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_clipboard_set (lua_State *L)
{
     int argc = lua_gettop (L);

     if (argc >= 2)
     {
          luaL_Clipboard *c = lua_check_clipboard (L, 1);

          if (lua_isstring (L, 2))
          {
               const gchar *txt = lua_tostring (L, 2);

               if (txt && strlen (txt))
                    gtk_clipboard_set_text (c->clip, txt, -1);
          }
          else
               luaL_typerror (L, 2, LUA_TSTRING);
     }

     return 0;
}

/*!
 * \fn static int luaL_clipboard_get (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_clipboard_get (lua_State *L)
{
     luaL_Clipboard *c = lua_check_clipboard (L, 1);
     gchar *txt = gtk_clipboard_wait_for_text (c->clip);

     if (txt)
     {
          lua_pushstring (L, txt);
          return 1;
     }

     return 0;
}

/* metatable */

/*!
 * \fn static int luaL_clipboard_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_clipboard_tostring (lua_State *L)
{
     char *ret = g_strdup_printf ("%p", lua_cast_clipboard (L, 1));
     lua_pushfstring (L, LUA_TCLIPBOARD " (%s)", ret);
     return 1;
}

/*!
 * \fn int luaL_clipboard_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_clipboard_register (lua_State *L)
{
     luaL_openlib (L, LUA_TCLIPBOARD, cream_clipboard_methods, 0);

     luaL_newmetatable (L, LUA_TCLIPBOARD);

     luaL_openlib (L, 0, cream_clipboard_meta, 0);

     lua_pushliteral (L, "__index");
     lua_pushvalue (L, -3);
     lua_rawset (L, -3);

     lua_pushliteral (L, "__metatable");
     lua_pushvalue (L, -3);
     lua_rawset (L, -3);

     lua_pop (L, 1);
     return 1;
}

/*! @} */
