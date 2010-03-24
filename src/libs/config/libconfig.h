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
          char *encodage;
          gboolean javascript;

          struct user_agent_t *user_agent;

          char *bookmarks;
          char *history;
          char *cookie;
     } global;

     struct handler_t *handlers;
     struct key_t *keys;
};

gboolean cream_config_load (gchar *path, struct cream_config_t *cfg);
void cream_config_free (struct cream_config_t *cfg);

#endif /* __LIB_CONFIG_H */
