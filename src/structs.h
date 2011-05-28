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

/*! \struct GUI */
struct GUI
{
     GtkWidget *window;         /*!< Main Window */
     GtkWidget *splittedwindow; /*!< Split area */
     GtkWidget *statusbar;      /*!< Statusbar widget */
     GtkWidget *promptbox;      /*!< Promptbox widget */
     GtkWidget *box;            /*!< Box containing all widgets */

     SplittedWindow *sw;
};

/*! \struct Socket */
struct Socket
{
     gchar *path;             /*!< Path to the Unix socket */
     int fd;                  /*!< Socket file descriptor */
     GIOChannel *channel;     /*!< \class{GIOChannel} to use the socket */
};

/*!
 * \struct global_t
 * \brief Global structure to access in every point of the program to all usefull structures and widgets
 */
struct global_t
{
     struct GUI gui;          /*!< GUI */
     struct Socket sock;      /*!< Unix socket */

     gboolean log;            /*!< if <code>TRUE</code> log every errors */
     FILE *flog;              /*!< Log's file descriptor */
     gchar *prgname;          /*!< Program's name */

     GHashTable *protocols;   /*!< List of protocols and asscoiated modules */

     lua_State *luavm;        /*!< Lua VM state */
     struct theme_t *theme;   /*!< Cream-Browser's theme */
};

#endif /* __STRUCTS_H */
