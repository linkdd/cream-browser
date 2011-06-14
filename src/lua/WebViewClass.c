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
 * \defgroup lua-webview WebView
 * \ingroup lua
 * #WebView bindings for lua.
 * @{
 */

typedef struct
{
     WebView *w;
     int self;
} luaL_WebView;

static luaL_WebView *lua_cast_webview (lua_State *L, int index)
{
     luaL_WebView *ret = (luaL_WebView *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TWEBVIEW);
     return ret;
}

static luaL_WebView *lua_check_webview (lua_State *L, int index)
{
     luaL_WebView *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_WebView *) luaL_checkudata (L, index, LUA_TWEBVIEW);
     if (!ret) luaL_typerror (L, index, LUA_TWEBVIEW);
     if (!ret->self) luaL_error (L, _("%s not referenced."), LUA_TWEBVIEW);
     return ret;
}

/*!
 * \fn void lua_pushwebview (lua_State *L, WebView *w)
 * @param L The lua VM state.
 * @param w The #WebView object to push in lua.
 *
 * Push a #WebView object in lua.
 */
void lua_pushwebview (lua_State *L, WebView *w)
{
     luaL_WebView *ret = (luaL_WebView *) lua_newuserdata (L, sizeof (luaL_WebView));
     ret->w = w;

     /* create a self reference */
     ret->self = luaL_ref (L, LUA_REGISTRYINDEX);
     lua_rawgeti (L, LUA_REGISTRYINDEX, ret->self);

     luaL_getmetatable (L, LUA_TWEBVIEW);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_webview_uri (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get the current URI loaded.
 * \code function WebView:uri () \endcode
 */
static int luaL_webview_uri (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     lua_pushstring (L, webview_get_uri (obj->w));
     return 1;
}

/*!
 * \fn static int luaL_webview_title (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get the title of the loaded page.
 * \code function WebView:title () \endcode
 */
static int luaL_webview_title (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     lua_pushstring (L, webview_get_title (obj->w));
     return 1;
}

/*!
 * \fn static int luaL_webview_load_uri (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Load a new URI.
 * \code function WebView:open (uri) \endcode
 */
static int luaL_webview_load_uri (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     const gchar *uri = luaL_checkstring (L, 2);
     webview_load_uri (obj->w, uri);
     return 0;
}

/*!
 * \fn static int luaL_webview_useragent (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set User-Agent.
 * \code function WebView:useragent (ua) \endcode
 */
static int luaL_webview_useragent (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     const gchar *ua = luaL_checkstring (L, 2);
     obj->w->mod->call ("useragent", NULL, obj->w->child, ua, NULL);
     return 0;
}

/*!
 * \fn static int luaL_webview_raise (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Raise the #WebView.
 * \code function WebView:raise () \endcode
 */
static int luaL_webview_raise (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     webview_raise (obj->w);
     return 0;
}

/*!
 * \fn static int luaL_webview_close (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Close the #WebView. After calling this function, the object
 * is unusable but still exists in Lua (will be deleted by the
 * garbage collector).
 * \code function WebView:close () \endcode
 */
static int luaL_webview_close (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);

     /* delete self reference */
     luaL_unref (L, LUA_REGISTRYINDEX, obj->self);
     obj->self = 0;

     gtk_widget_destroy (GTK_WIDGET (obj->w));
     return 0;
}

static const luaL_reg cream_webview_methods[] =
{
     { "uri",       luaL_webview_uri },
     { "title",     luaL_webview_title },
     { "open",      luaL_webview_load_uri },
     { "close",     luaL_webview_close },
     { "useragent", luaL_webview_useragent },
     { "raise",     luaL_webview_raise },
     { NULL, NULL }
};

/* metatable */

/*!
 * \fn static int luaL_webview_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>
 */
static int luaL_webview_tostring (lua_State *L)
{
     lua_pushfstring (L, LUA_TWEBVIEW ": %p", lua_cast_webview (L, 1));
     return 1;
}

static const luaL_reg cream_webview_meta[] =
{
     { "__tostring", luaL_webview_tostring },
     { NULL, NULL }
};

/* Setters / Getters */

static const luaI_reg cream_webview_getters[] =
{
     { "uri",       luaI_getstring,     offsetof (WebView, uri) },
     { "title",     luaI_getstring,     offsetof (WebView, title) },
     { "focus",     luaI_getbool,       offsetof (WebView, has_focus) },
     { NULL, NULL, 0 }
};

static const luaI_reg cream_webview_setters[] =
{
     { NULL, NULL, 0 }
};

/*!
 * \fn int luaL_webview_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
LUAL_REGISTER_DECL (webview, LUA_TWEBVIEW)

/*! @} */
