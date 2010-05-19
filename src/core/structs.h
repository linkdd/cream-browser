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

#ifndef __STRUCTS_H
#define __STRUCTS_H

struct variable_t
{
     char *key;
     char *value;
     gboolean readwrite; /* FALSE : read-only, TRUE : read-write */

     enum { String, Integer, Boolean, None } type;
     void *data;
};

struct global_t
{
     struct
     {
          gchar *config;
          gchar *url;
     } cmdline;

     struct
     {
          enum { InsertMode = 0, BindMode, CmdMode, HintsMode } mode;
          SoupCookieJar *cookies;
          GtkClipboard *clipboard;
          GSList *variables;
          gchar *main_user_agent;
     } browser;

     struct
     {
          GIOChannel *channel;
          gchar *path;
     } unix_sock;

     struct utsname uname;
     struct cream_config_t cfg;
     GtkNotebook *notebook;
};

struct cmd_t
{
     char *id;
     int (*func)(int argc, char **argv, CreamTabbed *obj);
};

#endif /* __STRUCTS_H */
