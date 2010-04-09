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

#ifndef __LOCAL_H
#define __LOCAL_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <webkit/webkit.h>
#include <curl/curl.h>
#include <gnet.h>

#include <libconfig.h>

#include "CreamTabbed.h"

#include "xdg.h"
#include "structs.h"
#include "interface.h"
#include "callbacks.h"
#include "command.h"
#include "bind.h"

extern struct global_t global;

gboolean cream_init (int *argc, char ***argv, GError **error);
void cream_release (int exit_code);
CreamTabbed *get_current_creamtabbed (void);
gboolean run_command (const gchar *cmd, GString **ret, CreamTabbed *obj);

/* errors */
typedef enum
{
     CREAM_ERROR_XDGFILENOTFOUND,
     CREAM_ERROR_FAILED
} CreamError;

GQuark cream_error_quark (void);
#define CREAM_ERROR cream_error_quark ()

#endif /* __LOCAL_H */
