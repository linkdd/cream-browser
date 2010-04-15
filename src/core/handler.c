/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
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
gboolean handle_yank (int argc, char **argv, GString **ret, CreamTabbed *obj);
gboolean handle_paste (int argc, char **argv, GString **ret, CreamTabbed *obj);
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
     { "yank",     handle_yank },
     { "paste",    handle_paste },
     { "set",      handle_set },
     { "get",      handle_get },
     { "bind",     handle_bind },
     { "quit",     handle_quit },
     { NULL, NULL }
};


gboolean run_command (const gchar *cmd, GString **ret, CreamTabbed *obj)
{
     GError *error = NULL;
     gint argc;
     gchar **argv;
     int i;

     if (!g_shell_parse_argv (cmd, &argc, &argv, &error) || error != NULL)
     {
          if (ret != NULL)
               *ret = g_string_new (error->message);
          g_error_free (error);
          return FALSE;
     }

     for (i = 0; cmd_handlers[i].argv0 != NULL; ++i)
          if (g_str_equal (cmd_handlers[i].argv0, argv[0]))
               if (cmd_handlers[i].func != NULL)
                    return cmd_handlers[i].func (argc, argv, ret, obj);

     if (ret != NULL)
          *ret = g_string_new (g_strdup_printf ("Error: The command '%s' isn't a browser's command.\n", argv[0]));

     return FALSE;
}

gboolean handle_spawn (int argc, char **argv, GString **ret, CreamTabbed *obj)
{
     return FALSE;
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

     if (g_str_equal (argv[1], "%s"))
     {
          echo (obj, ":open ");
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

     if (g_str_equal (argv[1], "%s"))
     {
          echo (obj, ":tabopen ");
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
}
