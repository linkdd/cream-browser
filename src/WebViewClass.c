#include "local.h"

/*!
 * \addtogroup webview
 * @{
 */

static void webview_destroy (GtkWidget *obj);
static void webview_child_signal_connect (WebView *w);

/* signals */
enum
{
     WEBVIEW_LOAD_COMMIT_SIGNAL,
     WEBVIEW_LOAD_CHANGED_SIGNAL,
     WEBVIEW_LOAD_FINISHED_SIGNAL,

     WEBVIEW_URI_CHANGED_SIGNAL,
     WEBVIEW_TITLE_CHANGED_SIGNAL,
     WEBVIEW_STATUS_CHANGED_SIGNAL,

     WEBVIEW_RAISE_SIGNAL,
     WEBVIEW_DOWNLOAD_SIGNAL,

     WEBVIEW_NB_SIGNALS
};

static guint webview_signals[WEBVIEW_NB_SIGNALS] = { 0 };

G_DEFINE_TYPE (WebView, webview, GTK_TYPE_SCROLLED_WINDOW)

/* Methods */

/*!
 * \public \memberof WebView
 * \fn CreamModule *webview_get_module (WebView *w)
 * @param w A #WebView object.
 * @return A #CreamModule object.
 *
 * Get the #CreamModule used.
 */
CreamModule *webview_get_module (WebView *w)
{
     return (w ? w->mod : NULL);
}

/*!
 * \public \memberof WebView
 * \fn void webview_set_module (WebView *w, CreamModule *mod)
 * @param w A #WebView object.
 * @param mod A #CreamModule object.
 *
 * Associate a new #CreamModule to the #WebView.
 */
void webview_set_module (WebView *w, CreamModule *mod)
{
     if (w && mod)
     {
          gtk_container_remove (GTK_CONTAINER (w), w->child);
          gtk_widget_destroy (w->child);
          w->mod = mod;
          w->child = mod->webview_new ();
          webview_child_signal_connect (w);
          gtk_container_add (GTK_CONTAINER (w), w->child);
     }
}

/*!
 * \public \memberof WebView
 * \fn GtkWidget *webview_get_child (WebView *w)
 * @param w A #WebView object.
 * @return A \class{GtkWidget} object.
 *
 * Get the widget created by the module.
 */
GtkWidget *webview_get_child (WebView *w)
{
     return (w ? w->child : NULL);
}

/*!
 * \public \memberof WebView
 * \fn gboolean webview_has_focus (WebView *w)
 * @param w A #WebView object.
 * @return <code>TRUE</code> if the #WebView is focused, <code>FALSE</code> otherwise.
 *
 * Check if the #WebView is focused.
 */
gboolean webview_has_focus (WebView *w)
{
     return (w ? w->has_focus : FALSE);
}

/*!
 * \public \memberof WebView
 * \fn void webview_raise (WebView *w)
 * @param w A #WebView object.
 *
 * Give focus to the #WebView.
 */
void webview_raise (WebView *w)
{
     g_return_if_fail (w);
     w->has_focus = TRUE;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_RAISE_SIGNAL], 0);
}

/*!
 * \public \memberof WebView
 * \fn void webview_load_uri (WebView *w, const gchar *uri)
 * @param w A #WebView object.
 * @param uri The URI to load.
 *
 * Load a new URI.
 */
void webview_load_uri (WebView *w, const gchar *uri)
{
     UriScheme u;

     g_return_if_fail (w && uri);

     uri_scheme_parse (&u, uri);

     g_return_if_fail (get_protocol (u.scheme) != 0);

     if (w->mod == get_protocol (u.scheme))
          w->mod->call ("load-uri", w, &u);
     else
     {
          webview_set_module (w, get_protocol (u.scheme));
          w->mod->call ("load-uri", w, &u);
     }
}

