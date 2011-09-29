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
 * \defgroup lua-notebook Notebook
 * \ingroup lua-widgets
 * #Notebook bindings for lua.
 * @{
 */

typedef struct
{
     Notebook *n;
     int self;
} luaL_Notebook;

static luaL_Notebook *lua_cast_notebook (lua_State *L, int index)
{
     luaL_Notebook *ret = (luaL_Notebook *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TNOTEBOOK);
     return ret;
}

static luaL_Notebook *lua_check_notebook (lua_State *L, int index)
{
     luaL_Notebook *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_Notebook *) luaL_checkudata (L, index, LUA_TNOTEBOOK);
     if (!ret) luaL_typerror (L, index, LUA_TNOTEBOOK);
     if (!ret->self) luaL_error (L, _("%s not referenced."), LUA_TNOTEBOOK);
     return ret;
}


/*!
 * \fn void lua_pushnotebook (lua_State *L, Notebook *n)
 * @param L The lua VM state.
 * @param n The #Notebook object to push in lua.
 *
 * Push a #Notebook object in lua.
 */
void lua_pushnotebook (lua_State *L, Notebook *n)
{
     luaL_Notebook *ret = (luaL_Notebook *) lua_newuserdata (L, sizeof (luaL_Notebook));
     ret->n = n;

     /* create a self reference */
     ret->self = luaL_ref (L, LUA_REGISTRYINDEX);
     lua_rawgeti (L, LUA_REGISTRYINDEX, ret->self);

     luaL_getmetatable (L, LUA_TNOTEBOOK);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_notebook_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Open a new tab.
 * \code function Notebook:new (url) \endcode
 */
static int luaL_notebook_new (lua_State *L)
{
     luaL_Notebook *obj = lua_check_notebook (L, 1);
     const char *uri = luaL_checkstring (L, 2);
     notebook_tabopen (obj->n, uri);
     return 0;
}

/*!
 * \fn static int luaL_notebook_viewprev (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Select previous tab.
 * \code function Notebook:viewprev () \endcode
 */
static int luaL_notebook_viewprev (lua_State *L)
{
     luaL_Notebook *obj = lua_check_notebook (L, 1);
     gtk_notebook_prev_page (GTK_NOTEBOOK (obj->n));
     return 0;
}

/*!
 * \fn static int luaL_notebook_viewnext (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Select next tab.
 * \code function Notebook:viewnext () \endcode
 */
static int luaL_notebook_viewnext (lua_State *L)
{
     luaL_Notebook *obj = lua_check_notebook (L, 1);
     gtk_notebook_next_page (GTK_NOTEBOOK (obj->n));
     return 0;
}

/*!
 * \fn static int luaL_notebook_focus (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Return focus #WebView.
 * \code function Notebook:focus () \endcode
 */
static int luaL_notebook_focus (lua_State *L)
{
     luaL_Notebook *obj = lua_check_notebook (L, 1);
     lua_pushwebview (L, CREAM_WEBVIEW (obj->n->focus));
     return 1;
}

/*!
 * \fn static int luaL_notebook_webviews (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Return the nth page.
 * \code function Notebook:webviews (idx) \endcode
 */
static int luaL_notebook_webviews (lua_State *L)
{
     luaL_Notebook *obj = lua_check_notebook (L, 1);
     int page = luaL_checkinteger (L, 2);
     WebView *w = CREAM_WEBVIEW (g_list_nth_data (obj->n->webviews, page));
     lua_pushwebview (L, w);
     return 1;
}

static const luaL_reg cream_notebook_methods[] =
{
     { "new",      luaL_notebook_new },
     { "viewprev", luaL_notebook_viewprev },
     { "viewnext", luaL_notebook_viewnext },
     { "focus",    luaL_notebook_focus },
     { "webviews", luaL_notebook_webviews },
     { NULL, NULL }
};

/* metatable */

/*!
 * \fn static int luaL_notebook_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>
 */
static int luaL_notebook_tostring (lua_State *L)
{
     lua_pushfstring (L, LUA_TNOTEBOOK ": %p", lua_cast_notebook (L, 1));
     return 1;
}

static const luaL_reg cream_notebook_meta[] =
{
     { "__tostring", luaL_notebook_tostring },
     { NULL, NULL }
};

/* Setters / Getters */

static const luaI_reg cream_notebook_getters[] =
{
     { NULL, NULL, 0 }
};

static const luaI_reg cream_notebook_setters[] =
{
     { NULL, NULL, 0 }
};

/*!
 * \fn int luaL_notebook_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
LUAL_REGISTER_DECL (notebook, LUA_TNOTEBOOK)

/*! @} */
