#ifndef __LUA_H
#define __LUA_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <glib.h>

#include "errors.h"

/*!
 * \defgroup lua Lua context.
 * Functions to manipulate the Lua context.
 *
 * @{
 */

/*!
 * \struct LuaContext
 * Abstraction to the lua state object.
 */
typedef struct _LuaContext LuaContext;
struct _LuaContext
{
     /*< private >*/
     guint domain;
     lua_State *L;
     int s;
};

LuaContext *lua_ctx_new (void);
void lua_ctx_destroy (LuaContext *ctx);

void lua_ctx_report_errors (LuaContext *ctx, ErrorLevel level);
int lua_ctx_parse (LuaContext *ctx, const char *filename);

#define lua_pusherror(L,...)                           \
     do {                                              \
          char *msg = g_strdup_printf (__VA_ARGS__);   \
          lua_pushstring (L, msg);                     \
          lua_error (L);                               \
          g_free (msg);                                \
     } while (0);

#define lua_argcheck_string(L,n)                                                \
     do {                                                                       \
          if (!lua_isstring (L, n))                                             \
               lua_pusherror (L, "Wrong type: argument must be a string");      \
     } while (0)


#define lua_argcheck_number(L,n)                                                \
     do {                                                                       \
          if (!lua_isnumber (L, n))                                             \
               lua_pusherror (L, "Wrong type: argument must be a number");      \
     } while (0)


#define lua_argcheck_boolean(L,n)                                               \
     do {                                                                       \
          if (!lua_isboolean (L, n))                                            \
               lua_pusherror (L, "Wrong type: argument must be a boolean");     \
     } while (0)


#define lua_argcheck_table(L,n)                                                 \
     do {                                                                       \
          if (!lua_istable (L, n))                                              \
               lua_pusherror (L, "Wrong type: argument must be a table");       \
     } while (0)

#define lua_argcheck_function(L,n)                                              \
     do {                                                                       \
          if (!lua_isfunction (L, n))                                           \
               lua_pusherror (L, "Wrong type: argument must be a function");    \
     } while (0)

/*! @} */

#endif /* __LUA_H */
