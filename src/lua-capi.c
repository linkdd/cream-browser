#include "local.h"

static int luaA_capi_go_back (lua_State *L) { return 0; }
static int luaA_capi_go_forward (lua_State *L) { return 0; }
static int luaA_capi_history_get (lua_State *L) { return 0; }

static int luaA_capi_module_enable (lua_State *L)
{
     const char *name;
     guint id = -1;

     if (lua_gettop (L) == 1)
     {
          lua_argcheck_string (L, 1);
          name = lua_tostring (L, 1);
          id = g_str_hash (name);

          if (modules_get (id) == NULL)
               id = modules_load (name);
          else
               lua_pusherror (L, "module_enable: The module '%s' is already enabled.", name);
     }

     lua_pushnumber (L, id);
     return 1;
}

static int luaA_capi_module_disable (lua_State *L)
{
     const char *name;
     guint id = FALSE;

     if (lua_gettop (L) == 1)
     {
          lua_argcheck_string (L, 1);
          name = lua_tostring (L, 1);
          id = g_str_hash (name);

          if (modules_get (id) != NULL)
          {
               modules_unload (id);
               id = TRUE;
          }
          else
               lua_pusherror (L, "module_disable: The module '%s' isn't loaded", name);
     }

     lua_pushnumber (L, id);
     return 1;
}

static int luaA_capi_tab_viewprev (lua_State *L) { return 0; }
static int luaA_capi_tab_viewnext (lua_State *L) { return 0; }
static int luaA_capi_tab_goto (lua_State *L) { return 0; }
static int luaA_capi_tab_close (lua_State *L) { return 0; }
static int luaA_capi_tab_new (lua_State *L) { return 0; }

static int luaA_capi_clipboard_set (lua_State *L)
{
     const gchar *txt;
     gboolean primary = TRUE;

     if (lua_gettop (L) >= 2)
     {
          GdkAtom atom;
          GtkClipboard *clip;

          lua_argcheck_boolean (L, 1);
          lua_argcheck_string (L, 2);

          primary = lua_toboolean (L, 1);
          txt = lua_tostring (L, 2);

          atom = (primary ? GDK_SELECTION_PRIMARY : GDK_NONE);
          clip = gtk_clipboard_get (atom);

          if (txt && strlen (txt))
               gtk_clipboard_set_text (clip, txt, -1);
     }
     else
          lua_pusherror (L, "clipboard_set: Too few arguments");

     return 0;
}

static int luaA_capi_clipboard_get (lua_State *L)
{
     gboolean primary = TRUE;
     gchar *txt;

     if (lua_gettop (L) >= 1)
     {
          GdkAtom atom;
          GtkClipboard *clip;

          lua_argcheck_boolean (L, 1);
          primary = lua_toboolean (L, 1);

          atom = (primary ? GDK_SELECTION_PRIMARY : GDK_NONE);
          clip = gtk_clipboard_get (atom);
          txt = gtk_clipboard_wait_for_text (clip);

          if (txt != NULL)
          {
               lua_pushstring (L, txt);
               return 1;
          }
     }
     else
          lua_pusherror (L, "clipboard_get: Too few arguments");

     return 0;
}

static int luaA_capi_spawn_command (lua_State *L)
{
     gint argc;
     gchar **argv;
     const gchar *cmd;

     if (lua_gettop (L) >= 1)
     {
          GError *error = NULL;
          GPid pid = 0;

          lua_argcheck_string (L, 1);
          cmd = lua_tostring (L, 1);

          if (!g_shell_parse_argv (cmd, &argc, &argv, &error) && error != NULL)
          {
               lua_pusherror (L, "spawn_command: %s", error->message);
               g_error_free (error);
               return 0;
          }

          if (!g_spawn_async (NULL, argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL, &pid, &error) && error != NULL)
          {
               lua_pusherror (L, "spawn_command: %s", error->message);
               g_error_free (error);
               return 0;
          }

          lua_pushnumber (L, pid);
          return 1;
     }
     else
          lua_pusherror (L, "spawn_command: Too few arguments");

     return 0;
}

static int luaA_capi_quit (lua_State *L)
{
     int ret = EXIT_SUCCESS;

     if (lua_gettop (L) >= 1)
     {
          lua_argcheck_number (L, 1);
          ret = lua_tonumber (L, 1);
     }

     exit (ret);
     return 0;
}

static int luaA_capi_regex_match (lua_State *L)
{
     const gchar *regex, *string;
     gboolean ret = FALSE;

     if (lua_gettop (L) >= 2)
     {
          lua_argcheck_string (L, 1);
          lua_argcheck_string (L, 2);

          regex  = lua_tostring (L, 1);
          string = lua_tostring (L, 2);

          ret = g_regex_match_simple (regex, string, 0, 0);
     }
     else
          lua_pusherror (L, "regex_match: Too few arguments");

     lua_pushboolean (L, ret);
     return 1;
}

