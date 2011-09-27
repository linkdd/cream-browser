/*
* Copyright © 2011, David Delassus <david.jose.delassus@gmail.com>
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

#include "local.h"

/*!
 * \defgroup command Commands
 * Commands definition.
 *
 * @{
 */

static gboolean command_exit (gint argc, gchar **argv, GError **err);
static gboolean command_open (gint argc, gchar **argv, GError **err);
static gboolean command_tabopen (gint argc, gchar **argv, GError **err);
static gboolean command_tabclose (gint argc, gchar **argv, GError **err);
static gboolean command_split (gint argc, gchar **argv, GError **err);
static gboolean command_vsplit (gint argc, gchar **argv, GError **err);
static gboolean command_close (gint argc, gchar **argv, GError **err);

#define CREAM_COMMAND_ERROR        (cream_command_error_quark ())

typedef enum
{
     CREAM_COMMAND_ERROR_ARGS,
     CREAM_COMMAND_ERROR_NOT_IMPLEMENTED,
     CREAM_COMMAND_ERROR_UNKNOW_CMD,
     CREAM_COMMAND_ERROR_FAILED
} CreamCommandError;

static GQuark cream_command_error_quark (void)
{
     static GQuark domain = 0;

     if (!domain)
          domain = g_quark_from_string ("cream.command");

     return domain;
}

/*!
 * \struct command_t
 * Internal structure to define a command.
 */
struct command_t
{
     char *cmd;  /*!< Command's name */
     char *desc; /*!< Description */

     gboolean (*func) (gint, gchar **, GError **); /*!< Callback function */
};

static struct command_t internal_commands[] =
{
     { "exit",      gettext_noop ("Exit Cream-Browser"),                   command_exit },
     { "open",      gettext_noop ("Open URL"),                             command_open },
     { "tabopen",   gettext_noop ("Open URL in a new tab"),                command_tabopen },
     { "tabclose",  gettext_noop ("Close the current tab"),                command_tabclose },
     { "split",     gettext_noop ("Split the current view"),               command_split },
     { "vsplit",    gettext_noop ("Split the current view vertically"),    command_vsplit },
     { "close",     gettext_noop ("Close the current view"),               command_close },
     { NULL, NULL, NULL }
};

/*!
 * \fn gboolean run_command (const char *cmd, GError **err)
 * @param cmd Command to execute
 * @param err \class{GError} pointer in order to follow possible errors.
 * @return <code>TRUE</code> on success, <code>FALSE</code> otherwise.
 *
 * Execute an internal command.
 */

gboolean run_command (const char *cmd, GError **err)
{
     gchar **argv;
     gint argc, i;

     if (!g_shell_parse_argv (cmd, &argc, &argv, err))
          return FALSE;

     for (i = 0; internal_commands[i].cmd != NULL; ++i)
     {
          if (g_str_equal (internal_commands[i].cmd, argv[0]))
          {
               if (internal_commands[i].func != NULL)
                    return internal_commands[i].func (argc, argv, err);

               g_set_error (err, CREAM_COMMAND_ERROR, CREAM_COMMAND_ERROR_NOT_IMPLEMENTED, _("%s isn't implemented"), argv[0]);
               return FALSE;
          }
     }

     g_set_error (err, CREAM_COMMAND_ERROR, CREAM_COMMAND_ERROR_UNKNOW_CMD, _("Unknow command '%s'"), argv[0]);
     return FALSE;
}

/* callbacks */

static gboolean command_exit (gint argc, gchar **argv, GError **err)
{
     cream_browser_exit (app, EXIT_SUCCESS);
     return FALSE;
}

static gboolean command_open (gint argc, gchar **argv, GError **err)
{
     GtkWidget *notebook;
     GtkWidget *webview;

     if (argc != 2)
     {
          g_set_error (err, CREAM_COMMAND_ERROR, CREAM_COMMAND_ERROR_ARGS, _("open: Too few arguments"));
          return FALSE;
     }

     notebook = gtk_vim_split_get_focus (GTK_VIM_SPLIT (app->gui.vimsplit));

     if (notebook == NULL)
          return command_split (argc, argv, err);

     webview = notebook_get_focus (CREAM_NOTEBOOK (notebook));
     webview_load_uri (CREAM_WEBVIEW (webview), argv[1]);

     return TRUE;
}

static gboolean command_tabopen (gint argc, gchar **argv, GError **err)
{
     GtkWidget *notebook;
     int i;

     if (argc < 2)
     {
          g_set_error (err, CREAM_COMMAND_ERROR, CREAM_COMMAND_ERROR_ARGS, _("tabopen: Too few arguments"));
          return FALSE;
     }

     notebook = gtk_vim_split_get_focus (GTK_VIM_SPLIT (app->gui.vimsplit));

     for (i = 1; i < argc; ++i)
          notebook_tabopen (CREAM_NOTEBOOK (notebook), argv[i]);

     ui_show ();
     return TRUE;
}

static gboolean command_tabclose (gint argc, gchar **argv, GError **err)
{
     GtkWidget *notebook = gtk_vim_split_get_focus (GTK_VIM_SPLIT (app->gui.vimsplit));
     gint page = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));

     if (argc == 1)
          notebook_close (CREAM_NOTEBOOK (notebook), page);
     else
     {
          int i;

          for (i = 1; i < argc; ++i)
          {
               page = g_ascii_strtoll (argv[i], NULL, 10);
               notebook_close (CREAM_NOTEBOOK (notebook), page-1);
          }
     }

     if (0 == gtk_notebook_get_n_pages (GTK_NOTEBOOK (notebook)))
          gtk_vim_split_close (GTK_VIM_SPLIT (app->gui.vimsplit));

     ui_show ();
     return TRUE;
}

static gboolean command_split (gint argc, gchar **argv, GError **err)
{
     GtkWidget *nb = notebook_new ();

     if (argc == 1)
     {
          notebook_tabopen (CREAM_NOTEBOOK (nb), "dummy://user:pass@domain.com/path/to/index.html#anchor");
     }
     else if (argc >= 1)
     {
          int i;

          for (i = 1; i < argc; ++i)
               notebook_tabopen (CREAM_NOTEBOOK (nb), argv[i]);
     }

     gtk_vim_split_add (GTK_VIM_SPLIT (app->gui.vimsplit), nb, GTK_ORIENTATION_VERTICAL);
     ui_show ();
     return TRUE;
}

static gboolean command_vsplit (gint argc, gchar **argv, GError **err)
{
     GtkWidget *nb = notebook_new ();

     if (argc == 1)
     {
          notebook_tabopen (CREAM_NOTEBOOK (nb), "dummy://user:pass@domain.com/path/to/index.html#anchor");
     }
     else if (argc >= 1)
     {
          int i;

          for (i = 1; i < argc; ++i)
               notebook_tabopen (CREAM_NOTEBOOK (nb), argv[i]);
     }

     gtk_vim_split_add (GTK_VIM_SPLIT (app->gui.vimsplit), nb, GTK_ORIENTATION_HORIZONTAL);
     ui_show ();
     return TRUE;
}

static gboolean command_close (gint argc, gchar **argv, GError **err)
{
     gtk_vim_split_close (GTK_VIM_SPLIT (app->gui.vimsplit));
     ui_show ();
     return TRUE;
}

/*! @} */