/*!
 * \public \memberof WebView
 * \fn const gchar *webview_get_uri (WebView *w)
 * @param w A #WebView object.
 * @return Loaded URI.
 *
 * Get the loaded URI.
 */
const gchar *webview_get_uri (WebView *w)
{
     return (w ? w->uri : NULL);
}

/*!
 * \public \memberof WebView
 * \fn const gchar *webview_get_title (WebView *w)
 * @param w A #WebView object.
 * @return Title of the loaded page.
 *
 * Get the title of the loaded page.
 */
const gchar *webview_get_title (WebView *w)
{
     return (w ? w->title : NULL);
}

/*!
 * \public \memberof WebView
 * \fn const gchar *webview_get_status (WebView *w)
 * @param w A #WebView object.
 * @return The current status.
 *
 * Get the current page's status.
 */
const gchar *webview_get_status (WebView *w)
{
     return (w ? w->status : NULL);
}

/* Signals */

static void webview_child_signal_load_commit (GtkWidget *child, const char *uri, WebView *w)
{
     if (w->uri) g_free (w->uri);
     w->uri = g_strdup (uri);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_COMMIT_SIGNAL], 0, uri);
}

static void webview_child_signal_load_changed (GtkWidget *child, gint progress, WebView *w)
{
     w->load_status = progress;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_CHANGED_SIGNAL], 0, progress);
}

static void webview_child_signal_load_finished (GtkWidget *child, WebView *w)
{
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_LOAD_FINISHED_SIGNAL], 0);
}

static void webview_child_signal_uri_changed (GtkWidget *child, const char *uri, WebView *w)
{
     if (w->uri) g_free (w->uri);
     w->uri = g_strdup (uri);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_URI_CHANGED_SIGNAL], 0, uri);
}

static void webview_child_signal_title_changed (GtkWidget *child, const char *title, WebView *w)
{
     if (w->title) g_free (w->title);
     w->title = g_strdup (title);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL], 0, title);
}

static void webview_child_signal_status_changed (GtkWidget *child, const char *status, WebView *w)
{
     if (w->status) g_free (w->status);
     w->status = g_strdup (status);

     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_STATUS_CHANGED_SIGNAL], 0, status);
}

static gboolean webview_child_signal_download_requested (GtkWidget *child, const char *file_uri, WebView *w)
{
     gboolean ret = FALSE;
     g_signal_emit (G_OBJECT (w), webview_signals[WEBVIEW_DOWNLOAD_SIGNAL], 0, file_uri, &ret);
     return ret;
}

static void webview_child_signal_connect (WebView *w)
{
     g_return_if_fail (w && w->child);

     g_signal_connect (G_OBJECT (w->child), "load-commit",        G_CALLBACK (webview_child_signal_load_commit),        w);
     g_signal_connect (G_OBJECT (w->child), "load-changed",       G_CALLBACK (webview_child_signal_load_changed),       w);
     g_signal_connect (G_OBJECT (w->child), "load-finished",      G_CALLBACK (webview_child_signal_load_finished),      w);
     g_signal_connect (G_OBJECT (w->child), "uri-changed",        G_CALLBACK (webview_child_signal_uri_changed),        w);
     g_signal_connect (G_OBJECT (w->child), "title-changed",      G_CALLBACK (webview_child_signal_title_changed),      w);
     g_signal_connect (G_OBJECT (w->child), "status-changed",     G_CALLBACK (webview_child_signal_status_changed),     w);
     g_signal_connect (G_OBJECT (w->child), "download-requested", G_CALLBACK (webview_child_signal_download_requested), w);
}

/* Constructors */

/*!
 * \public \memberof WebView
 * \fn GtkWidget *webview_new (CreaMModule *mod)
 * @param mod A #CreamModule object.
 * @return A #WebView object.
 *
 * Create a new #WebView.
 */
