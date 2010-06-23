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

/*!
  @addtogroup libconfig
  @{
 */

#ifndef __LIB_CONFIG_H
#define __LIB_CONFIG_H

#include <stdlib.h>
#include <glib.h>
#include <confuse.h>

/*! \struct user_agent_t */
struct user_agent_t
{
     char *domain;                 /*!< Dynamic domain name */
     char *name;                   /*!< User-Agent to use according to the domain-name */

     struct user_agent_t *next;    /*!< Next list entry */
};

/*! \struct handler_t */
struct handler_t
{
     char *name;              /*!< Handler's name */
     char *cmd;               /*!< Command to execute */

     struct handler_t *next;  /*!< Next list entry */
};

/*! \struct key_t */
struct key_t
{
     char *bind;         /*!< Key-binding */
     char *cmd;          /*!< Command to execute when the user type the previous key-binding */

     struct key_t *next; /*!< Next list entry */
};

/*! \struct cream_config_t */
struct cream_config_t
{
     struct
     {
          char *homepage;                    /*!< Browser's homepage */
          char *encoding;                    /*!< Default encoding used for web-pages */
          gboolean javascript;               /*!< Is javascript enable ? */

          struct user_agent_t *user_agent;   /*!< List of user-agent */

          char *bookmarks;                   /*!< Bookmarks file */
          char *history;                     /*!< History file */
          char *cookie;                      /*!< Cookie file */
          char *proxy;                       /*!< proxy server */
     } global;                               /*!< Global browser options */

     struct handler_t *handlers;             /*!< List of handlers */
     struct key_t *keys;                     /*!< List of key-bindings */

     struct
     {
          struct
          {
               char *font;                   /*!< Font of the inputbox */
          } inputbox;                        /*!< Inputbox options */

          struct
          {
               char *font;                   /*!< Font of the statusbar */
               struct
               {
                    char *normal;            /*!< Color of normal website (for example: http://) */
                    char *ssl;               /*!< Color of SSL website (for example: https://) */
               } bg, fg;
          } statusbar;                       /*!< Statusbar options */

          struct
          {
               char *font;                   /*!< VTE Terminal font */
               char *shell;                  /*!< VTE Terminal shell */
               gboolean reverse;             /*!< Reverse color */
               gboolean doublebuffer;        /*!< Use doublebuffer */
               gboolean hints;
          } vte;                             /*!< VTE Terminal options */
     } gui;                                  /*!< GUI options */
};

gboolean cream_config_load (gchar *path, struct cream_config_t *cfg, GError **error);
void cream_config_free (struct cream_config_t *cfg);

/*! @} */

#endif /* __LIB_CONFIG_H */
