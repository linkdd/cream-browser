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

#include <config_build.h>

#include <webkit/webkit.h>
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

#include "xdg.h"
#include "structs.h"

#include "errors.h"
#include "modules.h"
#include "lua.h"
#include "config.h"

#include "marshal.h"
#include "WebViewClass.h"

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

void socket_init (void);

char *str_replace (const char *search, const char *replace, const char *string);
void add_protocol (guint id, CreamModule *mod);
CreamModule *get_protocol (guint id);

/*! @} */

#endif /* __LOCAL_H */
