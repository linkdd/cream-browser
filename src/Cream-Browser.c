/*
 * Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "local.h"

/*!
 * \addtogroup cream-browser
 * @{
 */

static void cream_browser_error_handler (CreamBrowser *self, gboolean abort, GError *error);
static void cream_browser_finalize (GObject *obj);
static void cream_browser_activate (GApplication *gapp);
static gint cream_browser_command_line (GApplication *gapp, GApplicationCommandLine *cmdline);
static void cream_browser_startup (CreamBrowser *self);
static void cream_browser_ctl (CreamBrowser *self);

#define CREAM_BROWSER_ERROR         cream_browser_error_quark()

typedef enum
{
     CREAM_BROWSER_ERROR_CONFIG,
     CREAM_BROWSER_ERROR_FAILED
} CreamBrowserError;

static GQuark cream_browser_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream");

     return domain;
}

G_DEFINE_TYPE (CreamBrowser, cream_browser, GTK_TYPE_APPLICATION)

CreamBrowser *cream_browser_new (void)
{
     g_type_init ();

     return g_object_new (GAPP_TYPE_CREAM_BROWSER,
                          "application-id", "org.gtk.CreamBrowser",
                          "flags", G_APPLICATION_HANDLES_COMMAND_LINE,
                          NULL);
}

static void cream_browser_class_init (CreamBrowserClass *klass)
{
     G_OBJECT_CLASS (klass)->finalize = cream_browser_finalize;

     G_APPLICATION_CLASS (klass)->activate     = cream_browser_activate;
     G_APPLICATION_CLASS (klass)->command_line = cream_browser_command_line;

     klass->error = cream_browser_error_handler;
}

static void cream_browser_init (CreamBrowser *self)
{
     self->url       = NULL;
     self->config    = NULL;
     self->sockpath  = NULL;
     self->cmd       = NULL;

     self->profile   = NULL;

     self->log       = FALSE;
     self->version   = FALSE;
     self->checkconf = FALSE;
}

/* Callbacks */

/*! Error handler */
static void cream_browser_error_handler (CreamBrowser *self, gboolean abort, GError *error)
{
     gchar *str;

     g_return_if_fail (error != NULL);

     str = g_strdup_printf (_("Error (%s:%d): %s\n"), g_quark_to_string (error->domain), error->code, error->message);
     g_error_free (error);

     if (self->flog)
          fprintf (self->flog, "%s", str);
     fprintf (stderr, "%s", str);

     if (abort)
     {
          g_application_command_line_set_exit_status (self->gappcmdline, EXIT_FAILURE);
          g_application_release (G_APPLICATION (self));
     }
}

