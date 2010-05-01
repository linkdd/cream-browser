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

#ifndef __LIB_CONFIG_H
#define __LIB_CONFIG_H

#include <stdlib.h>
#include <glib.h>
#include <confuse.h>

struct user_agent_t
{
     char *domain;
     char *name;

     struct user_agent_t *next;
};

struct handler_t
{
     char *name;
     char *cmd;

     struct handler_t *next;
};

struct key_t
{
     char *bind;
     char *cmd;

     struct key_t *next;
};

struct cream_config_t
{
     struct
     {
          char *homepage;
          char *encoding;
          gboolean javascript;

          struct user_agent_t *user_agent;

          char *bookmarks;
          char *history;
          char *cookie;
     } global;

     struct handler_t *handlers;
     struct key_t *keys;
};

gboolean cream_config_load (gchar *path, struct cream_config_t *cfg, GError **error);
void cream_config_free (struct cream_config_t *cfg);

#endif /* __LIB_CONFIG_H */