GtkWidget *webview_new (CreamModule *mod)
{
     WebView *w = g_object_new (webview_get_type (), NULL);

     g_return_val_if_fail (w != NULL && mod != NULL, NULL);

     w->mod = mod;
     w->child = w->mod->webview_new ();
     webview_child_signal_connect (w);

     gtk_container_add (GTK_CONTAINER (w), w->child);

     return GTK_WIDGET (w);
}

static void webview_class_init (WebViewClass *klass)
{
#if GTK_CHECK_VERSION (3, 0, 0)
     GtkWidgetClass *object_class = (GtkWidgetClass *) klass;
#else
     GtkObjectClass *object_class = (GtkObjectClass *) klass;
#endif

     object_class->destroy = webview_destroy;
     /* signals */
     webview_signals[WEBVIEW_LOAD_COMMIT_SIGNAL] = g_signal_new (
               "load-commit",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_commit),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_LOAD_CHANGED_SIGNAL] = g_signal_new (
               "load-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__INT,
               G_TYPE_NONE,
               1, G_TYPE_INT);

     webview_signals[WEBVIEW_LOAD_FINISHED_SIGNAL] = g_signal_new (
               "load-finished",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, load_finished),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0);

     webview_signals[WEBVIEW_URI_CHANGED_SIGNAL] = g_signal_new (
               "uri-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_TITLE_CHANGED_SIGNAL] = g_signal_new (
               "title-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, title_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_STATUS_CHANGED_SIGNAL] = g_signal_new (
               "status-changed",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     webview_signals[WEBVIEW_RAISE_SIGNAL] = g_signal_new (
               "raise",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, raise),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0);

     webview_signals[WEBVIEW_DOWNLOAD_SIGNAL] = g_signal_new (
               "download-requested",
               G_TYPE_FROM_CLASS (klass),
               G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
               G_STRUCT_OFFSET (WebViewClass, download),
               NULL, NULL,
               cream_marshal_BOOLEAN__STRING,
               G_TYPE_BOOLEAN,
               1, G_TYPE_STRING);
}

static void webview_init (WebView *obj)
{
     obj->mod   = NULL;
     obj->child = NULL;
     obj->has_focus = FALSE;

     obj->uri    = NULL;
     obj->title  = NULL;
     obj->status = NULL;
     obj->load_status = 0;
}

/* Destructor */

static void webview_destroy (GtkWidget *obj)
{
     WebView *w;

     g_return_if_fail (obj);
     g_return_if_fail (IS_WEB_VIEW (obj));
     w = WEB_VIEW (obj);

     gtk_container_remove (GTK_CONTAINER (w), w->child);
     gtk_widget_destroy (w->child);

     if (w->uri) g_free (w->uri);
     if (w->title) g_free (w->title);
     if (w->status) g_free (w->status);

#if !GTK_CHECK_VERSION (3, 0, 0)
     GtkObjectClass *obj_class = GTK_OBJECT_CLASS (gtk_type_class (webview_get_type ()));
     if (obj_class->destroy)
          obj_class->destroy (GTK_OBJECT (obj));
#endif
}

/*! @} */

/*!
 * \defgroup lua-webview WebView
 * \ingroup lua
 * #WebView bindings for lua.
 * @{
 */

typedef struct
{
     WebView *w;
     int self;
} luaL_WebView;

static luaL_WebView *lua_cast_webview (lua_State *L, int index)
{
     luaL_WebView *ret = (luaL_WebView *) lua_touserdata (L, index);
     if (!ret) luaL_typerror (L, index, LUA_TWEBVIEW);
     return ret;
}

static luaL_WebView *lua_check_webview (lua_State *L, int index)
{
     luaL_WebView *ret;
     luaL_checktype (L, index, LUA_TUSERDATA);
     ret = (luaL_WebView *) luaL_checkudata (L, index, LUA_TWEBVIEW);
     if (!ret) luaL_typerror (L, index, LUA_TWEBVIEW);
     if (!ret->self) luaL_error (L, LUA_TWEBVIEW " not referenced.");
     return ret;
}

