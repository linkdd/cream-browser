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
 *   <li>Server: <a>irc.freenode.net</a></li>
 *   <li>Channel: <strong>\#cream-browser</strong></li>
 * </ul>
 *
 * Or via mail :
 * <ul>
 *   <li>David Delassus &lt; <a href="#">david <strong>(dot)</strong> jose <strong>(dot)</strong> delassus <strong>(at)</strong> gmail <strong>(dot)</strong> com</a> &gt;</li>
 *   <li>Mailing list &lt; <a href="#">cream-browser-devel@lists.sourceforge.net</a> &gt; (subscribe on : http://mails.cream-browser.net)</li>
 * </ul>
 *
 */

#include <cream-browser_build.h>

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
#include "WebViewClass.h"
#include "ViewAreaClass.h"
#include "CreamTabClass.h"

#include "lua.h"
#include "modules.h"
#include "interface.h"

#include "theme.h"
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

/*! \def FILE_TYPE_CONFIG */
#define FILE_TYPE_CONFIG           (1 << 0)
/*! \def FILE_TYPE_DATA */
#define FILE_TYPE_DATA             (1 << 1)
/*! \def FILE_TYPE_CACHE */
#define FILE_TYPE_CACHE            (1 << 2)

gchar *find_file (guint type, const gchar *filename);

gboolean socket_init (GError **err);

char *str_replace (const char *search, const char *replace, const char *string);
void add_protocol (const gchar *scheme, CreamModule *mod);
void del_protocol (CreamModule *mod);
CreamModule *get_protocol (const gchar *scheme);

void print_error (GError *error, gboolean abort, const gchar *fmt, ...);

/*! @} */

#endif /* __LOCAL_H */
