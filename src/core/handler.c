/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

#include "local.h"

struct handler_cmd_t
{
     char *argv0;
     gboolean (*func) (int argc, char **argv, GString **ret, CreamTabbed *obj);
};

gboolean handle_spawn (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_download (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_open (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_tabopen (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_close (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_yank (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_paste (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_buffers (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_set (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_get (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_bind (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_quit (int argc, char **argv, GString **ret, CreamTabbed *obj);

static struct handler_cmd_t cmd_handlers[] =
{
     { "spawn",    handle_spawn },
     { "download", handle_download },
     { "open",     handle_open },
     { "tabopen",  handle_tabopen },
     { "close",    handle_close },
     { "yank",     handle_yank },
     { "paste",    handle_paste },
     { "buffers",  handle_buffers },
     { "set",      handle_set },
     { "get",      handle_get },
     { "bind",     handle_bind },
     { "quit",     handle_quit },
     { NULL, NULL }
};


gboolean run_command (const gchar *cmd, GString **ret, CreamTabbed *obj)
{
     GError *error = NULL;
     gboolean retval = TRUE;
     gchar **commands;
     int i;

     commands = g_strsplit (cmd, ";", -1);

     for (i = 0; i < g_strv_length (commands); ++i)
     {
          gint argc;
          gchar **argv;
          int j;

          if (!g_shell_parse_argv (commands[i], &argc, &argv, &error) || error != NULL)
          {
               if (ret != NULL)
                    *ret = g_string_new (error->message);
               g_error_free (error);
               retval = FALSE;
               break;
          }

          for (j = 0; cmd_handlers[j].argv0 != NULL; ++j)
          {
               if (g_str_equal (cmd_handlers[j].argv0, argv[0]))
               {
                    if (cmd_handlers[j].func != NULL)
                    {
                         retval = cmd_handlers[j].func (argc, argv, ret, obj);
                         break;
                    }
               }
          }


          if (!retval && ret != NULL)
               *ret = g_string_new (g_strdup_printf ("Error: The command '%s' isn't a browser's command.\n", argv[0]));
     }

     g_strfreev (commands);
     return retval;
}

gboolean handle_spawn (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     /* command <pid> <config path> <socket path> [args...] */
     gchar **child_argv;
     GError *error = NULL;
     GPid child;
     int i, j;

     if (argc < 2)
     {
          if (ret != NULL)
               *ret = g_string_new ("spawn: need an argument\n");
          return FALSE;
     }

     child_argv = calloc (3 + argc, sizeof (char *));

     child_argv[0] = g_strdup (argv[1]);
     child_argv[1] = g_strdup_printf ("%d", getpid ());
     child_argv[2] = global.cmdline.config;
     child_argv[3] = global.unix_sock.path;

     for (i = 4, j = 2; j < argc; ++i, ++j)
     {
          child_argv[i] = g_strdup (argv[j]);
     }
     child_argv[i] = NULL;

     if (!g_spawn_async (NULL, child_argv, NULL, 0, NULL, NULL, &child, &error))
     {
          if (ret != NULL)
               *ret = g_string_new (error->message);
          g_error_free (error);
          return FALSE;
     }

     if (ret != NULL)
          *ret = g_string_new (g_strdup_printf ("spawn: child process <%d>\n", child));

     return TRUE;
}

gboolean handle_download (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     if (ret != NULL)
          *ret = g_string_new (g_strdup_printf ("Program: %s\n", argv[0]));
     return TRUE;
}

gboolean handle_open (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     if (argc < 2 && ret != NULL)
     {
          *ret = g_string_new ("The function 'open' need an argument.\n");
          return FALSE;
     }

     if (g_str_has_suffix (argv[1], "%s"))
     {
          if (g_str_equal (argv[1], "%s"))
               echo (obj, ":open ");
          else
          {
               argv[1][strlen (argv[1]) - 2] = 0;
               echo (obj, ":open %s", argv[1]);
          }
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@uri@"))
     {
          echo (obj, ":open %s", cream_tabbed_get_uri (obj));
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@home@"))
     {
          if (global.cfg.global.homepage != NULL)
               cream_tabbed_load_uri (obj, global.cfg.global.homepage);
          else
               cream_tabbed_load_uri (obj, "http://cream-browser.net");
          global.browser.mode = BindMode;
     }
     else
     {
          cream_tabbed_load_uri (obj, argv[1]);
          global.browser.mode = BindMode;
     }

     return TRUE;
}

gboolean handle_tabopen (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     if (argc < 2 && ret != NULL)
     {
          *ret = g_string_new ("The function 'open' need an argument.\n");
          return FALSE;
     }

     if (g_str_has_suffix (argv[1], "%s"))
     {
          if (g_str_equal (argv[1], "%s"))
               echo (obj, ":tabopen ");
          else
          {
               argv[1][strlen (argv[1]) - 2] = 0;
               echo (obj, ":tabopen %s", argv[1]);
          }
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@uri@"))
     {
          echo (obj, ":tabopen %s", cream_tabbed_get_uri (obj));
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@home@"))
     {
          if (global.cfg.global.homepage != NULL)
               notebook_append_page (global.cfg.global.homepage);
          else
               notebook_append_page ("http://cream-browser.net");
          global.browser.mode = BindMode;
     }
     else
     {
          notebook_append_page (argv[1]);
          global.browser.mode = BindMode;
     }

     return TRUE;
}

gboolean handle_close (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     cb_cream_notebook_close_page (NULL, GTK_WIDGET (obj));
     return TRUE;
}

gboolean handle_yank (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     const gchar *uri = cream_tabbed_get_uri (obj);
     echo (obj, "Yanked %s", uri);
     gtk_clipboard_set_text (global.browser.clipboard, uri, -1);
     return TRUE;
}

gboolean handle_paste (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     gboolean tabopen = FALSE;
     gchar *uri = gtk_clipboard_wait_for_text (global.browser.clipboard);

     if (argc >= 2 && g_str_equal (argv[1], "tab"))
          tabopen = TRUE;

     if (tabopen)
          notebook_append_page (uri);
     else
          cream_tabbed_load_uri (obj, uri);

     return TRUE;
}

gboolean handle_buffers (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     GString *tmp = g_string_new ("");
     int i;

     for (i = 0; i < gtk_notebook_get_n_pages (global.notebook); ++i)
     {
          CreamTabbed *tab = CREAM_TABBED (gtk_notebook_get_nth_page (global.notebook, i));
          tmp = g_string_append (tmp, cream_tabbed_get_uri (tab));
          tmp = g_string_append (tmp, " ");
     }
     tmp = g_string_append (tmp, "\n");

     if (ret != NULL)
          *ret = tmp;

     return TRUE;
}

gboolean handle_set (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     if (argc < 3)
     {
          if (ret != NULL)
               *ret = g_string_new ("Usage : set <key> <value>\n");
          return FALSE;
     }

     if (!set (argv[1], argv[2]))
     {
          if (ret != NULL)
               *ret = g_string_new (g_strdup_printf ("set: Can't set '%s' to '%s' (read-only)\n", argv[1], argv[2]));
          return FALSE;
     }

     return TRUE;
}

gboolean handle_get (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     char *tmp;

     if (argc < 2)
     {
          if (ret != NULL)
               *ret = g_string_new ("Usage : get <key>\n");
          return FALSE;
     }

     tmp = get (argv[1]);

     if (tmp == NULL)
     {
          if (ret != NULL)
               *ret = g_string_new (g_strdup_printf ("get: Can't get value of '%s' (variable doesn't exist)\n", argv[1]));
          return FALSE;
     }
     else
     {
          if (ret != NULL)
               *ret = g_string_new (g_strdup_printf ("%s\n", tmp));
     }

     return TRUE;
}

gboolean handle_bind (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     struct key_t *new, *tmp;

     if (argc < 3)
     {
          if (ret != NULL)
               *ret = g_string_new ("Usage : bind <key> <cmd>\n");
          return -1;
     }

     new = malloc (sizeof (struct key_t));

     new->bind = g_strdup (argv[1]);
     new->cmd  = g_strdup (argv[2]);
     new->next = NULL;

     if (global.cfg.keys == NULL)
          global.cfg.keys = new;
     else
     {
          for (tmp = global.cfg.keys; tmp->next != NULL; tmp = tmp->next);
          tmp->next = new;
     }

     return 0;
}


gboolean handle_quit (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     cream_release (EXIT_SUCCESS);
     return TRUE;
}
