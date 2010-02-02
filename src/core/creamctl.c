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
     g_option_context_add_group (context, gtk_get_option_group (TRUE));
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
          fprintf (stderr, "Usage: Cream-Browserctrl -s /path/to/socket -c \"command\"");
          return EXIT_FAILURE;
     }
}

