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

#ifndef __LOCAL_H
#define __LOCAL_H

/*!
  @defgroup libcream Internal browser library
  @brief This library implements every tools needed by the browser.

  Tools like #CreamView object, confuse integration (for the
  configuration file), and other.
 */

/*!
  @mainpage

  This manual documents the <strong>Cream-Browser C API</strong>
  of its library. It describe how to use the main objects of the
  browser (like #CreamView). If you need some help even with this
  documentation, you can come on the IRC channel :

  <strong>Server :</strong> Freenode<br/>
  <strong>Channel :</strong> \#cream-browser

  http://cream-browser.net
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>

#include <libconfig.h>

#include "CreamTabbed.h"

#include "xdg.h"
#include "structs.h"
#include "interface.h"
#include "callbacks.h"
#include "command.h"
#include "bind.h"

extern struct global_t global;

char *str_replace (const char *search, const char *replace, const char *string);
gboolean cream_init (int *argc, char ***argv, GError **error);
void cream_release (int exit_code);
CreamTabbed *get_current_creamtabbed (void);
void run_command (const gchar *cmd, GString **ret, CreamTabbed *obj);

gboolean set (char *key, char *value);
char *get (char *key);
void set_user_agent (WebKitWebSettings *settings, const gchar *uri);
void add_to_bookmarks (const gchar *uri, const gchar *title);

/* errors */
typedef enum
{
     CREAM_ERROR_XDGFILENOTFOUND,
     CREAM_ERROR_FAILED
} CreamError;

GQuark cream_error_quark (void);
#define CREAM_ERROR cream_error_quark ()

#define WEBKIT_MAJOR     g_strdup_printf ("%d", WEBKIT_MAJOR_VERSION)
#define WEBKIT_MINOR     g_strdup_printf ("%d", WEBKIT_MINOR_VERSION)
#define WEBKIT_MICRO     g_strdup_printf ("%d", WEBKIT_MICRO_VERSION)

#endif /* __LOCAL_H */
