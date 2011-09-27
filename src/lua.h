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

#ifndef __LUA_H
#define __LUA_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <glib.h>
#include <err.h>

/*!
 * \defgroup lua Lua context
 * Functions to manipulate the Lua context.
 *
 * @{
 */

int luaL_error_handler (lua_State *L);

/*!
 * \fn static inline int luaL_checkfunction (lua_State *L, int idx)
 * @param L The lua VM state.
 * @param idx Index of the stack.
 * @return A reference on the function or 0.
 *
 * Check if lua_stack[idx] is a function, return a reference in that
 * case.
 */
static inline int luaL_checkfunction (lua_State *L, int idx)
{
     luaL_checktype (L, idx, LUA_TFUNCTION);
     lua_pushvalue (L, idx);
     return luaL_ref (L, LUA_REGISTRYINDEX);
}

/*!
 * \fn static inline void luaL_callfunction (lua_State *L, int ref, int nargs, int nreturns)
 * @param L The lua VM state.
 * @param ref The function's reference.
 * @param nargs Number of arguments.
 * @param nreturns Number of return values.
 *
 * Call a function (using lua_call()).
 */
static inline void luaL_callfunction (lua_State *L, int ref, int nargs, int nreturns)
{
     if (ref)
     {
          int errfunc;

          lua_rawgeti (L, LUA_REGISTRYINDEX, ref);

          /* Move function before arguments */
          lua_insert (L, - nargs - 1);

          /* push error handler and move it before args and function */
          lua_pushcfunction (L, luaL_error_handler);
          lua_insert (L, - nargs - 2);
          errfunc = lua_gettop (L) - nargs - 1;

          if (lua_pcall (L, nargs, nreturns, - nargs - 2))
          {
               warn ("%s", lua_tostring (L, -1));
               lua_pop (L, 2);
               return;
          }

          lua_remove (L, errfunc);
     }
}

/*!
 * \fn static inline void luaL_checktable (lua_State *L, int idx)
 * @param L The lua VM state.
 * @param idx Index of the stack.
 *
 * Check if lua_stack[idx] is a table (raise an error if not).
 */
static inline void luaL_checktable (lua_State *L, int idx)
{
     luaL_checktype (L, idx, LUA_TTABLE);
}

/*!
 * \fn static inline gboolean luaL_checkboolean (lua_State *L, int idx)
 * @param L The lua VM state.
 * @param idx Index of the stack.
 * @return A boolean.
 *
 * Check if lua_stack[idx] is a boolean, return it.
 */
static inline gboolean luaL_checkboolean (lua_State *L, int idx)
{
     luaL_checktype (L, idx, LUA_TBOOLEAN);
     return lua_toboolean (L, idx);
}

/*!
 * \def LUA_TCLIPBOARD
 * Userdata type associated to \class{GtkClipboard}.
 */
#define LUA_TCLIPBOARD "Clipboard"

/*!
 * \def LUA_TREGEX
 * Userdata type associated to \class{GRegex}.
 */
#define LUA_TREGEX     "Regex"

/*!
 * \def LUA_TWEBVIEW
 * Userdata type associated to #WebView.
 */
#define LUA_TWEBVIEW   "WebView"

/*!
 * \def LUA_TNOTEBOOK
 * Userdata type associated to #Notebook.
 */
#define LUA_TNOTEBOOK  "Notebook"

gboolean lua_ctx_init (GError **err);
gboolean lua_ctx_parse (const char *file, GError **err);
void lua_ctx_close (void);

/* push objects */
extern void lua_pushwebview (lua_State *L, WebView *w);
extern void lua_pushnotebook (lua_State *L, Notebook *n);

/* Used for __index and __newindex */

typedef int (*luaI_func) (lua_State *L, gpointer v);

/*!
 * \struct luaI_reg
 * Member info for get and set handlers
 */
typedef struct
{
     const gchar *name;       /*!< member name. */
     luaI_func func;          /*!< get or set function for type of member. */
     size_t offset;           /*!< offset of member with its type. */
} luaI_reg;

void luaI_add (lua_State *L, const luaI_reg *l);
int luaI_index (lua_State *L);
int luaI_newindex (lua_State *L);

int luaI_getint (lua_State *L, gpointer v);
int luaI_setint (lua_State *L, gpointer v);

int luaI_getbool (lua_State *L, gpointer v);
int luaI_setbool (lua_State *L, gpointer v);

int luaI_getstring (lua_State *L, gpointer v);
int luaI_setstring (lua_State *L, gpointer v);

/*!
 * \def LUAL_REGISTER_DECL (name, lua_type)
 * @param name Package's name.
 * @param lua_type Lua type's name.
 *
 * Register a package in lua.
 */
#define LUAL_REGISTER_DECL(name,lua_type)                        \
int luaL_##name##_register (lua_State *L)                        \
{                                                                \
     int metatable, methods;                                     \
                                                                 \
     luaL_openlib (L, lua_type, cream_##name##_methods, 0);      \
     methods = lua_gettop (L);                                   \
                                                                 \
     luaL_newmetatable (L, lua_type);                            \
     luaL_openlib (L, 0, cream_##name##_meta, 0);                \
     metatable = lua_gettop (L);                                 \
                                                                 \
     lua_pushliteral (L, "__metatable");                         \
     lua_pushvalue (L, methods);                                 \
     lua_rawset (L, metatable);                                  \
                                                                 \
     lua_pushliteral (L, "__index");                             \
     lua_pushvalue (L, metatable);                               \
     luaI_add (L, cream_##name##_getters);                       \
     lua_pushvalue (L, methods);                                 \
     lua_pushcclosure (L, luaI_index, 2);                        \
     lua_rawset (L, metatable);                                  \
                                                                 \
     lua_pushliteral (L, "__newindex");                          \
     lua_newtable (L);                                           \
     luaI_add (L, cream_##name##_setters);                       \
     lua_pushcclosure (L, luaI_newindex, 1);                     \
     lua_rawset (L, metatable);                                  \
                                                                 \
     lua_pop (L, 1);                                             \
     return 1;                                                   \
}


/*! @} */

#endif /* __LUA_H */
