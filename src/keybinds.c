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
 * \addtogroup keybinds
 *
 * @{
 */

static GSList *keys = NULL;

static gboolean keybinds_callback (GtkWindow *window, GdkEvent *event)
{
     static GString *command = NULL;

     guint modifiers = gtk_accelerator_get_default_mod_mask ();
     GdkEventKey ekey = event->key;
     GSList *tmp;

     /* Escape is the default key to clear the buffer */
     if (g_str_equal ("Escape", gdk_keyval_name (ekey.keyval)) && command)
          g_string_free (command, TRUE), command = NULL;

     /* initialize buffer */
     if (command == NULL) command = g_string_new ("");

     /* get key */
     if (ekey.is_modifier)
     {
          return FALSE;
     }
     else if (app->mode & (CREAM_MODE_NORMAL | CREAM_MODE_EMBED | CREAM_MODE_CARET))
          command = g_string_append (command, gdk_keyval_name (ekey.keyval));

     /* check for keybind */
     for (tmp = keys; tmp != NULL; tmp = tmp->next)
     {
          struct key_t *keybind = (struct key_t *) tmp->data;

          if (g_string_equal (command, keybind->cmd) && (ekey.state & modifiers) == keybind->modmask && (app->mode & keybind->statemask))
          {
               /* call lua callback */
               lua_pushwebview (app->luavm, CREAM_WEBVIEW (cream_browser_get_focused_webview (app)));
               luaL_callfunction (app->luavm, keybind->func, 1, 0);
               /* free current buffer */
               g_string_free (command, TRUE), command = NULL;
               return TRUE;
          }
     }

     return FALSE;
}

/*! Initialize keybindings */
void keybinds_init (void)
{
     g_signal_connect (G_OBJECT (app->gui.window), "key-press-event", G_CALLBACK (keybinds_callback), NULL);
}


/*!
 * \fn void keybinds_add (int statemask, int modmask, const char *cmd, int lua_func)
 * @param statemask Browser's mode in which the keybind is affected.
 * @param modmask Modifier keys.
 * @param cmd Command.
 * @param lua_func Reference on a lua function.
 *
 * Add a key binding.
 */
void keybinds_add (int statemask, int modmask, const char *cmd, int lua_func)
{
     struct key_t *keybind = g_new0 (struct key_t, 1);

     keybind->statemask = statemask;
     keybind->modmask   = modmask;
     keybind->cmd       = g_string_new (cmd);
     keybind->func      = lua_func;

     keys = g_slist_append (keys, keybind);
}

/*! @} */