/*!
 * \fn void lua_pushwebview (lua_State *L, WebView *w)
 * @param L The lua VM state.
 * @param w The #WebView object to push in lua.
 *
 * Push a #WebView object in lua.
 */
void lua_pushwebview (lua_State *L, WebView *w)
{
     luaL_WebView *ret = (luaL_WebView *) lua_newuserdata (L, sizeof (WebView *));
     ret->w = w;

     /* create a self reference */
     ret->self = luaL_ref (L, LUA_REGISTRYINDEX);
     lua_rawgeti (L, LUA_REGISTRYINDEX, ret->self);

     luaL_getmetatable (L, LUA_TWEBVIEW);
     lua_setmetatable (L, -2);
}

/* methods */

/*!
 * \fn static int luaL_webview_uri (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get the current URI loaded.
 * \code function WebView:uri () \endcode
 */
static int luaL_webview_uri (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     lua_pushstring (L, webview_get_uri (obj->w));
     return 1;
}

/*!
 * \fn static int luaL_webview_title (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Get the title of the loaded page.
 * \code function WebView:title () \endcode
 */
static int luaL_webview_title (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     lua_pushstring (L, webview_get_title (obj->w));
     return 1;
}

/*!
 * \fn static int luaL_webview_load_uri (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Load a new URI.
 * \code function WebView:open (uri) \endcode
 */
static int luaL_webview_load_uri (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     const gchar *uri = luaL_checkstring (L, 2);
     webview_load_uri (obj->w, uri);
     return 0;
}

/*!
 * \fn static int luaL_webview_useragent (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Set User-Agent.
 * \code function WebView:useragent (ua) \endcode
 */
static int luaL_webview_useragent (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     const gchar *ua = luaL_checkstring (L, 2);
     obj->w->mod->call ("useragent", obj->w, ua);
     return 0;
}

/*!
 * \fn static int luaL_webview_raise (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Raise the #WebView.
 * \code function WebView:raise () \endcode
 */
static int luaL_webview_raise (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);
     webview_raise (obj->w);
     return 0;
}

/*!
 * \fn static int luaL_webview_close (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Close the #WebView. After calling this function, the object
 * is unusable but still exists in Lua (will be deleted by the
 * garbage collector).
 * \code function WebView:close () \endcode
 */
static int luaL_webview_close (lua_State *L)
{
     luaL_WebView *obj = lua_check_webview (L, 1);

     /* delete self reference */
     luaL_unref (L, LUA_REGISTRYINDEX, obj->self);
     obj->self = 0;

     gtk_widget_destroy (GTK_WIDGET (obj->w));
     return 0;
}

static const luaL_reg cream_webview_methods[] =
{
     { "uri",       luaL_webview_uri },
     { "title",     luaL_webview_title },
     { "open",      luaL_webview_load_uri },
     { "close",     luaL_webview_close },
     { "useragent", luaL_webview_useragent },
     { "raise",     luaL_webview_raise },
     { NULL, NULL }
};

/*!
 * \fn static int luaL_webview_tostring (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Lua metatable: <code>__tostring</code>
 */
static int luaL_webview_tostring (lua_State *L)
{
     lua_pushfstring (L, LUA_TWEBVIEW ": %p", lua_cast_webview (L, 1));
     return 1;
}

static luaL_reg cream_webview_meta[] =
{
     { "__tostring", luaL_webview_tostring },
     { NULL, NULL }
};

/*!
 * \fn int luaL_webview_register (lua_State *L)
 * @param L The lua VM state.
 * @return Number of return value in lua.
 *
 * Register package in the lua VM state.
 */
int luaL_webview_register (lua_State *L)
{
     luaL_openlib (L, LUA_TWEBVIEW, cream_webview_methods, 0);

     luaL_newmetatable (L, LUA_TWEBVIEW);

     luaL_openlib (L, 0, cream_webview_meta, 0);

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
