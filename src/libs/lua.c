#include <libs/lua.h>

static guint domain = 0;

static LuaContext *lua_ctx_init (void)
{
     LuaContext *ret = g_malloc (sizeof (LuaContext));

     if (ret)
     {
          ret->domain = error_domain_register ("lua");
          ret->L = lua_open ();
          ret->s = 0;
     }

     return ret;
}

static void lua_ctx_openlibs (LuaContext *ctx)
{
     if (ctx)
     {
          luaopen_io (ctx->L);
          luaopen_base (ctx->L);
          luaopen_table (ctx->L);
          luaopen_string (ctx->L);
          luaopen_math (ctx->L);
          luaopen_loadlib (ctx->L);
     }
}

void lua_ctx_set_func (LuaContext *ctx, const char *funcname, LuaCtxFunc func)
{
     if (ctx)
     {
          lua_register (ctx->L, funcname, func);
     }
}

void lua_ctx_report_errors (LuaContext *ctx, ErrorLevel level)
{
     if (ctx && ctx->s != 0)
     {
          error_send (domain, level, "%s", lua_tostring (ctx->L, -1));
          lua_pop (ctx->L, 1);
     }
}

int lua_ctx_parse (LuaContext *ctx, const char *filename)
{
     if (ctx)
     {
          ctx->s = luaL_loadfile (ctx->L, filename);
          if (ctx->s == 0)
               ctx->s = lua_pcall (ctx->L, 0, LUA_MULTRET, 0);

          lua_ctx_report_errors (ctx, ERROR_FATAL);

          return ctx->s;
     }

     return -1;
}

LuaContext *lua_ctx_new (void)
{
     LuaContext *obj = lua_ctx_init ();
     lua_ctx_openlibs (obj);

     /* basic API
      *
      * lua_ctx_set_func (obj, "name", function_callback);
      */

     return obj;
}

void lua_ctx_destroy (LuaContext *ctx)
{
     lua_close (ctx->L);
     g_free (ctx);
}

