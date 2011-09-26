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

#ifndef __STRUCTS_H
#define __STRUCTS_H

/*!
 * \struct GUI
 * GUI structure.
 */
struct GUI
{
     GtkWidget *window;         /*!< Main Window */
     GtkWidget *vimsplit;       /*!< Split area */
     GtkWidget *statusbar;      /*!< Statusbar widget */
     GtkWidget *inputbox;       /*!< Inputbox widget */
     GtkWidget *box;            /*!< Box containing all widgets */

     GtkWidget *fwebview;       /*!< Current focused webview */
};

/*!
 * \struct global_t
 * Global structure to access in every point of the program to all usefull structures and widgets
 */
struct global_t
{
     struct GUI gui;          /*!< GUI */
     Theme *theme;            /*!< Cream-Browser's theme */

     gboolean log;            /*!< if <code>TRUE</code> log every errors */
     FILE *flog;              /*!< Log's file descriptor */
     gchar *prgname;          /*!< Program's name */
     gchar *profile;          /*!< Cream-Browser's profile */

     GHashTable *protocols;   /*!< List of protocols and asscoiated modules */
     CreamMode mode;          /*!< Current mode */

     lua_State *luavm;        /*!< Lua VM state */
     Socket *sock;            /*!< Unix socket */
     Cache *cache;            /*!< Cream-Browser's cache */
};

#endif /* __STRUCTS_H */
