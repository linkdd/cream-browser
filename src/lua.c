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

extern int luaL_module_register (lua_State *L);
extern int luaL_clipboard_register (lua_State *L);
extern int luaL_util_register (lua_State *L);
extern int luaL_webview_register (lua_State *L);
extern int luaL_theme_register (lua_State *L);

/*!
 * \addtogroup lua
 *
 * \todo Implements all C API.
 * \todo Finish the Lua library using the C API.
 *
 * @{
 */

#define CREAM_LUA_ERROR       cream_lua_error_quark()

typedef enum
{
     CREAM_LUA_ERROR_PACKAGE,
     CREAM_LUA_ERROR_PARSE,
     CREAM_LUA_ERROR_FAILED
} CreamLuaError;

static GQuark cream_lua_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream.lua");

     return domain;
}

/*!
 * \fn gboolean lua_ctx_init (GError **err)
 * @return <code>TRUE</code> on success, <code>FALSE</code> otherwise.
 *
 * Initialize the lua VM state.
 */
gboolean lua_ctx_init (GError **err)
{
     const gchar * const *sysconfdirs = g_get_system_config_dirs ();
     const gchar * const *sysdatadirs = g_get_system_data_dirs ();
     const gchar *usrconfdir = g_get_user_config_dir ();
     const gchar *usrdatadir = g_get_user_data_dir ();
     gchar *tmp;
     int i;

     global.luavm = luaL_newstate ();

     /* open libraries */
     luaL_openlibs (global.luavm);

     luaL_module_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_clipboard_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_util_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_webview_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_theme_register (global.luavm);
     lua_pop (global.luavm, 1);

     /* get package.path */
     lua_getglobal (global.luavm, "package");
     if (!lua_istable (global.luavm, 1))
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PACKAGE,
                       "'package' isn't a table"
          );
          return FALSE;
     }

     lua_getfield (global.luavm, 1, "path");
     if (!lua_isstring (global.luavm, 2))
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PACKAGE,
                       "'package.path' isn't a table."
          );
          return FALSE;
     }

     /* add user and system config dirs in path */
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, global.prgname, "lib", "?.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, global.prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     lua_concat (global.luavm, 3); /* concatenate with package.path */

     for (i = 0; sysconfdirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], global.prgname, "lib", "?.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], global.prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          lua_concat (global.luavm, 3); /* concatenate with package.path */
     }

     /* add Lua lib path (located in user/system data dirs) */
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, global.prgname, "lib", "?.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, global.prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     lua_concat (global.luavm, 3); /* concatenate with package.path */

     for (i = 0; sysdatadirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], global.prgname, "lib", "?.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], global.prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          lua_concat (global.luavm, 3); /* concatenate with package.path */
     }

     lua_setfield (global.luavm, 1, "path"); /* package.path = "concatenated string" */

     lua_pop (global.luavm, 1);

     return TRUE;
}

/*!
 * \fn gboolean lua_ctx_parse (const char *file, GError **err)
 * @param file Path of the file to parse.
 * @return <code>TRUE</code> on success, <code>FALSE</code> otherwise.
 *
 * Parse a lua file.
 */
gboolean lua_ctx_parse (const char *file, GError **err)
{
     int s = 0;

     g_return_val_if_fail (file, FALSE);

     s = luaL_loadfile (global.luavm, file);
     if (s == 0)
          s = lua_pcall (global.luavm, 0, LUA_MULTRET, 0);

     if (s != 0)
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PARSE,
                       "%s", lua_tostring (global.luavm, -1)
          );
          lua_pop (global.luavm, 1);
          return FALSE;
     }

     return TRUE;
}

/*!
 * \fn void lua_ctx_close (void)
 *
 * Close the lua VM state.
 */
void lua_ctx_close (void)
{
     if (global.luavm) lua_close (global.luavm);
}

/* Used for __index and __newindex */

/*!
 * \fn void luaI_add (lua_State *L, const luaI_reg *l)
 * @param L The lua VM state.
 * @param l Table of functions to add.
 *
 * Register member's setters or getters.
 */
void luaI_add (lua_State *L, const luaI_reg *l)
{
     for (; l->name; ++l)
     {
          lua_pushstring (L, l->name);
          lua_pushlightuserdata (L, (void*) l);
          lua_settable (L, -3);
     }
}

static int luaI_call (lua_State *L)
{
     /* for get: stack has userdara, index, lightuserdata
      * for set: stack has userdata, index, value, lightuserdata
      */
     luaI_reg *m = (luaI_reg*) lua_touserdata (L, -1);   /* member info */
     lua_pop (L, 1);                                   /* drop lightuserdata */
     luaL_checktype (L, 1, LUA_TUSERDATA);
     return m->func (L, (void *)((char *) lua_touserdata (L, 1) + m->offset));
}

/*!
 * \fn int luaI_index (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * __index handler.
 */
int luaI_index (lua_State *L)
{
     /* stack has userdata, index */
     lua_pushvalue (L, 2);                   /* dup index */
     lua_rawget (L, lua_upvalueindex (1));   /* lookup member by name */
     if (!lua_islightuserdata (L, -1))
     {
          lua_pop (L, 1);                         /* drop value */
          lua_pushvalue (L, 2);                   /* dup index */
          lua_gettable (L, lua_upvalueindex (2)); /* else try methods */
          if (lua_isnil (L, -1))                  /* invalid member */
               luaL_error (L, "cannot get member '%s'", lua_tostring (L, 2));
          return 1;
     }

     return luaI_call (L);
}

/*!
 * \fn int luaI_newindex (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * __newindex handler.
 */
int luaI_newindex (lua_State *L)
{
     /* stack has userdata, index, value */
     lua_pushvalue (L, 2);                   /* dup index */
     lua_rawget (L, lua_upvalueindex (1));   /* lookup member by name */
     if (!lua_islightuserdata (L, -1))       /* invalid member */
          luaL_error (L, "cannot set member '%s'", lua_tostring (L, 2));
     return luaI_call (L);
}

/*!
 * \fn int luaI_getint (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Int getter.
 */
int luaI_getint (lua_State *L, gpointer v)
{
     lua_pushnumber (L, *(int *) v);
     return 1;
}

/*!
 * \fn int luaI_getbool (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Boolean getter.
 */
int luaI_getbool (lua_State *L, gpointer v)
{
     lua_pushboolean (L, *(gboolean *) v);
     return 1;
}

/*!
 * \fn int luaI_getstring (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Int getter.
 */
int luaI_getstring (lua_State *L, gpointer v)
{
     lua_pushstring (L, (char *) v);
     return 1;
}

/*!
 * \fn int luaI_setint (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Int setter.
 */
int luaI_setint (lua_State *L, gpointer v)
{
     *(int *) v = luaL_checkint (L, 3);
     return 0;
}

/*!
 * \fn int luaI_setbool (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Boolean setter.
 */
int luaI_setbool (lua_State *L, gpointer v)
{
     *(gboolean *) v = luaL_checkboolean (L, 3);
     return 0;
}

/*!
 * \fn int luaI_setstring (lua_State *L, gpointer v)
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * Int setter.
 */
int luaI_setstring (lua_State *L, gpointer v)
{
     char *str = (char *) v;
     str = g_strdup (luaL_checkstring (L, 3));
     return 0;
}

/*! @} */
