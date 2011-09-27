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
 * \defgroup util API.util
 * \ingroup lua
 * Package 'util' of the API
 * @{
 */

/*!
 * \fn static int luaL_util_state (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get current browser's mode, or set it.
 * \code function state () \endcode
 * \code function state (newmode) \endcode
 */
static int luaL_util_state (lua_State *L)
{
     if (lua_gettop (L) >= 1)
     {
          statusbar_set_state (CREAM_STATUSBAR (app->gui.statusbar), luaL_checkint (L, 1));
          return 0;
     }

     lua_pushinteger (L, app->mode);
     return 1;
}

/*!
 * \fn static int luaL_util_spawn (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Spawn a process.
 * \code function spawn (command) \endcode
 */
static int luaL_util_spawn (lua_State *L)
{
     GError *error = NULL;
     GPid pid = 0;

     const gchar *cmd;
     gchar **argv;
     gint argc;

     cmd = luaL_checkstring (L, 1);

     if (!g_shell_parse_argv (cmd, &argc, &argv, &error) && error != NULL)
     {
          luaL_error (L, "spawn: %s", error->message);
          g_error_free (error);
          return 0;
     }

     if (!g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, &pid, &error) && error != NULL)
     {
          luaL_error (L, "spawn: %s", error->message);
          g_error_free (error);
          return 0;
     }

     lua_pushnumber (L, pid);
     return 1;
}

/*!
 * \fn static int luaL_util_quit (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Exit Cream-Browser.
 * \code function quit (code = EXIT_SUCCESS) \endcode
 */
static int luaL_util_quit (lua_State *L)
{
     int ret = EXIT_SUCCESS;

     if (lua_gettop (L) >= 1)
          ret = luaL_checkint (L, 1);

     exit (ret);
     return 0;
}

static const luaL_reg cream_util_functions[] =
{
     { "state", luaL_util_state },
     { "spawn", luaL_util_spawn },
     { "quit",  luaL_util_quit },
     { NULL, NULL }
};


/*!
 * \defgroup regex Regular Expression
 * \ingroup util
 * \class{GRegex} bindings for lua.
 * @{
 */

static GRegex **lua_cast_regex (lua_State *L, int index)
{
     GRegex **ret = (GRegex **) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TREGEX);
     return ret;
}

static GRegex **lua_check_regex (lua_State *L, int index)
{
     GRegex **ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (GRegex **) luaL_checkudata (L, index, LUA_TREGEX);
     if (!ret) luaL_typerror (L, index, LUA_TREGEX);
     return ret;
}

static void lua_pushregex (lua_State *L, GRegex *r)
{
     GRegex **ret = (GRegex **) lua_newuserdata (L, sizeof (GRegex *));
     *ret = r;
     luaL_getmetatable (L, LUA_TREGEX);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_regex_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Create a new lua \class{GRegex} object.
 * \code function Regex.new (pattern) \endcode
 */
static int luaL_regex_new (lua_State *L)
{
     const gchar *pattern = luaL_checkstring (L, 1);
     GRegex *r = g_regex_new (pattern, 0, 0, NULL);
     lua_pushregex (L, r);
     return 1;
}

/*!
 * \fn static int luaL_regex_match (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Scans for a match in string for pattern.
 * \code function Regex:match (string) \endcode
 */
static int luaL_regex_match (lua_State *L)
{
     GRegex **r = lua_check_regex (L, 1);
     const gchar *str = luaL_checkstring (L, 2);
     gboolean ret = g_regex_match_simple (g_regex_get_pattern (*r), str, 0, 0);
     lua_pushboolean (L, ret);
     return 1;
}

/*!
 * \fn static int luaL_regex_replace (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Replaces all occurrences of the pattern in regex with the replacement text.
 * \code function Regex:replace (string, replace) \endcode
 */
static int luaL_regex_replace (lua_State *L)
{
     GRegex **r = lua_check_regex (L, 1);
     const gchar *str = luaL_checkstring (L, 2);
     const gchar *rep = luaL_checkstring (L, 3);
     gchar *ret = NULL;

     if ((ret = g_regex_replace (*r, str, -1, 0, rep, 0, NULL)) != NULL)
     {
          lua_pushstring (L, ret);
          return 1;
     }

     return 0;
}

static const luaL_reg cream_regex_methods[] =
{
     { "new",     luaL_regex_new },
     { "match",   luaL_regex_match },
     { "replace", luaL_regex_replace },
     { NULL, NULL }
};

/* metatable */

/*!
 * \fn static int luaL_regex_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>.
 */
static int luaL_regex_tostring (lua_State *L)
{
     lua_pushfstring (L, LUA_TREGEX ": %p", lua_cast_regex (L, 1));
     return 1;
}

/*!
 * \fn static int luaL_regex_gc (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__gc</code>.
 */
static int luaL_regex_gc (lua_State *L)
{
     GRegex **r = lua_cast_regex (L, 1);
     g_regex_unref (*r);
     return 0;
}

static const luaL_reg cream_regex_meta[] =
{
     { "__gc",       luaL_regex_gc },
     { "__tostring", luaL_regex_tostring },
     { NULL, NULL }
};

/*! @} */

/*!
 * \fn int luaL_util_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_util_register (lua_State *L)
{
     luaL_register (L, "util", cream_util_functions);
     lua_pop (L, 1);

     luaL_openlib (L, LUA_TREGEX, cream_regex_methods, 0);

     luaL_newmetatable (L, LUA_TREGEX);

     luaL_openlib (L, 0, cream_regex_meta, 0);

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
