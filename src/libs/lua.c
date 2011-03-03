#include <libs/lua.h>

extern const struct luaL_reg cream_capi_libs[];
static guint domain = 0;
static GList *luactxs = NULL;

static LuaContext *lua_ctx_init (void)
{
     LuaContext *ret = g_malloc (sizeof (LuaContext));

     if (domain == 0)
          domain = error_domain_register ("lua");

     if (ret)
     {
          ret->domain = domain;
          ret->L = lua_open ();
          ret->s = 0;
     }

     return ret;
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

     luaL_openlibs (obj->L);
     luaL_register (obj->L, "capi", cream_capi_libs);
     lua_pop (obj->L, 1);

     luactxs = g_list_append (luactxs, obj);

     return obj;
}

void lua_ctx_destroy (LuaContext *ctx)
{
     lua_close (ctx->L);
     luactxs = g_list_remove_all (luactxs, ctx);
     g_free (ctx);
}
