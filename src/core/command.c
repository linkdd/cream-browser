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
int cmd_quit (int argc, char **argv, CreamTabbed *obj);

struct cmd_t commands[] =
{
     { "open",    cmd_open },
     { "tabopen", cmd_tabopen },
     { "close",   cmd_close },
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

     if (MODULE_IS_WEB_VIEW (CREAM_VIEW (obj->creamview)->content))
          webkit_web_view_unmark_text_matches (WEBKIT_WEB_VIEW (CREAM_VIEW (obj->creamview)->content));

     getcmd (inputbox, text, obj);
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

     for (i = 1; i < argc; ++i)
          notebook_append_page (argv[i]);

     return 0;
}

int cmd_close (int argc, char **argv, CreamTabbed *obj)
{
     gtk_notebook_remove_page (global.notebook,
          gtk_notebook_get_current_page (global.notebook));
     return 0;
}

int cmd_quit (int argc, char **argv, CreamTabbed *obj)
{
     cream_release (EXIT_SUCCESS);
     return 0;
}
