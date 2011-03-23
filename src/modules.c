#include "local.h"

/*!
 * \addtogroup modules
 * @{
 */

static guint domain = 0;
static GList *modules = NULL;

static CreamModule *lua_cast_module (lua_State *L, int index);
static CreamModule *lua_check_module (lua_State *L, int index);
static CreamModule *lua_pushmodule (lua_State *L);

static int luaL_module_new (lua_State *L);
static int luaL_module_init (lua_State *L);
static int luaL_module_unload (lua_State *L);

static const luaL_reg cream_module_methods[] =
{
     { "new",    luaL_module_new },
     { "init",   luaL_module_init },
     { "unload", luaL_module_unload },
     { NULL, NULL }
};

static int luaL_module_tostring (lua_State *L);

static const luaL_reg cream_module_meta[] =
{
     { "__tostring", luaL_module_tostring },
     { NULL, NULL }
};

/*!
 * \fn gboolean modules_init (void)
 * @return <code>TRUE</code> if \class{GModule} is supported, <code>FALSE</code> otherwise.
 *
 * Check if \class{GModule} is supported and register the error domain.
 */
gboolean modules_init (void)
{
     if (!g_module_supported ())
          return FALSE;

     domain = error_domain_register ("modules");

     return TRUE;
}

/*!
 * \fn CreamModule *modules_load (const char *filename)
 * @param filename Filename of the library to load.
 * @return A #CreamModule object.
 *
 * Open a \class{GModule} object and check if it contains the correct API (see #CreamModule).
 * If the module is alreay loaded, just returns it.
 */
CreamModule *modules_load (const char *filename)
{
     CreamModule *m;
     GList *tmp;

     guint id = g_str_hash (filename);

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          m = tmp->data;
          if (m->module_id == id)
               return m;
     }

     /* open module */
     m = (CreamModule *) lua_pushmodule (global.luavm);

     m->module = g_module_open (filename, G_MODULE_BIND_LAZY);
     if (!m->module)
     {
          error_send (domain, ERROR_CRITICAL, "%s: Couldn't open module: %s", filename, g_module_error ());
          lua_pop (global.luavm, 1);
          return NULL;
     }

     /* load symbols */
     if (!(g_module_symbol (m->module, "module_init", (gpointer *) &m->init)
          && g_module_symbol (m->module, "module_unload", (gpointer *) &m->unload)
          && g_module_symbol (m->module, "module_webview_new", (gpointer *) &m->webview_new)
          && g_module_symbol (m->module, "module_call", (gpointer *) &m->call))
          || !(m->init != NULL && m->unload != NULL && m->webview_new != NULL && m->call != NULL)
        )
     {
          error_send (domain, ERROR_CRITICAL, "%s: Couldn't load symbols: %s", filename, g_module_error ());
          if (!g_module_close (m->module))
               error_send (domain, ERROR_WARNING, "%s: %s", filename, g_module_error ());
          lua_pop (global.luavm, 1);
          return NULL;
     }

     m->modulename = g_strdup (filename);
     m->module_id = id;

     modules = g_list_append (modules, m);

     return m;
}

/*!
 * \fn void modules_unload (CreamModule *mod)
 * @param mod A #CreamModule object.
 *
 * Unload a module
 */
void modules_unload (CreamModule *mod)
{
     GList *tmp;

     g_return_if_fail (mod);

     for (tmp = modules; tmp != NULL; tmp = tmp->next)
     {
          CreamModule *m = tmp->data;

          if (m->module_id == mod->module_id)
          {
               m->unload ();

               if (!g_module_close (m->module))
                    error_send (domain, ERROR_WARNING, "%s: %s", m->modulename, g_module_error ());

               modules = g_list_remove_link (modules, tmp);

               g_free (m->modulename);
               break;
          }
     }
}

/*!
 * @}
 *
 * \defgroup lua-module API.modules
 * \ingroup lua
 *
 * #CreamModule bindings for lua.
 * @{
 */

static CreamModule *lua_cast_module (lua_State *L, int index)
{
     CreamModule *ret = (CreamModule *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TMODULE);
     return ret;
}

static CreamModule *lua_check_module (lua_State *L, int index)
{
     CreamModule *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (CreamModule *) luaL_checkudata (L, index, LUA_TMODULE);
     if (!ret) luaL_typerror (L, index, LUA_TMODULE);
     return ret;
}

static CreamModule *lua_pushmodule (lua_State *L)
{
     CreamModule *ret = (CreamModule *) lua_newuserdata (L, sizeof (CreamModule));
     luaL_getmetatable (L, LUA_TMODULE);
     lua_setmetatable (L, -2);
     return ret;
}

/* methods */

/*!
 * \fn static int luaL_module_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_module_new (lua_State *L)
{
     int argc = lua_gettop (L), i;
     CreamModule *mod;

     if (argc >= 2)
     {
          mod = modules_load (luaL_checkstring (L, 1));
          if (mod == NULL)
               return luaL_error (L, "Can't load module.");

          for (i = 2; i <= argc; ++i)
          {
               luaL_checkstring (L, i);
               add_protocol (lua_tostring (L, i), mod);
          }

          return 1;
     }

     return 0;
}

/*!
 * \fn static int luaL_module_init (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_module_init (lua_State *L)
{
     CreamModule *mod = lua_check_module (L, 1);
     mod->init ();
     return 0;
}

/*!
 * \fn static int luaL_module_unload (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_module_unload (lua_State *L)
{
     CreamModule *mod = lua_check_module (L, 1);
     modules_unload (mod);
     return 0;
}

/* metatable */

/*!
 * \fn static int luaL_module_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 */
static int luaL_module_tostring (lua_State *L)
{
     char *ret = g_strdup_printf ("%p", lua_cast_module (L, 1));
     lua_pushfstring (L, LUA_TMODULE " (%s)", ret);
     return 1;
}

/*!
 * \fn int luaL_module_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_module_register (lua_State *L)
{
     luaL_openlib (L, LUA_TMODULE, cream_module_methods, 0);

     luaL_newmetatable (L, LUA_TMODULE);

     luaL_openlib (L, 0, cream_module_meta, 0);

     lua_pushliteral (L, "__index");
     lua_pushvalue (L, -3);
     lua_rawset (L, -3);

     lua_pushliteral (L, "__metatable");
     lua_pushvalue (L, -3);
     lua_rawset (L, -3);

     lua_pop (L, 1);
     return 1;
}

/*! @} */
