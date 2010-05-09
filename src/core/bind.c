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

GString *bind_buffer = NULL;

gboolean bind_parse_buffer (CreamTabbed *obj)
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
               GtkWidget *content = cream_view_get_content (creamview);

               if (MODULE_IS_WEB_VIEW (content))
               {
                    WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));
                    g_object_set (settings, "enable-caret-browsing", FALSE, NULL);
                    webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
               }

               echo (obj, "");
               gtk_widget_grab_focus (GTK_WIDGET (obj));
               global.browser.mode = BindMode;
               break;
          }

          case GDK_Insert:
          {
               GtkWidget *content = cream_view_get_content (creamview);

               if (MODULE_IS_WEB_VIEW (content))
               {
                    WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));
                    g_object_set (settings, "enable-caret-browsing", TRUE, NULL);
                    webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
               }

               echo (obj, "-- CARET --");
               gtk_widget_grab_focus (GTK_WIDGET (obj));
               global.browser.mode = InsertMode;
               break;
          }

          case GDK_colon:
               if (!gtk_entry_get_text (GTK_ENTRY (obj->inputbox)))
               {
                    echo (obj, ":");
                    gtk_widget_grab_focus (obj->inputbox);
                    gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
                    global.browser.mode = CmdMode;
               }
               break;

          case GDK_slash:
               if (!gtk_entry_get_text (GTK_ENTRY (obj->inputbox)))
               {
                    echo (obj, "/");
                    gtk_widget_grab_focus (obj->inputbox);
                    gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
                    global.browser.mode = CmdMode;
               }
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
          ret = bind_parse_buffer (obj);

     return ret;
}
