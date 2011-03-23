#include "local.h"

/*!
 * \defgroup util API.util
 * \ingroup lua
 * Package 'util' of the API
 * @{
 */

/*!
 * \fn static int luaL_util_spawn (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_util_spawn (lua_State *L)
{
     gint argc;
     gchar **argv;
     const gchar *cmd;

     if (lua_gettop (L) >= 1)
     {
          GError *error = NULL;
          GPid pid = 0;

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

     return 0;
}

/*!
 * \fn static int luaL_util_quit (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
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

typedef struct
{
     GRegex *regex;
     gchar *pattern;
} luaL_Regex;

static luaL_Regex *lua_cast_regex (lua_State *L, int index)
{
     luaL_Regex *ret = (luaL_Regex *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TREGEX);
     return ret;
}

static luaL_Regex *lua_check_regex (lua_State *L, int index)
{
     luaL_Regex *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_Regex *) luaL_checkudata (L, index, LUA_TREGEX);
     if (!ret) luaL_typerror (L, index, LUA_TREGEX);
     return ret;
}

static luaL_Regex *lua_pushregex (lua_State *L)
{
     luaL_Regex *ret = (luaL_Regex *) lua_newuserdata (L, sizeof (luaL_Regex));
     luaL_getmetatable (L, LUA_TREGEX);
     lua_setmetatable (L, -2);
     return ret;
}

/* methods */

/*!
 * \fn static int luaL_regex_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_regex_new (lua_State *L)
{
     int argc = lua_gettop (L);
     luaL_Regex *r;

     if (argc >= 1)
     {
          r = lua_pushregex (L);
          r->pattern = g_strdup (luaL_checkstring (L, 1));
          r->regex   = g_regex_new (r->pattern, 0, 0, NULL);
          return 1;
     }

     return 0;
}

/*!
 * \fn static int luaL_regex_match (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_regex_match (lua_State *L)
{
     gboolean ret = FALSE;

     if (lua_gettop (L) >= 2)
     {
          luaL_Regex *r = lua_check_regex (L, 1);
          const gchar *str = luaL_checkstring (L, 2);

          ret = g_regex_match_simple (r->pattern, str, 0, 0);
     }

     lua_pushboolean (L, ret);
     return 1;
}

/*!
 * \fn static int luaL_regex_replace (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_regex_replace (lua_State *L)
{
     gchar *ret = NULL;

     if (lua_gettop (L) >= 3)
     {
          luaL_Regex *r = lua_check_regex (L, 1);
          const gchar *str = luaL_checkstring (L, 2);
          const gchar *rep = luaL_checkstring (L, 3);

          if ((ret = g_regex_replace (r->regex, str, -1, 0, rep, 0, NULL)) != NULL)
          {
               lua_pushstring (L, ret);
               return 1;
          }
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
 */
static int luaL_regex_tostring (lua_State *L)
{
     char *ret = g_strdup_printf ("%p", lua_cast_regex (L, 1));
     lua_pushfstring (L, LUA_TREGEX " (%s)", ret);
     return 1;
}

static const luaL_reg cream_regex_meta[] =
{
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
