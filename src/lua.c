#include "local.h"

extern int luaL_module_register (lua_State *L);
extern int luaL_clipboard_register (lua_State *L);
extern int luaL_util_register (lua_State *L);

/*!
 * \addtogroup lua
 *
 * \todo Implements all C API.
 * \todo Finish the Lua library using the C API.
 *
 * @{
 */

static guint domain = 0;

/*!
 * \fn void lua_ctx_init (void)
 *
 * Initialize the lua VM state.
 */
void lua_ctx_init (void)
{
     gchar **xdgdirs = NULL;
     int i;

     domain = error_domain_register ("lua");

     global.luavm = lua_open ();

     /* open libraries */
     luaL_openlibs (global.luavm);

     luaL_module_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_clipboard_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_util_register (global.luavm);
     lua_pop (global.luavm, 1);

     /* get package.path */
     lua_getglobal (global.luavm, "package");
     if (!lua_istable (global.luavm, 1))
     {
          error_send (domain, ERROR_FATAL, "'package' isn't a table.");
          return;
     }

     lua_getfield (global.luavm, 1, "path");
     if (!lua_isstring (global.luavm, 2))
     {
          error_send (domain, ERROR_FATAL, "'package.path' isn't a table.");
          return;
     }

     /* add XDG_CONFIG_DIRS in path */
     gchar *tmp = g_strdup_printf ("%s:%s", get_xdg_var_by_name ("XDG_CONFIG_HOME"),
                                            get_xdg_var_by_name ("XDG_CONFIG_DIRS"));
     xdgdirs = g_strsplit (tmp, ":", -1);
     g_free (tmp);

     for (i = 0; i < g_strv_length (xdgdirs); ++i)
     {
          size_t len = strlen (xdgdirs[i]);

          lua_pushliteral (global.luavm, ";");
          lua_pushlstring (global.luavm, xdgdirs[i], len);
          lua_pushliteral (global.luavm, "/cream-browser/?.lua");
          lua_concat (global.luavm, 3);

          lua_pushliteral (global.luavm, ";");
          lua_pushlstring (global.luavm, xdgdirs[i], len);
          lua_pushliteral (global.luavm, "/cream-browser/?/init.lua");
          lua_concat (global.luavm, 3);

          lua_concat (global.luavm, 3); /* concatenate with package.path */
     }

     /* add Lua lib path (PREFIX /share/cream-browser/lib) */
     lua_pushliteral (global.luavm, ";" PREFIX "/share/cream-browser/lib/?.lua");
     lua_pushliteral (global.luavm, ";" PREFIX "/share/cream-browser/lib/?/init.lua");
     lua_concat (global.luavm, 3); /* concatenate with package.path */

     lua_setfield (global.luavm, 1, "path"); /* package.path = "concatenated string" */
}

/*!
 * \fn gboolean lua_ctx_parse (const char *file)
 * @param file Path of the file to parse.
 * @return <code>TRUE</code> on success, <code>FALSE</code> otherwise.
 *
 * Parse a lua file.
 */
gboolean lua_ctx_parse (const char *file)
{
     int s = 0;

     g_return_val_if_fail (file, FALSE);

     s = luaL_loadfile (global.luavm, file);
     if (s == 0)
          s = lua_pcall (global.luavm, 0, LUA_MULTRET, 0);

     if (s != 0)
     {
          error_send (domain, ERROR_FATAL, "%s", lua_tostring (global.luavm, -1));
          lua_pop (global.luavm, 1);
          return FALSE;
     }

     return TRUE;
}

/*!
 * \fn void lua_ctx_close (void)
 *
 * Close the lua VM state.
 */
void lua_ctx_close (void)
{
     if (global.luavm) lua_close (global.luavm);
}

/*! @} */
