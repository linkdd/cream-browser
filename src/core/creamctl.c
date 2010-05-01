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

#include <glib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

static gchar *sockpath;
static gchar *command;

static GOptionEntry entries[] =
{
     { "socket",  's', 0, G_OPTION_ARG_STRING, &sockpath, "Path to the Cream-Browser socket",     NULL },
     { "command", 'c', 0, G_OPTION_ARG_STRING, &command,  "The Cream-Browser command to execute", NULL },
     { NULL, 0, 0, 0, NULL, NULL, NULL }
};

int main (int argc, char **argv)
{
     GError *error = NULL;
     GOptionContext* context = g_option_context_new ("- utility for controlling and interacting with Cream-Browser through its socket file");
     g_option_context_add_main_entries (context, entries, NULL);
     g_option_context_parse (context, &argc, &argv, &error);

     if (sockpath && command)
     {
          int s, len;
          struct sockaddr_un remote;
          char tmp;

          if ((s = socket (AF_UNIX, SOCK_STREAM, 0)) == -1)
          {
               perror ("socket");
               exit (EXIT_FAILURE);
          }

          remote.sun_family = AF_UNIX;
          strcpy (remote.sun_path, (char *) sockpath);
          len = strlen (remote.sun_path) + sizeof (remote.sun_family);

          if (connect (s, (struct sockaddr *) &remote, len) == -1)
          {
               perror ("connect");
               exit (EXIT_FAILURE);
          }

          if ((send (s, command, strlen (command), 0) == -1)
               || (send (s, "\n", 1, 0) == -1))
          {
               perror ("send");
               exit (EXIT_FAILURE);
          }

          while ((len = recv (s, &tmp, 1, 0)))
          {
               putchar (tmp);
               if (tmp == '\n')
                    break;
          }

          close(s);

          return EXIT_SUCCESS;
     }
     else
     {
          fprintf (stderr, "Usage: creamctl -s /path/to/socket -c \"command\"\n");
          return EXIT_FAILURE;
     }
}

