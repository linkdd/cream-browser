#include "local.h"

/*!
 * \defgroup clipboard Clipboard
 * \ingroup util
 * \class{GtkClipboard} bindings for lua.
 * @{
 */

static guint nclips = 0;

static int luaL_clipboard_new (lua_State *L);
static int luaL_clipboard_set (lua_State *L);
static int luaL_clipboard_get (lua_State *L);
static int luaL_clipboard_clear (lua_State *L);

static const luaL_reg cream_clipboard_methods[] =
{
     { "new",   luaL_clipboard_new },
     { "set",   luaL_clipboard_set },
     { "get",   luaL_clipboard_get },
     { "clear", luaL_clipboard_clear },
     { NULL, NULL }
};

static int luaL_clipboard_tostring (lua_State *L);

static const luaL_reg cream_clipboard_meta[] =
{
     { "__tostring", luaL_clipboard_tostring },
     { NULL, NULL }
};

static GtkClipboard **lua_cast_clipboard (lua_State *L, int index)
{
     GtkClipboard **ret = (GtkClipboard **) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TCLIPBOARD);
     return ret;
}

static GtkClipboard **lua_check_clipboard (lua_State *L, int index)
{
     GtkClipboard **ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (GtkClipboard **) luaL_checkudata (L, index, LUA_TCLIPBOARD);
     if (!ret) luaL_typerror (L, index, LUA_TCLIPBOARD);
     return ret;
}

static void lua_pushclipboard (lua_State *L, GtkClipboard *c)
{
     GtkClipboard **ret = (GtkClipboard **) lua_newuserdata (L, sizeof (GtkClipboard*));
     *ret = c;
     luaL_getmetatable (L, LUA_TCLIPBOARD);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_clipboard_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Create a new lua \class{GtkClipboard} object.
 * \code function Clipboard.new (atom_name) \endcode
 */
static int luaL_clipboard_new (lua_State *L)
{
     GtkClipboard *clip;
     GdkAtom atom;

     if (lua_gettop (L) == 1 && lua_isstring (L, 1))
     {
          const gchar *tmp = lua_tostring (L, 1);
          atom = gdk_atom_intern (tmp, FALSE);
     }
     else
     {
         gchar *tmp = g_strdup_printf ("__CREAM_BROWSER_CLIPBOARD%d", nclips++);
         atom = gdk_atom_intern (tmp, FALSE);
     }

     clip = gtk_clipboard_get (atom);

     lua_pushclipboard (L, clip);
     return 1;
}

/*!
 * \fn static int luaL_clipboard_set (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set text to the clipboard.
 * \code function Clipboard:set (txt) \endcode
 */
static int luaL_clipboard_set (lua_State *L)
{
     int argc = lua_gettop (L);

     if (argc >= 2)
     {
          GtkClipboard **c = lua_check_clipboard (L, 1);

          if (lua_isstring (L, 2))
          {
               const gchar *txt = lua_tostring (L, 2);

               if (txt && strlen (txt))
                    gtk_clipboard_set_text (*c, txt, -1);
          }
          else
               luaL_typerror (L, 2, "string");
     }

     return 0;
}

/*!
 * \fn static int luaL_clipboard_get (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get text from the clipboard.
 * \code function Clipboard:get () \endcode
 */
static int luaL_clipboard_get (lua_State *L)
{
     GtkClipboard **c = lua_check_clipboard (L, 1);
     gchar *txt = gtk_clipboard_wait_for_text (*c);

     if (txt)
     {
          lua_pushstring (L, txt);
          return 1;
     }

     return 0;
}

/*!
 * \fn static int luaL_clipboard_clear (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Clear the content of the clipboard.
 * \code function Clipboard:clear () \endcode
 */
static int luaL_clipboard_clear (lua_State *L)
{
     GtkClipboard **c = lua_check_clipboard (L, 1);
     gtk_clipboard_clear (*c);
     return 0;
}

/* metatable */

/*!
 * \fn static int luaL_clipboard_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>.
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
