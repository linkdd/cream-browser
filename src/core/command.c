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

int cmd_open (int argc, char **argv, CreamTabbed *obj);
int cmd_tabopen (int argc, char **argv, CreamTabbed *obj);
int cmd_close (int argc, char **argv, CreamTabbed *obj);
int cmd_set (int argc, char **argv, CreamTabbed *obj);
int cmd_get (int argc, char **argv, CreamTabbed *obj);
int cmd_bind (int argc, char **argv, CreamTabbed *obj);
int cmd_quit (int argc, char **argv, CreamTabbed *obj);

struct cmd_t commands[] =
{
     { "open",    cmd_open },
     { "tabopen", cmd_tabopen },
     { "close",   cmd_close },
     { "set",     cmd_set },
     { "get",     cmd_get },
     { "bind",    cmd_bind },
     { "quit",    cmd_quit },
     { NULL, NULL }
};

int getcmd (GtkEntry *inputbox, const gchar *text, CreamTabbed *obj)
{
     int ret = 0;

     if (text[0] == '/' && strlen (text) > 1)
     {
          if (MODULE_IS_WEB_VIEW (CREAM_VIEW (obj->creamview)->content))
          {
               WebKitWebView *view = WEBKIT_WEB_VIEW (CREAM_VIEW (obj->creamview)->content);

               webkit_web_view_mark_text_matches (view, text + 1, TRUE, 0);
               webkit_web_view_set_highlight_text_matches (view, TRUE);
          }
     }
     else if (text[0] == ':' && strlen (text) > 1)
     {
          int i, argc;
          char **argv;

          text++;
          argv = g_strsplit (text, " ", -1);
          argc = g_strv_length (argv);

          ret = -1;
          for (i = 0; commands[i].id != NULL; ++i)
          {
               if (g_str_equal (commands[i].id, argv[0]))
               {
                    if (commands[i].func != NULL)
                    {
                         ret = commands[i].func (argc, argv, obj);
                         break;
                    }
               }
          }
     }
     else
     {
          ret = -1;
     }

     gtk_entry_set_text (inputbox, "");
     return ret;
}

void cb_inputbox (GtkEntry *inputbox, CreamTabbed *obj)
{
     const gchar *text = gtk_entry_get_text (inputbox);

     g_return_if_fail (text != NULL);
     g_return_if_fail (strlen (text) != 0);

     if (global.browser.mode == CmdMode)
     {
          if (MODULE_IS_WEB_VIEW (CREAM_VIEW (obj->creamview)->content))
               webkit_web_view_unmark_text_matches (WEBKIT_WEB_VIEW (CREAM_VIEW (obj->creamview)->content));

          getcmd (inputbox, text, obj);
     }

     global.browser.mode = BindMode;
}

int cmd_open (int argc, char **argv, CreamTabbed *obj)
{
     int i;

     for (i = 1; i < argc; ++i)
          cream_tabbed_load_uri (obj, argv[i]);

     return 0;
}

int cmd_tabopen (int argc, char **argv, CreamTabbed *obj)
{
     int i;

     if (argc == 1 || strlen (argv[1]) == 0)
          notebook_append_page ("about:blank");
     else
          for (i = 1; i < argc; ++i)
               notebook_append_page (argv[i]);

     return 0;
}

int cmd_close (int argc, char **argv, CreamTabbed *obj)
{
     cb_cream_notebook_close_page (NULL, GTK_WIDGET (obj));
     return 0;
}

int cmd_set (int argc, char **argv, CreamTabbed *obj)
{
     if (argc < 3)
     {
          echo (obj, "Usage : set <key> <value>");
          return -1;
     }

     if (!set (argv[1], argv[2]))
     {
          echo (obj, "set: Can't set '%s' to '%s' (read-only)", argv[1], argv[2]);
          return -1;
     }

     return 0;
}

int cmd_get (int argc, char **argv, CreamTabbed *obj)
{
     char *tmp;

     if (argc < 2)
     {
          echo (obj, "Usage : get <key>");
          return -1;
     }

     tmp = get (argv[1]);

     if (tmp == NULL)
     {
          echo (obj, "get: Can't get value of '%s' (variable doesn't exist)", argv[1]);
          return -1;
     }

     echo (obj, "%s\n", tmp);
     return 0;
}

int cmd_bind (int argc, char **argv, CreamTabbed *obj)
{
     struct key_t *new, *tmp;

     if (argc < 3)
     {
          echo (obj, "Usage : bind <key> <cmd>");
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

int cmd_quit (int argc, char **argv, CreamTabbed *obj)
{
     cream_release (EXIT_SUCCESS);
     return 0;
}

