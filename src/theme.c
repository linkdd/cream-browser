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

/*!
 * \addtogroup theme
 * @{
 */

static struct theme_t *lua_pushtheme (lua_State *L)
{
     struct theme_t *ret = (struct theme_t *) lua_newuserdata (L, sizeof (struct theme_t));
     luaL_getmetatable (L, LUA_TTHEME);
     lua_setmetatable (L, -2);
     return ret;
}

/* methods */

/*!
 * \fn static int luaL_theme_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Create a new lua #theme_t table.
 * \code function theme.new () \endcode
 */
static int luaL_theme_new (lua_State *L)
{
     struct theme_t *t = lua_pushtheme (L);

     t->global.font = g_strdup ("sans normal 8");

     t->statusbar.font      = g_strdup ("sans normal 8");
     t->statusbar.bg.normal = g_strdup ("#000000");
     t->statusbar.bg.secure = g_strdup ("#B0FF00");
     t->statusbar.fg.normal = g_strdup ("#FFFFFF");
     t->statusbar.fg.secure = g_strdup ("#000000");

     t->tab.font      = g_strdup ("sans normal 8");
     t->tab.bg.border = g_strdup ("1px solid black");
     t->tab.bg.normal = g_strdup ("#505050");
     t->tab.bg.focus  = g_strdup ("#000000");
     t->tab.fg.border = g_strdup ("1px solid black");
     t->tab.fg.normal = g_strdup ("#CCCCCC");
     t->tab.fg.focus  = g_strdup ("#FFFFFF");

     t->promptbox.font      = g_strdup ("sans normal 8");
     t->promptbox.bg.normal = g_strdup ("#151515");
     t->promptbox.bg.error  = g_strdup ("#FF0000");
     t->promptbox.fg.normal = g_strdup ("#CCCCCC");
     t->promptbox.fg.error  = g_strdup ("#000000");

     global.theme = t;
     return 1;
}

static const luaL_reg cream_theme_methods[] =
{
     { "new", luaL_theme_new },
     { NULL, NULL }
};

/* metatable */

/*!
 * \fn static int luaL_theme_gc (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__gc</code>.
 */
static int luaL_theme_gc (lua_State *L)
{
     struct theme_t *t = (struct theme_t *) lua_touserdata (L, 1);

     g_free (t->global.font);

     g_free (t->statusbar.font);
     g_free (t->statusbar.bg.normal);
     g_free (t->statusbar.bg.secure);
     g_free (t->statusbar.fg.normal);
     g_free (t->statusbar.fg.secure);

     g_free (t->tab.font);
     g_free (t->tab.bg.border);
     g_free (t->tab.bg.normal);
     g_free (t->tab.bg.focus);
     g_free (t->tab.fg.border);
     g_free (t->tab.fg.normal);
     g_free (t->tab.fg.focus);

     g_free (t->promptbox.font);
     g_free (t->promptbox.bg.normal);
     g_free (t->promptbox.bg.error);
     g_free (t->promptbox.fg.normal);
     g_free (t->promptbox.fg.error);

     return 0;
}

static const luaL_reg cream_theme_meta[] =
{
     { "__gc", luaL_theme_gc },
     { NULL, NULL }
};

/* Setters / Getters */

static const luaI_reg cream_theme_setters[] =
{
     /* section global */
     { "global.font",            luaI_setstring, offsetof (struct theme_t, global.font) },

     /* statusbar */
     { "statusbar.font",           luaI_setstring, offsetof (struct theme_t, statusbar.font) },
     { "statusbar.bg.normal",      luaI_setstring, offsetof (struct theme_t, statusbar.bg.normal) },
     { "statusbar.bg.secure",      luaI_setstring, offsetof (struct theme_t, statusbar.bg.secure) },
     { "statusbar.fg.normal",      luaI_setstring, offsetof (struct theme_t, statusbar.fg.normal) },
     { "statusbar.fg.secure",      luaI_setstring, offsetof (struct theme_t, statusbar.fg.secure) },

     /* tab */
     { "tab.font",           luaI_setstring, offsetof (struct theme_t, tab.font) },
     { "tab.bg.border",      luaI_setstring, offsetof (struct theme_t, tab.bg.border) },
     { "tab.bg.normal",      luaI_setstring, offsetof (struct theme_t, tab.bg.normal) },
     { "tab.bg.focus",       luaI_setstring, offsetof (struct theme_t, tab.bg.focus) },
     { "tab.fg.border",      luaI_setstring, offsetof (struct theme_t, tab.fg.border) },
     { "tab.fg.normal",      luaI_setstring, offsetof (struct theme_t, tab.fg.normal) },
     { "tab.fg.focus",       luaI_setstring, offsetof (struct theme_t, tab.fg.focus) },

     /* promptbox */
     { "promptbox.font",           luaI_setstring, offsetof (struct theme_t, promptbox.font) },
     { "promptbox.bg.normal",      luaI_setstring, offsetof (struct theme_t, promptbox.bg.normal) },
     { "promptbox.bg.error",       luaI_setstring, offsetof (struct theme_t, promptbox.bg.error) },
     { "promptbox.fg.normal",      luaI_setstring, offsetof (struct theme_t, promptbox.fg.normal) },
     { "promptbox.fg.error",       luaI_setstring, offsetof (struct theme_t, promptbox.fg.error) },

     { NULL, NULL, 0 }
};

static const luaI_reg cream_theme_getters[] =
{
     /* section global */
     { "global.font",            luaI_getstring, offsetof (struct theme_t, global.font) },

     /* statusbar */
     { "statusbar.font",           luaI_getstring, offsetof (struct theme_t, statusbar.font) },
     { "statusbar.bg.normal",      luaI_getstring, offsetof (struct theme_t, statusbar.bg.normal) },
     { "statusbar.bg.secure",      luaI_getstring, offsetof (struct theme_t, statusbar.bg.secure) },
     { "statusbar.fg.normal",      luaI_getstring, offsetof (struct theme_t, statusbar.fg.normal) },
     { "statusbar.fg.secure",      luaI_getstring, offsetof (struct theme_t, statusbar.fg.secure) },

     /* tab */
     { "tab.font",           luaI_getstring, offsetof (struct theme_t, tab.font) },
     { "tab.bg.border",      luaI_getstring, offsetof (struct theme_t, tab.bg.border) },
     { "tab.bg.normal",      luaI_getstring, offsetof (struct theme_t, tab.bg.normal) },
     { "tab.bg.focus",       luaI_getstring, offsetof (struct theme_t, tab.bg.focus) },
     { "tab.fg.border",      luaI_getstring, offsetof (struct theme_t, tab.fg.border) },
     { "tab.fg.normal",      luaI_getstring, offsetof (struct theme_t, tab.fg.normal) },
     { "tab.fg.focus",       luaI_getstring, offsetof (struct theme_t, tab.fg.focus) },

     /* promptbox */
     { "promptbox.font",           luaI_getstring, offsetof (struct theme_t, promptbox.font) },
     { "promptbox.bg.normal",      luaI_getstring, offsetof (struct theme_t, promptbox.bg.normal) },
     { "promptbox.bg.error",       luaI_getstring, offsetof (struct theme_t, promptbox.bg.error) },
     { "promptbox.fg.normal",      luaI_getstring, offsetof (struct theme_t, promptbox.fg.normal) },
     { "promptbox.fg.error",       luaI_getstring, offsetof (struct theme_t, promptbox.fg.error) },

     { NULL, NULL, 0 }
};


/*!
 * \fn int luaL_theme_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
LUAL_REGISTER_DECL (theme, LUA_TTHEME)

/*! @} */
