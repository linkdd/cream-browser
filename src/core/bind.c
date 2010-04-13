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

static GString *bind_buffer = NULL;

static gboolean bind_parse_buffer (CreamTabbed *obj)
{
     struct key_t *tmp = global.cfg.keys;

     for (; tmp != NULL; tmp = tmp->next)
     {
          if (g_str_equal (bind_buffer->str, tmp->bind))
          {
               g_string_free (bind_buffer, TRUE);
               bind_buffer = NULL;
               return run_command (tmp->cmd, NULL, obj);
          }
     }

     return FALSE;
}

gboolean bind_getkey (CreamView *creamview, GdkEventKey *event, CreamTabbed *obj)
{
     gboolean ret = TRUE;

     global.browser.mode = BindMode;

     if (obj == NULL)
          obj = get_current_creamtabbed ();

     if (bind_buffer == NULL)
          bind_buffer = g_string_new ("");

     switch (event->keyval)
     {
          case GDK_Escape:
          {
               GtkWidget *content = cream_view_get_content (CREAM_VIEW (obj->creamview));

               if (MODULE_IS_WEB_VIEW (content))
               {
                    WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));
                    g_object_set (settings, "enable-caret-browsing", FALSE, NULL);
                    webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
               }

               echo (obj, "");
               gtk_widget_grab_focus (obj->creamview);
               global.browser.mode = InsertMode;
               break;
          }

          case GDK_Insert:
          {
               GtkWidget *content = cream_view_get_content (CREAM_VIEW (obj->creamview));

               if (MODULE_IS_WEB_VIEW (content))
               {
                    WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));
                    g_object_set (settings, "enable-caret-browsing", TRUE, NULL);
                    webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
               }

               echo (obj, "-- CARET --");
               gtk_widget_grab_focus (obj->creamview);
               break;
          }

          case GDK_colon:
               echo (obj, ":");
               gtk_widget_grab_focus (obj->inputbox);
               gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
               break;

          case GDK_slash:
               echo (obj, "/");
               gtk_widget_grab_focus (obj->inputbox);
               gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
               break;

          default:
               if (event->string != NULL)
                    bind_buffer = g_string_append (bind_buffer, event->string);
               ret = FALSE;
               break;
     }

     if (ret)
     {
          g_string_free (bind_buffer, TRUE);
          bind_buffer = NULL;
     }
     else
          bind_parse_buffer (obj);

     return ret;
}
