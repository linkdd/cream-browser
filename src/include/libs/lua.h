#ifndef __LUA_H
#define __LUA_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <glib.h>

#include <libs/errors.h>

typedef struct _LuaContext LuaContext;
struct _LuaContext
{
     guint domain;
     lua_State *L;
     int s;
};

typedef int (*LuaCtxFunc) (lua_State *);

LuaContext *lua_ctx_new (void);
void lua_ctx_destroy (LuaContext *ctx);

void lua_ctx_set_func (LuaContext *ctx, const char *funcname, LuaCtxFunc func);
void lua_ctx_report_errors (LuaContext *ctx, ErrorLevel level);
int lua_ctx_parse (LuaContext *ctx, const char *filename);

#endif /* __LUA_H */
