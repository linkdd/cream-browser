#include "local.h"

/*!
 * \addtogroup lua
 *
 * \todo Implements all C API.
 * \todo Finish the Lua library using the C API.
 *
 * @{
 */

extern const struct luaL_reg cream_capi_libs[];
static guint domain = 0;

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

/*!
 * \fn void lua_ctx_report_errors (LuaContext *ctx, ErrorLevel level)
 * @param ctx A #LuaContext object.
 * @param level Type of error (see #ErrorLevel).
 *
 * Send errors received at the lua execution.
 */
void lua_ctx_report_errors (LuaContext *ctx, ErrorLevel level)
{
     if (ctx && ctx->s != 0)
     {
          error_send (domain, level, "%s", lua_tostring (ctx->L, -1));
          lua_pop (ctx->L, 1);
     }
}

/*!
 * \fn int lua_ctx_parse (LuaContext *ctx, const char *filename)
 * @param ctx A #LuaContext object.
 * @param filename Lua file to parse.
 * @return 0 on success, the error code otherwise.
 *
 * Parse a lua file.
 *
 */
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

/*!
 * \fn LuaContext *lua_ctx_new (void)
 * @return A #LuaContext object.
 *
 * Create a new #LuaContext object.
 *
 */
LuaContext *lua_ctx_new (void)
{
     LuaContext *obj = lua_ctx_init ();

     luaL_openlibs (obj->L);
     luaL_register (obj->L, "capi", cream_capi_libs);
     lua_pop (obj->L, 1);

     return obj;
}

/*!
 * \fn void lua_ctx_destroy (LuaContext *ctx)
 * @param ctx A #LuaContext object.
 *
 * Free memory used by \a ctx and delete it.
 */
void lua_ctx_destroy (LuaContext *ctx)
{
     lua_close (ctx->L);
     g_free (ctx);
}

/*! @} */
