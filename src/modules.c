#include "local.h"

/*!
 * \addtogroup modules
 * @{
 */

#define CREAM_MODULE_ERROR         cream_module_error_quark()

typedef enum
{
     CREAM_MODULE_ERROR_SUPPORTED,
     CREAM_MODULE_ERROR_OPEN,
     CREAM_MODULE_ERROR_SYMBOLS,
     CREAM_MODULE_ERROR_CLOSE,
     CREAM_MODULE_ERROR_FAILED
} CreamModuleError;

static GQuark cream_module_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream.modules");

     return domain;
}

static GList *modules = NULL;

/*!
 * \fn gboolean modules_init (GError **err)
 * @return <code>TRUE</code> if \class{GModule} is supported, <code>FALSE</code> otherwise.
 *
 * Check if \class{GModule} is supported.
 */
gboolean modules_init (GError **err)
{
     if (!g_module_supported ())
     {
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_SUPPORTED,
                       "GModule isn't supported."
          );

          return FALSE;
     }

     return TRUE;
}

/*!
 * \fn CreamModule *modules_load (const char *filename, GError **err)
 * @param filename Filename of the library to load.
 * @return A #CreamModule object.
 *
 * Open a \class{GModule} object and check if it contains the correct API (see #CreamModule).
 * If the module is alreay loaded, just returns it.
 */
CreamModule *modules_load (const char *filename, GError **err)
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
     m = g_malloc (sizeof (CreamModule));

     m->module = g_module_open (g_strdup_printf ("libcreammod-%s.so", filename), G_MODULE_BIND_LAZY);
     if (!m->module)
     {
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_OPEN,
                       "%s: Couldn't open module: %s",
                       filename, g_module_error ()
          );
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
          g_set_error (err,
                       CREAM_MODULE_ERROR,
                       CREAM_MODULE_ERROR_SYMBOLS,
                       "%s: Couldn't load symbols: %s",
                       filename, g_module_error ()
          );

          if (!g_module_close (m->module))
               g_warning ("%s: %s", filename, g_module_error ());

          return NULL;
     }

     m->init ();

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
                    g_warning ("%s: %s", m->modulename, g_module_error ());

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

typedef struct
{
     CreamModule *mod;
     int self;
} luaL_Module;

static luaL_Module *lua_cast_module (lua_State *L, int index)
{
     luaL_Module *ret = (luaL_Module *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TMODULE);
     return ret;
}

static luaL_Module *lua_check_module (lua_State *L, int index)
{
     luaL_Module *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_Module *) luaL_checkudata (L, index, LUA_TMODULE);
     if (!ret) luaL_typerror (L, index, LUA_TMODULE);
     if (!ret->self) luaL_error (L, LUA_TMODULE " not referenced.");
     return ret;
}

static void lua_pushmodule (lua_State *L, CreamModule *mod)
{
     luaL_Module *ret = (luaL_Module *) lua_newuserdata (L, sizeof (luaL_Module));
     ret->mod = mod;

     /* create a self reference */
     ret->self = luaL_ref (L, LUA_REGISTRYINDEX);
     lua_rawgeti (L, LUA_REGISTRYINDEX, ret->self);

     luaL_getmetatable (L, LUA_TMODULE);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_module_new (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Load a module. Possibily associate protocols to it.
 * \code function luaL_Module.new (filename, ...) \endcode
 */
static int luaL_module_new (lua_State *L)
{
     int argc = lua_gettop (L), i;
     CreamModule *mod;

     if (argc >= 2)
     {
          GError *err = NULL;

          mod = modules_load (luaL_checkstring (L, 1), &err);
          if (mod == NULL)
          {
               char *msg = g_strdup (err->message);
               g_error_free (err);
               return luaL_error (L, msg);
          }

          luaL_checktable (L, 2);

          lua_pushnil (L);
          while (lua_next (L, 2))
          {
               add_protocol (luaL_checkstring (L, -1), mod);
               lua_pop (L, 1);
          }

          lua_pushmodule (L, mod);

          return 1;
     }
     else
          return luaL_error (L, "Too few arguments");

     return 0;
}

/*!
 * \fn static int luaL_module_init (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Initialise the module.
 * \code function luaL_Module:init () \endcode
 */
static int luaL_module_init (lua_State *L)
{
     luaL_Module *obj = lua_check_module (L, 1);
     obj->mod->init ();
     return 0;
}

/*!
 * \fn static int luaL_module_unload (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Unload the module.
 * \code function luaL_Module:unload () \endcode
 */
static int luaL_module_unload (lua_State *L)
{
     luaL_Module *obj = lua_check_module (L, 1);

     /* delete self reference */
     luaL_unref (L, LUA_REGISTRYINDEX, obj->self);
     obj->self = 0;

     modules_unload (obj->mod);
     return 0;
}

static const luaL_reg cream_module_methods[] =
{
     { "new",    luaL_module_new },
     { "init",   luaL_module_init },
     { "unload", luaL_module_unload },
     { NULL, NULL }
};

/* metatable */

/*!
 * \fn static int luaL_module_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>
 */
static int luaL_module_tostring (lua_State *L)
{
     lua_pushfstring (L, LUA_TMODULE ": %p", lua_cast_module (L, 1));
     return 1;
}

/*!
 * \fn static int luaL_module_gc (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__gc</code>
 */
static int luaL_module_gc (lua_State *L)
{
     luaL_Module *obj = lua_cast_module (L, 1);

     /* delete self reference */
     luaL_unref (L, LUA_REGISTRYINDEX, obj->self);
     obj->self = 0;

     modules_unload (obj->mod);
     return 0;
}


static const luaL_reg cream_module_meta[] =
{
     { "__gc",       luaL_module_gc },
     { "__tostring", luaL_module_tostring },
     { NULL, NULL }
};

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
