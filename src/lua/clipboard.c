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

#include "../local.h"

/*!
 * \defgroup clipboard Clipboard
 * \ingroup util
 * \class{GtkClipboard} bindings for lua.
 * @{
 */

static guint nclips = 0;

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

     if (lua_gettop (L) == 1)
     {
          const gchar *tmp = luaL_checkstring (L, 1);
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
     GtkClipboard **c = lua_check_clipboard (L, 1);
     const gchar *txt = luaL_checkstring (L, 2);

     if (txt && strlen (txt))
          gtk_clipboard_set_text (*c, txt, -1);

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

static const luaL_reg cream_clipboard_methods[] =
{
     { "new",   luaL_clipboard_new },
     { "set",   luaL_clipboard_set },
     { "get",   luaL_clipboard_get },
     { "clear", luaL_clipboard_clear },
     { NULL, NULL }
};

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
     lua_pushfstring (L, LUA_TCLIPBOARD ": %p", lua_cast_clipboard (L, 1));
     return 1;
}

static const luaL_reg cream_clipboard_meta[] =
{
     { "__tostring", luaL_clipboard_tostring },
     { NULL, NULL }
};

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
