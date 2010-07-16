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
  @defgroup handlers Commands handlers
  @brief Functions associated to commands used in creamctl and key-bindings

  \section errcode Error code

  Every handlers return an error code. This error code contains 3 numbers.
  -# The error code of the parser.
  -# The error code common to all handler.
  -# The specific error code of one handler.

  For example: "310"
  - 3 : The error comes from the handler
  - 1 : There is an "ArgumentsError" (too few)
  - 0 : There is no specific error

  @{
 */

#include "local.h"

/*!
  \def STATE0_NOERR
  \brief There is no error from the parser

  \def STATE0_PARSEERR
  \brief There is a "ParsingError" (ie. missing quote)

  \def STATE0_UNKNOWFUNC
  \brief The function called doesn't exist

  \def STATE0_FUNCERR
  \brief The error comes from the function
 */

#define STATE0_NOERR      0
#define STATE0_PARSEERR   1
#define STATE0_UNKNOWFUNC 2
#define STATE0_FUNCERR    3

/*!
  \def STATE1_NOERR
  \brief There is no error from the function

  \def STATE1_ARGERR
  \brief There is an "ArgumentsError" (too few ?)

  \def STATE1_UNDEF
  \brief The function is undefined, that means that the function exists but it is not yet available.

  \def STATE1_SPEC
  \brief The error is specific to the handler called
 */

#define STATE1_NOERR      0
#define STATE1_ARGERR     1
#define STATE1_UNDEF      2
#define STATE1_SPEC       3

/*!
  \def STATE2_NOERR
  \brief There is no specific error

  \def STATE2_UNKNOWERR
  \brief There is a unknow error
 */

#define STATE2_NOERR      0
#define STATE2_UNKNOWERR  1

/*! \struct handler_cmd_t */
struct handler_cmd_t
{
     char *argv0; /*!< Command's name */
     char (*func) (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
     /*!< Function to call

       \param argc Number of arguments
       \param argv Arguments list
       \param ret Variable to store the specific error code of the handler
       \param msg Message returned by the handler
       \param obj
       \return Function's error code
      */
};

char handle_spawn (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_download (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_open (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_tabopen (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_close (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_yank (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_paste (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_buffers (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_set (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_get (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_bind (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);
char handle_quit (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj);

/*!
  \var static struct handler_cmd_t cmd_handlers[]
  \brief List ofavailable commands
 */
static struct handler_cmd_t cmd_handlers[] =
{
     { "spawn",    handle_spawn },
     { "download", handle_download },
     { "open",     handle_open },
     { "tabopen",  handle_tabopen },
     { "close",    handle_close },
     { "yank",     handle_yank },
     { "paste",    handle_paste },
     { "buffers",  handle_buffers },
     { "set",      handle_set },
     { "get",      handle_get },
     { "bind",     handle_bind },
     { "quit",     handle_quit },
     { NULL, NULL }
};

static void set_msg (GString **msg, const char *format, ...)
{
     va_list args;

     if (msg != NULL)
     {
          va_start (args, format);
          *msg = g_string_new (g_strdup_vprintf (format, args));
          va_end (args);
     }
}

static gchar *strip_nl (gchar *string)
{
     return str_replace ("\n", " ", string);
}

void run_command (const gchar *cmd, GString **ret, CreamTabbed *obj)
{
     gboolean retval = FALSE;
     char state[3] = {0};

     GError *error = NULL;
     GString *msg = NULL;

     gchar **commands;
     int i;

     commands = g_strsplit (cmd, ";", -1);

     for (i = 0; i < g_strv_length (commands); ++i)
     {
          gint argc;
          gchar **argv;
          int j;

          if (!g_shell_parse_argv (commands[i], &argc, &argv, &error) || error != NULL)
          {
               msg = g_string_new (strip_nl (error->message));
               g_error_free (error);

               state[0] = STATE0_PARSEERR;
               retval = FALSE;
               break;
          }

          state[0] = STATE0_UNKNOWFUNC;
          for (j = 0; cmd_handlers[j].argv0 != NULL; ++j)
          {
               if (g_str_equal (cmd_handlers[j].argv0, argv[0]))
               {
                    if (cmd_handlers[j].func != NULL)
                    {
                         state[1] = cmd_handlers[j].func (argc, argv, &state[2], &msg, obj);
                         state[0] = STATE0_NOERR;
                         break;
                    }
               }
          }

          if (state[1] != 0)
          {
               state[0] = STATE0_FUNCERR;
               break;
          }

          if (state[0] == STATE0_UNKNOWFUNC)
               break;
     }

     g_strfreev (commands);

     if (msg == NULL)
     {
          char *tmp;

          switch (state[0])
          {
               case STATE0_NOERR: tmp = "Success."; break;
               case STATE0_PARSEERR: tmp = "Parse error."; break;
               case STATE0_UNKNOWFUNC: tmp = "Unknow command."; break;
               case STATE0_FUNCERR:
                    switch (state[1])
                    {
                         case STATE1_ARGERR: tmp = "Too few arguments"; break;
                         case STATE1_UNDEF: tmp = "Function not yet implemented"; break;
                         case STATE1_SPEC:
                              if (state[2] == STATE2_UNKNOWERR)
                                   tmp = "Unknow error.";
                              break;
                    }
          }

          msg = g_string_new (tmp);
     }

     if (ret != NULL)
          *ret = g_string_new (g_strdup_printf ("%d%d%d: %s", state[0], state[1], state[2], msg->str));
}

/*!
  \fn char handle_spawn (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'spawn'

  Specific error code:
  - 2 : Can't execute file
 */
char handle_spawn (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     /* command <pid> <config path> <socket path> [args...] */
     GError *error = NULL;
     gchar **child_argv;
     GPid child;
     int i, j;

     if (argc < 2)
          return STATE1_ARGERR;

     child_argv = calloc (3 + argc, sizeof (char *));

     child_argv[0] = g_strdup (argv[1]);
     child_argv[1] = g_strdup_printf ("%d", getpid ());
     child_argv[2] = global.cmdline.config;
     child_argv[3] = global.unix_sock.path;

     for (i = 4, j = 2; j < argc; ++i, ++j)
     {
          child_argv[i] = g_strdup (argv[j]);
     }
     child_argv[i] = NULL;

     if (!g_spawn_async (NULL, child_argv, NULL, 0, NULL, NULL, &child, &error) || error != NULL)
     {
          set_msg (msg, strip_nl (error->message));
          g_error_free (error);

          *ret = 2;
          return STATE1_SPEC;
     }

     set_msg (msg, "spawn: child process <%d>", getpid ());

     return STATE1_NOERR;
}

/*!
  \fn char handle_download (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'download'
 */
char handle_download (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     return STATE1_UNDEF;
}

/*!
  \fn char handle_open (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'open'
 */
char handle_open (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     if (argc < 2)
          return STATE1_ARGERR;

     if (g_str_has_suffix (argv[1], "%s"))
     {
          if (g_str_equal (argv[1], "%s"))
               echo (obj, ":open ");
          else
          {
               argv[1][strlen (argv[1]) - 2] = 0;
               echo (obj, ":open %s", argv[1]);
          }
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@uri@"))
     {
          echo (obj, ":open %s", cream_tabbed_get_uri (obj));
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@home@"))
     {
          if (global.cfg.global.homepage != NULL)
               cream_tabbed_load_uri (obj, global.cfg.global.homepage);
          else
               cream_tabbed_load_uri (obj, "http://cream-browser.net");
          global.browser.mode = BindMode;
     }
     else
     {
          cream_tabbed_load_uri (obj, argv[1]);
          global.browser.mode = BindMode;
     }

     return STATE1_NOERR;
}

/*!
  \fn char handle_tabopen (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'tabopen'
 */
char handle_tabopen (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     if (argc < 2)
          return STATE1_ARGERR;

     if (g_str_has_suffix (argv[1], "%s"))
     {
          if (g_str_equal (argv[1], "%s"))
               echo (obj, ":tabopen ");
          else
          {
               argv[1][strlen (argv[1]) - 2] = 0;
               echo (obj, ":tabopen %s", argv[1]);
          }
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@uri@"))
     {
          echo (obj, ":tabopen %s", cream_tabbed_get_uri (obj));
          gtk_widget_grab_focus (obj->inputbox);
          gtk_entry_set_position (GTK_ENTRY (obj->inputbox), -1);
          global.browser.mode = CmdMode;
     }
     else if (g_str_equal (argv[1], "@home@"))
     {
          if (global.cfg.global.homepage != NULL)
               notebook_append_page (global.cfg.global.homepage);
          else
               notebook_append_page ("http://cream-browser.net");
          global.browser.mode = BindMode;
     }
     else
     {
          notebook_append_page (argv[1]);
          global.browser.mode = BindMode;
     }

     return STATE1_NOERR;
}

/*!
  \fn char handle_close (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'close'
 */
char handle_close (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     cb_cream_notebook_close_page (NULL, GTK_WIDGET (obj));
     return STATE1_NOERR;
}

/*!
  \fn char handle_yank (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'yank'
 */
char handle_yank (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     const gchar *uri = cream_tabbed_get_uri (obj);
     echo (obj, "Yanked %s", uri);
     gtk_clipboard_set_text (global.browser.clipboard, uri, -1);
     return STATE1_NOERR;
}

/*!
  \fn char handle_paste (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'paste'
 */
char handle_paste (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     gboolean tabopen = FALSE;
     gchar *uri = gtk_clipboard_wait_for_text (global.browser.clipboard);

     if (argc >= 2 && g_str_equal (argv[1], "tab"))
          tabopen = TRUE;

     if (tabopen)
          notebook_append_page (uri);
     else
          cream_tabbed_load_uri (obj, uri);

     return STATE1_NOERR;
}

/*!
  \fn char handle_buffers (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'buffers'
 */
char handle_buffers (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     GString *tmp = g_string_new ("");
     int i;

     for (i = 0; i < gtk_notebook_get_n_pages (global.notebook); ++i)
     {
          CreamTabbed *tab = CREAM_TABBED (gtk_notebook_get_nth_page (global.notebook, i));
          tmp = g_string_append (tmp, cream_tabbed_get_uri (tab));
          tmp = g_string_append (tmp, " ");
     }
     if (msg != NULL) *msg = tmp;

     return STATE1_NOERR;
}

/*!
  \fn char handle_set (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'set'

  Specific error code:
  - 2 : Can't set variable (read-only)
 */
char handle_set (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     if (argc < 3)
          return STATE1_ARGERR;

     if (!set (argv[1], argv[2])) /* read-only */
     {
          set_msg (msg, "Can't set '%s' to '%s' (read-only).", argv[1], argv[2]);

          *ret = 2;
          return STATE1_SPEC;
     }

     return STATE1_NOERR;
}

/*!
  \fn char handle_get (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'get'

  Specific error code:
  - 2 : Variable doesn't exist.
 */
char handle_get (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     char *tmp;

     if (argc < 2)
          return STATE1_ARGERR;

     tmp = get (argv[1]);

     if (tmp == NULL)
     {
          set_msg (msg, "Can't get '%s', variable doesn't exist.", argv[1]);

          *ret = 2;
          return STATE1_SPEC;
     }
     else
          set_msg (msg, tmp);

     return STATE1_NOERR;
}

/*!
  \fn char handle_bind (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'bind'
 */
char handle_bind (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     struct key_t *new, *tmp;

     if (argc < 3)
          return STATE1_ARGERR;

     new = malloc (sizeof (struct key_t));

     new->bind = g_strdup (argv[1]);
     new->cmd  = g_strdup (argv[2]);
     new->next = NULL;

     if (global.cfg.keys == NULL)
          global.cfg.keys = new;
     else
     {
          for (tmp = global.cfg.keys; tmp->next != NULL; tmp = tmp->next);
          tmp->next = new;
     }

     return STATE1_NOERR;
}

/*!
  \fn char handle_quit (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
  \brief Handler for command 'quit'
 */
char handle_quit (int argc, char **argv, char *ret, GString **msg, CreamTabbed *obj)
{
     cream_release (EXIT_SUCCESS);
     return STATE1_NOERR;
}

/*! @} */
