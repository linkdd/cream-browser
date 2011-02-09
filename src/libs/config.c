#include <libs/config.h>
#include <libs/errors.h>

static guint domain = 0;

static lua_State *lua_openfile (lua_State *ctx, const char *path)
{
     int status;
     luaL_openlibs (ctx);

     status = luaL_loadfile (ctx, path);
     if (status)
          error_send (domain, ERROR_FATAL, "Couldn't load file: %s\n", lua_tostring (ctx, -1));

     return ctx;
}

void libconfig_init (void)
{
     lua_State *ctx = luaL_newstate ();
     domain = error_domain_register ("config");
}