static void cream_browser_finalize (GObject *obj)
{
     CreamBrowser *self = CREAM_BROWSER (obj);
     GError *error = NULL;

     G_APPLICATION_CLASS (cream_browser_parent_class)->quit_mainloop (G_APPLICATION (self));

     if (self->sock)
     {
          if (!g_socket_close (G_SOCKET (self->sock), &error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          unlink (self->sockpath);
          g_object_unref (self->sock);
     }

     g_hash_table_remove_all (self->protocols);

     lua_ctx_close ();
     g_free (self->profile);

     if (self->flog) fclose (self->flog);

     G_OBJECT_CLASS (cream_browser_parent_class)->finalize (G_OBJECT (self));
}

/*! Run the browser */
static void cream_browser_activate (GApplication *gapp)
{
     CreamBrowser *self = CREAM_BROWSER (gapp);
     GError *error = NULL;

     /* cream-browser -v */
     if (self->version)
     {
          printf (_("%s %s, developped by David Delassus <david.jose.delassus@gmail.com>\n"), PACKAGE, VERSION);
          printf (_("Released under %s license.\n"), LICENSE);

          printf (_("Builded with:\n"));
          printf (" - GLib %s\n", LIB_GLIB_VERSION);
          printf (" - GTK+ %s\n", LIB_GTK_VERSION);
          printf (" - Lua  %s\n\n", LIB_LUA_VERSION);

          printf (_("Builded on %s with:\n"), ARCH);
          printf (" - C Compiler: %s\n", COMPILER);
          printf (" - CFLAGS:     %s\n", CFLAGS);
          printf (" - LDFLAGS:    %s\n", LDFLAGS);

          if (HAVE_DEBUG)
               printf (_("Builded in debug mode.\n"));
          else
               printf (_("Builded in release mode.\n"));

          g_application_release (G_APPLICATION (self));
     }

     /* cream-browser -k */
     if (self->checkconf)
     {
          char *rc = self->config;

          /* find lua config */
          if (!rc || !g_file_test (rc, G_FILE_TEST_EXISTS))
          {
               if ((rc = find_file (FILE_TYPE_CONFIG, "rc.lua")) == NULL)
               {
                    error = g_error_new (CREAM_BROWSER_ERROR, CREAM_BROWSER_ERROR_CONFIG, _("Configuration not found."));
                    CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);
               }
          }

          /* init and parse lua */
          if (!lua_ctx_init (&error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          if (!lua_ctx_parse (rc, &error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          lua_ctx_close ();

          printf ("No errors found.\n");
          g_application_release (G_APPLICATION (self));
     }

     /* creamctl */
     if (self->cmd != NULL)
          cream_browser_ctl (self);

     /* cream-browser -p "profile" */
     if (self->profile == NULL)
          self->profile = g_strdup ("default");

     /* initialize */
     cream_browser_startup (self);

     if (self->url)
     {
          gchar *cmd = g_strdup_printf ("open %s", self->url);

          if (!run_command (cmd, &error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, FALSE, error);

          g_free (cmd);
     }

     gtk_main ();
}

/*! Parse command line */
static gint cream_browser_command_line (GApplication *gapp, GApplicationCommandLine *cmdline)
{
     CreamBrowser *self = CREAM_BROWSER (gapp);
     self->gappcmdline = cmdline;

     GOptionEntry options[] =
     {
          { "chkcfg",  'k', 0, G_OPTION_ARG_NONE,    &self->checkconf,        gettext_noop ("Check the validity of the lua configuration"), NULL },
          { "log",     'l', 0, G_OPTION_ARG_NONE,    &self->log,              gettext_noop ("Enable logging"), NULL },
          { "open",    'o', 0, G_OPTION_ARG_STRING,  &self->url,              gettext_noop ("Open URL"), NULL },
          { "config",  'c', 0, G_OPTION_ARG_STRING,  &self->config,           gettext_noop ("Load an alternate config file."), NULL },
          { "socket",  's', 0, G_OPTION_ARG_STRING,  &self->sockpath,         gettext_noop ("Unix socket's path"), NULL },
          { "command", 'e', 0, G_OPTION_ARG_STRING,  &self->cmd,              gettext_noop ("Send a command on the specified socket (see --socket,-s)"), NULL },
          { "profile", 'p', 0, G_OPTION_ARG_STRING,  &self->profile,          gettext_noop ("Select a profile (default='default')"), NULL },
          { "version", 'v', 0, G_OPTION_ARG_NONE,    &self->version,          gettext_noop ("Show version informations"), NULL },
          { NULL }
     };

     GOptionContext *optctx;
     GError *error = NULL;

     gchar **argv;
     gint argc;

     /* get argv/argc */
     argv = g_application_command_line_get_arguments (cmdline, &argc);

     /* parse command line */
     optctx = g_option_context_new (" - Web browser");
     g_option_context_add_main_entries (optctx, options, PACKAGE);
     g_option_context_add_group (optctx, gtk_get_option_group (TRUE));

     if (!g_option_context_parse (optctx, &argc, &argv, &error) && error != NULL)
          CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

     g_application_activate (gapp);
     return EXIT_SUCCESS;
}

/*! Initialize the browser */
static void cream_browser_startup (CreamBrowser *self)
{
     GError *error = NULL;
     char *rc = self->config;

     /* init threads */
     if (!g_thread_supported ())
          g_thread_init (NULL);

     /* open log */
     if (self->log || HAVE_DEBUG)
     {
          gchar *path = g_build_filename (g_get_user_config_dir (), g_get_prgname (), PACKAGE ".log", NULL);
          self->flog = fopen (path, "a");
     }

     /* init modules */
     self->protocols = g_hash_table_new (g_str_hash, g_str_equal);
     modules_init ();

     /* init socket */
     if ((self->sock = socket_new (&error)) == NULL)
          CREAM_BROWSER_GET_CLASS (self)->error (self, FALSE, error);

     /* init gui */
     self->theme = CREAM_THEME (g_object_new (CREAM_TYPE_THEME, NULL));
     ui_init ();
     keybinds_init ();

     /* find lua config */
     if (!rc || !g_file_test (rc, G_FILE_TEST_EXISTS))
     {
          if ((rc = find_file (FILE_TYPE_CONFIG, "rc.lua")) == NULL)
          {
               error = g_error_new (CREAM_BROWSER_ERROR, CREAM_BROWSER_ERROR_CONFIG, _("Configuration not found."));
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);
          }
     }

     /* init and parse lua */
     if (!lua_ctx_init (&error))
          CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

     if (!lua_ctx_parse (rc, &error))
          CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

     ui_show ();
}

/*! Send command on the socket */
static void cream_browser_ctl (CreamBrowser *self)
{
     GError *error = NULL;
     GSocketAddress *addr;
     GSocket *s;
     char tmp;
     int len;

     if (self->sockpath && self->cmd)
     {
          s    = g_socket_new (G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, &error);
          addr = g_unix_socket_address_new (self->sockpath);

          if (s == NULL && error != NULL)
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          if (!g_socket_connect (s, addr, NULL, &error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          if (!g_socket_send (s, self->cmd, strlen (self->cmd), NULL, &error)
              || !g_socket_send (s, "\n", 1, NULL, &error))
          {
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);
          }

          while ((len = g_socket_receive (s, &tmp, 1, NULL, &error)))
          {
               if (len == -1 || error != NULL)
                    CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

               putchar (tmp);
               if (tmp == '\n')
                    break;
          }

          if (!g_socket_close (s, &error))
               CREAM_BROWSER_GET_CLASS (self)->error (self, TRUE, error);

          g_application_release (G_APPLICATION (self));
     }
     else
     {
          fprintf (stderr, _("Usage: cream-browser -s /path/to/socket -e \"command\"\n"));
          g_application_command_line_set_exit_status (self->gappcmdline, EXIT_FAILURE);
          g_application_release (G_APPLICATION (self));
     }
}

/* Accessors */

/*! @return The current focused webview */
GtkWidget *cream_browser_get_focused_webview (CreamBrowser *self)
{
     return self->gui.fwebview;
}

/*! @param webview The current focused webview */
void cream_browser_set_focused_webview (CreamBrowser *self, GtkWidget *webview)
{
     self->gui.fwebview = webview;
}

/* Methods */

/*!
 * @param scheme The protocol to manage
 * @param mod A #CreamModule object.
 */
void cream_browser_add_protocol (CreamBrowser *self, const gchar *scheme, GObject *mod)
{
     g_hash_table_insert (self->protocols, g_strdup (scheme), mod);
}

/*!
 * @param mod The #CreamModule object to delete.
 */
void cream_browser_del_protocol (CreamBrowser *self, GObject *mod)
{
     g_hash_table_foreach_remove (self->protocols, (GHRFunc) g_direct_equal, mod);
}

/*!
 * @param scheme The protocol
 * @return The #CreamModule object associated to the protocol
 */
GObject *cream_browser_get_protocol (CreamBrowser *self, const gchar *scheme)
{
     return (GObject *) g_hash_table_lookup (self->protocols, scheme);
}

/*! @} */