static int luaA_capi_regex_replace (lua_State *L)
{
     const gchar *regex, *string, *replace;
     gchar *ret;

     if (lua_gettop (L) >= 3)
     {
          GRegex *reg;

          lua_argcheck_string (L, 1);
          lua_argcheck_string (L, 2);
          lua_argcheck_string (L, 3);

          regex   = lua_tostring (L, 1);
          string  = lua_tostring (L, 2);
          replace = lua_tostring (L, 3);

          reg = g_regex_new (regex, 0, 0, NULL);
          ret = g_regex_replace (reg, string, -1, 0, replace, 0, NULL);
          g_regex_unref (reg);

          if (ret != NULL)
          {
               lua_pushstring (L, ret);
               return 1;
          }
     }
     else
          lua_pusherror (L, "regex_replace: Too few arguments");

     return 0;
}

static int luaA_capi_widget_box_pack (lua_State *L)
{
     if (lua_gettop (L) >= 1)
     {
          gboolean pack_start = TRUE, expand = FALSE, fill = FALSE;
          guint padding = 0;
          GtkWidget *w;

          lua_argcheck_table (L, 1);

          lua_pushstring (L, "objptr");
          lua_gettable (L, -2);
          w = (GtkWidget *) GINT_TO_POINTER (lua_tonumber (L, -1));
          lua_pop (L, 1);

          /* theme = {} */
          lua_pushstring (L, "theme");
          lua_gettable (L, -2);

               /* theme.box */
               lua_pushstring (L, "box");
               lua_gettable (L, -2);

                    /* theme.box.pack_start */
                    lua_pushstring (L, "pack_start");
                    lua_gettable (L, -2);
                    pack_start = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.expand */
                    lua_pushstring (L, "expand");
                    lua_gettable (L, -2);
                    expand = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.fill */
                    lua_pushstring (L, "fill");
                    lua_gettable (L, -2);
                    fill = lua_toboolean (L, -1);
                    lua_pop (L, 1);

                    /* theme.box.padding */
                    lua_pushstring (L, "padding");
                    lua_gettable (L, -2);
                    padding = lua_tonumber (L, -1);
                    lua_pop (L, 1);

               lua_pop (L, 1);
          lua_pop (L, 1);

          if (pack_start)
               gtk_box_pack_start (global.gui.box, w, expand, fill, padding);
          else
               gtk_box_pack_end (global.gui.box, w, expand, fill, padding);
     }
     else
          lua_pusherror (L, "widget_box_pack: Too few arguments");

     return 0;
}

static int luaA_capi_widget_notebook (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.notebook);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_notebook_set_buttons (lua_State *L) { return 0; }

static int luaA_capi_widget_webview (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.webview);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_statusbar (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.statusbar);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_widget_promptbox (lua_State *L)
{
     int objptr = GPOINTER_TO_INT (global.gui.promptbox);

     lua_newtable (L);
     lua_pushstring (L, "objptr");
     lua_pushnumber (L, objptr);

     return 1;
}

static int luaA_capi_promptbox_set_text (lua_State *L)
{
     const gchar *text;

     if (lua_gettop (L) >= 1)
     {
          lua_argcheck_string (L, 1);
          text = lua_tostring (L, 1);

          gtk_entry_set_text (GTK_ENTRY (global.gui.promptbox), text);
     }
     else
          lua_pusherror (L, "promptbox_set_text: Too few arguments");

     return 0;
}

static int luaA_capi_widget_connect (lua_State *L)
{
     if (lua_gettop (L) >= 3)
     {
          GObject *obj;
          const gchar *signame;
          GCallback callback;

          lua_argcheck_table (L, 1);
          lua_argcheck_string (L, 2);
          lua_argcheck_function (L, 3);

          lua_pushstring (L, "objptr");
          lua_gettable (L, -2);
          obj = (GObject *) GINT_TO_POINTER (lua_tonumber (L, -1));
          lua_pop (L, 1);

          signame = lua_tostring (L, 2);
          /* callback = */

          g_signal_connect (obj, signame, callback, L);
     }
     else
          lua_pusherror (L, "widget_connect: Too few arguments");

     return 0;
}

const struct luaL_reg cream_capi_libs[] =
{
     { "go_back", luaA_capi_go_back },
     { "go_forward", luaA_capi_go_forward },
     { "history_get", luaA_capi_history_get },
     { "module_enable", luaA_capi_module_enable },
     { "module_disable", luaA_capi_module_disable },
     { "tab_viewprev", luaA_capi_tab_viewprev },
     { "tab_viewnext", luaA_capi_tab_viewnext },
     { "tab_goto", luaA_capi_tab_goto },
     { "tab_close", luaA_capi_tab_close },
     { "tab_new", luaA_capi_tab_new },
     { "clipboard_set", luaA_capi_clipboard_set },
     { "clipboard_get", luaA_capi_clipboard_get },
     { "spawn_command", luaA_capi_spawn_command },
     { "quit", luaA_capi_quit },
     { "regex_match", luaA_capi_regex_match },
     { "regex_replace", luaA_capi_regex_replace },
     { "widget_box_pack", luaA_capi_widget_box_pack },
     { "widget_notebook", luaA_capi_widget_notebook },
     { "widget_notebook_set_buttons", luaA_capi_widget_notebook_set_buttons },
     { "widget_webview", luaA_capi_widget_webview },
     { "widget_statusbar", luaA_capi_widget_statusbar },
     { "widget_promptbox", luaA_capi_widget_promptbox },
     { "promptbox_set_text", luaA_capi_promptbox_set_text },
     { "widget_connect", luaA_capi_widget_connect },
     { NULL, NULL }
};
