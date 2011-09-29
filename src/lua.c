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
extern int luaL_bit_register (lua_State *L);
extern int luaL_webview_register (lua_State *L);
extern int luaL_notebook_register (lua_State *L);
extern int luaL_widgets_register (lua_State *L);
extern int luaL_keybinds_register (lua_State *L);

/*!
 * \addtogroup lua
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
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Error handler.
 */
int luaL_error_handler (lua_State *L)
{
     GError *err = NULL;

     /* duplicate string error */
     lua_pushvalue (L, -1);

     err = g_error_new (CREAM_LUA_ERROR, CREAM_LUA_ERROR_FAILED, "%s", luaL_checkstring (L, 1));
     CREAM_BROWSER_GET_CLASS (app)->error (app, FALSE, err);

     if (!luaL_dostring (L, "return debug.traceback (\"error while running function\", 3)"))
     {
          /* move traceback before error */
          lua_insert (L, -2);
          /* Insert sentence */
          lua_pushliteral (L, "\nerror: ");
          /* Move it before error */
          lua_insert (L, -2);
          lua_concat (L, 3);
     }
     return 1;
}

/*!
 * @param err \class{GError} pointer in order to follow possible errors.
 * @return \c TRUE on success, \c FALSE otherwise.
 *
 * Initialize the lua VM state.
 */
gboolean lua_ctx_init (GError **err)
{
     const gchar * const *sysconfdirs = g_get_system_config_dirs ();
     const gchar * const *sysdatadirs = g_get_system_data_dirs ();
     const gchar *usrconfdir = g_get_user_config_dir ();
     const gchar *usrdatadir = g_get_user_data_dir ();
     gchar *tmp, *prgname = g_get_prgname ();
     int i;

     lua_State *luavm = app->luavm = luaL_newstate ();

     /* open libraries */
     luaL_openlibs (luavm);

     luaL_clipboard_register (luavm);
     lua_pop (luavm, 1);

     luaL_util_register (luavm);
     lua_pop (luavm, 1);

     luaL_bit_register (luavm);
     lua_pop (luavm, 1);

     luaL_webview_register (luavm);
     lua_pop (luavm, 1);

     luaL_notebook_register (luavm);
     lua_pop (luavm, 1);

     luaL_widgets_register (luavm);
     lua_pop (luavm, 1);

     luaL_keybinds_register (luavm);
     lua_pop (luavm, 1);

     /* get package.path */
     lua_getglobal (luavm, "package");
     if (!lua_istable (luavm, 1))
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PACKAGE,
                       _("'package' isn't a table")
          );
          return FALSE;
     }

     lua_getfield (luavm, 1, "path");
     if (!lua_isstring (luavm, 2))
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PACKAGE,
                       _("'package.path' isn't a table.")
          );
          return FALSE;
     }

     /* add build directories */
     tmp = g_strdup_printf (";%s", g_build_filename (DATADIR, prgname, "lib", "?.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (DATADIR, prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     lua_concat (luavm, 3); /* concatenate with package.path */

     /* add user and system config dirs in path */
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, prgname, "lib", "?.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     lua_concat (luavm, 3); /* concatenate with package.path */

     for (i = 0; sysconfdirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], prgname, "lib", "?.lua", NULL));
          lua_pushlstring (luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (luavm, tmp, strlen (tmp));
          lua_concat (luavm, 3); /* concatenate with package.path */
     }

     /* add Lua lib path (located in user/system data dirs) */
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, prgname, "lib", "?.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (luavm, tmp, strlen (tmp));
     lua_concat (luavm, 3); /* concatenate with package.path */

     for (i = 0; sysdatadirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], prgname, "lib", "?.lua", NULL));
          lua_pushlstring (luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (luavm, tmp, strlen (tmp));
          lua_concat (luavm, 3); /* concatenate with package.path */
     }

     lua_setfield (luavm, 1, "path"); /* package.path = "concatenated string" */

     lua_pop (luavm, 1);

     return TRUE;
}

/*!
 * @param file Path of the file to parse.
 * @param err \class{GError} pointer in order to follow possible errors.
 * @return \c TRUE on success, \c FALSE otherwise.
 *
 * Parse a lua file.
 */
gboolean lua_ctx_parse (const char *file, GError **err)
{
     lua_State *luavm = app->luavm;
     int s = 0;

     g_return_val_if_fail (file, FALSE);

     s = luaL_loadfile (luavm, file);
     if (s == 0)
     {
          lua_pushcfunction (luavm, luaL_error_handler);
          lua_insert (luavm, -2);
          s = lua_pcall (luavm, 0, LUA_MULTRET, -2);
     }

     if (s != 0)
     {
          g_set_error (err,
                       CREAM_LUA_ERROR,
                       CREAM_LUA_ERROR_PARSE,
                       "%s", lua_tostring (luavm, -1)
          );
          lua_pop (luavm, 1);
          return FALSE;
     }

     return TRUE;
}

/*! Close the lua VM state */
void lua_ctx_close (void)
{
     lua_close (app->luavm);
}

/* Used for __index and __newindex */

/*!
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

/*!
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
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
               luaL_error (L, _("cannot get member '%s'"), lua_tostring (L, 2));
          return 1;
     }

     return luaI_call (L);
}

/*!
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
          luaL_error (L, _("cannot set member '%s'"), lua_tostring (L, 2));
     return luaI_call (L);
}

/*!
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
 * @param L The lua VM state.
 * @param v C data.
 * @return Number of return value in lua.
 *
 * String getter.
 */
int luaI_getstring (lua_State *L, gpointer v)
{
     lua_pushstring (L, (char *) v);
     return 1;
}

/*!
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
