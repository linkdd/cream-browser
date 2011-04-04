#ifndef __LOCAL_H
#define __LOCAL_H

/*!
 * \mainpage Cream-Browser's Documentation
 *
 * \section intro What is Cream-Browser ?
 *
 * Cream-Browser is a light web browser based on the layout engine <a href="http://www.webkit.org">WebKit</a>
 * and using the library <a href="http://www.gtk.org/">GTK+</a>. Its interface is the same as <a href="http://vimperator.org">Vimperator</a>
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
 *   <li>David Delassus &lt; <a href="#">linkdd <strong>(at)</strong> ydb <strong>(dot)</strong> me</a> &gt;</li>
 *   <li>Mailing list &lt; <a href="#">cream-browser-devel@lists.sourceforge.net</a> &gt; (subscribe on : http://mail.cream-browser.net)</li>
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
