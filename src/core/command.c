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

void getcmd (GtkEntry *inputbox, const gchar *text, CreamTabbed *obj)
{
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
          cream_tabbed_load_uri (obj, text + 1);
     }
     else
     {
          printf ("Not a browser command : '%s'\n", text);
     }

     gtk_entry_set_text (inputbox, "");
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
