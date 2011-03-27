#include "local.h"

extern int luaL_module_register (lua_State *L);
extern int luaL_clipboard_register (lua_State *L);
extern int luaL_util_register (lua_State *L);
extern int luaL_webview_register (lua_State *L);

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
     const gchar * const *sysconfdirs = g_get_system_config_dirs ();
     const gchar * const *sysdatadirs = g_get_system_data_dirs ();
     const gchar *usrconfdir = g_get_user_config_dir ();
     const gchar *usrdatadir = g_get_user_data_dir ();
     gchar *tmp;
     int i;

     domain = error_domain_register ("lua");

     global.luavm = luaL_newstate ();

     /* open libraries */
     luaL_openlibs (global.luavm);

     luaL_module_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_clipboard_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_util_register (global.luavm);
     lua_pop (global.luavm, 1);

     luaL_webview_register (global.luavm);
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

     /* add user and system config dirs in path */
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, global.prgname, "lib", "?.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrconfdir, global.prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     lua_concat (global.luavm, 3); /* concatenate with package.path */

     for (i = 0; sysconfdirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], global.prgname, "lib", "?.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysconfdirs[i], global.prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          lua_concat (global.luavm, 3); /* concatenate with package.path */
     }

     /* add Lua lib path (located in user/system data dirs) */
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, global.prgname, "lib", "?.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     tmp = g_strdup_printf (";%s", g_build_filename (usrdatadir, global.prgname, "lib", "?", "init.lua", NULL));
     lua_pushlstring (global.luavm, tmp, strlen (tmp));
     lua_concat (global.luavm, 3); /* concatenate with package.path */

     for (i = 0; sysdatadirs[i] != NULL; ++i)
     {
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], global.prgname, "lib", "?.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          tmp = g_strdup_printf (";%s", g_build_filename (sysdatadirs[i], global.prgname, "lib", "?", "init.lua", NULL));
          lua_pushlstring (global.luavm, tmp, strlen (tmp));
          lua_concat (global.luavm, 3); /* concatenate with package.path */
     }

     lua_setfield (global.luavm, 1, "path"); /* package.path = "concatenated string" */

     lua_pop (global.luavm, 1);
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
