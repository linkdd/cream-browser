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

#ifndef __LOCAL_H
#define __LOCAL_H

/*!
 * \mainpage Cream-Browser's Documentation
 *
 * \section intro What is Cream-Browser ?
 *
 * Cream-Browser is a light web browser using <a href="http://www.gtk.org/">GTK+</a>.
 * Its interface is the same as <a href="http://vimperator.org">Vimperator</a>
 * (a famous Firefox plugin to use it like the text editor : Vim).
 *
 * \section contact Contact
 *
 * On IRC :
 * <ul>
 *   <li>Server: <em><a>irc.freenode.net</a></em></li>
 *   <li>Channel: <strong>\#cream-browser</strong></li>
 * </ul>
 *
 * Or via mail :
 * <ul>
 *   <li>David Delassus &lt; <a href="#">david <strong>(dot)</strong> jose <strong>(dot)</strong> delassus <strong>(at)</strong> gmail <strong>(dot)</strong> com</a> &gt;</li>
 *   <li>Mailing list : <a href="http://groups.google.com/group/cream-browser-devel">Developers</a> -
 *       <a href="http://groups.google.com/group/cream-browser-users">Users</a></li>
 * </ul>
 *
 */

#include <cream-browser_build.h>

#include <locale.h>
#include <libintl.h>

#define _(str)                gettext(str)
#define gettext_noop(str)     str

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* For sockets */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include "marshal.h"
#include "WebView.h"
#include "Notebook.h"
#include "GtkVimSplit.h"

#include "lua.h"
#include "modules.h"
#include "interface.h"

#include "structs.h"

/*!
 * \defgroup cream-browser Cream-Browser Utilities.
 * Useful functions and declarations.
 *
 * \todo Add support of cookies.
 * \todo Implements dynamic proxy (ie: proxy enabled for a specified website).
 * \todo Add session support (ie: saving multiple sessions and choose which to restore at start, maybe with simple files ?).
 * \todo Create Vim-like interface.
 *
 * @{
 */

extern struct global_t global;


#define FILE_TYPE_CONFIG           (1 << 0)       /*!< Used to search a configuration file into standard directories. */
#define FILE_TYPE_DATA             (1 << 1)       /*!< Used to search a ressource into standard directories. */
#define FILE_TYPE_CACHE            (1 << 2)       /*!< Used to search a cache file into standard directories. */

gchar *find_file (guint type, const gchar *filename);

gboolean socket_init (GError **err);

char *str_replace (const char *search, const char *replace, const char *string);
void add_protocol (const gchar *scheme, CreamModule *mod);
void del_protocol (CreamModule *mod);
CreamModule *get_protocol (const gchar *scheme);

void print_error (GError *error, gboolean abort, const gchar *fmt, ...);

/*! @} */

#endif /* __LOCAL_H */
