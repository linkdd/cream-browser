/*
 *  Copyright (c) 2010,  Marc Lagrange <rhaamo@gruik.at>
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
  @defgroup VTE VTE Terminal
  @ingroup modules
  @brief Implement a terminal into the browser

  @{
 */

#include "VTEModule.h"
#include <marshal.h>
#include <string.h>
#include "local.h"

extern struct global_t global;

enum
{
     URI_CHANGED_SIGNAL,
     NEW_TITLE_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     NEW_DOWNLOAD_SIGNAL,
     SWITCH_MODULE_SIGNAL,
     NB_SIGNALS
};

/*!
  \var static guint module_vte_signals[NB_SIGNALS]
  \brief Signal definitions
 */
static guint module_vte_signals[NB_SIGNALS] = { 0 };

static void module_vte_class_init (ModuleVteClass *class);
static void module_vte_init (ModuleVte *obj);
static void module_vte_cb_status_line_changed (ModuleVte *obj, gpointer data);
static void module_vte_cb_window_title_changed (ModuleVte *obj, gpointer data);
static void module_vte_cb_child_exited (ModuleVte *obj, gpointer data);

gchar *get_user_shell (void);

GtkType module_vte_get_type (void)
{
     static GtkType module_vte_type = 0;

     if (!module_vte_type)
     {
          static const GtkTypeInfo module_vte_info =
          {
               "ModuleVte",
               sizeof (ModuleVte),
               sizeof (ModuleVteClass),
               (GtkClassInitFunc) module_vte_class_init,
               (GtkObjectInitFunc) module_vte_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          module_vte_type = gtk_type_unique (VTE_TYPE_TERMINAL, &module_vte_info);
     }

     return module_vte_type;
}

static void module_vte_class_init (ModuleVteClass *class)
{
     module_vte_signals[URI_CHANGED_SIGNAL] = g_signal_new (
          "uri-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleVteClass, uri_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_vte_signals[NEW_TITLE_SIGNAL] = g_signal_new (
          "new-title",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleVteClass, new_title),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_vte_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
          "status-changed",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleVteClass, status_changed),
          NULL, NULL,
          g_cclosure_marshal_VOID__STRING,
          G_TYPE_NONE,
          1, G_TYPE_STRING);

     module_vte_signals[NEW_DOWNLOAD_SIGNAL] = g_signal_new (
          "new-download",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleVteClass, new_download),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__OBJECT,
          G_TYPE_BOOLEAN,
          1, G_TYPE_OBJECT);

     module_vte_signals[SWITCH_MODULE_SIGNAL] = g_signal_new (
          "switch-module",
          G_TYPE_FROM_CLASS (class),
          G_SIGNAL_RUN_LAST,
          G_STRUCT_OFFSET (ModuleVteClass, switch_module),
          NULL, NULL,
          g_cclosure_user_marshal_BOOLEAN__STRING,
          G_TYPE_BOOLEAN,
          1, G_TYPE_STRING);
}

static void module_vte_init (ModuleVte *obj)
{
     obj->uri = NULL;
     obj->title = NULL;
     obj->status = NULL;
     obj->view_source_mode = FALSE;
}

/*!
  \fn GtkWidget *module_vte_new (void)
  \brief Create a new ModuleVte object

  \return A new ModuleVte

  \todo Add some stuff in the configuration
 */
GtkWidget *module_vte_new (void)
{
     ModuleVte *obj = gtk_type_new (module_vte_get_type ());

     GdkColor fore, back, tint, highlight, cursor;
     gchar *font = NULL;
     gchar *termcap = NULL;

     if (global.cfg.vte.reverse == FALSE)
     {
	     back.red = back.green = back.blue = 0xFFFF;
	     fore.red = fore.green = fore.blue = 0x0000;
     }
     else
     {
	     back.red = back.green = back.blue = 0x0000;
          fore.red = fore.green = fore.blue = 0xFFFF;
     }

     highlight.red = highlight.green = highlight.blue = 0xc000;
     cursor.red = 0xFFFF;
     cursor.green = cursor.blue = 0x8000;
     tint.red = tint.green = tint.blue = 0;
     tint = back;

     if (global.cfg.vte.doublebuffer == FALSE)
          gtk_widget_set_double_buffered (GTK_WIDGET (obj), FALSE);

     if (global.cfg.vte.hints == TRUE)
     {
          ;
     }

     /* Set some defaults */
     vte_terminal_set_audible_bell (VTE_TERMINAL (obj), FALSE);
     vte_terminal_set_visible_bell (VTE_TERMINAL (obj), TRUE);
     vte_terminal_set_cursor_blink_mode (VTE_TERMINAL (obj), VTE_CURSOR_BLINK_SYSTEM);
     vte_terminal_set_scroll_background (VTE_TERMINAL (obj), TRUE);
     vte_terminal_set_scroll_on_output (VTE_TERMINAL (obj), FALSE);
     vte_terminal_set_scroll_on_keystroke (VTE_TERMINAL (obj), TRUE);
     vte_terminal_set_scrollback_lines (VTE_TERMINAL (obj), 500);
     vte_terminal_set_mouse_autohide (VTE_TERMINAL (obj), TRUE);
     vte_terminal_set_background_tint_color (VTE_TERMINAL (obj), &tint);
     vte_terminal_set_colors (VTE_TERMINAL (obj),  &fore, &back, NULL, 0);
     vte_terminal_set_color_highlight (VTE_TERMINAL (obj), &highlight);
     vte_terminal_set_color_cursor (VTE_TERMINAL (obj), &cursor);

     if (termcap != NULL)
          vte_terminal_set_emulation (VTE_TERMINAL (obj), termcap);

     if (font == NULL)
          vte_terminal_set_font_from_string (VTE_TERMINAL (obj), "monospace normal 8");
     else
          vte_terminal_set_font_from_string (VTE_TERMINAL (obj), font);

     /* connect signals */
     g_object_connect (G_OBJECT (obj),
          "signal::status-line-changed",  G_CALLBACK (module_vte_cb_status_line_changed),  NULL,
          "signal::window-title-changed", G_CALLBACK (module_vte_cb_window_title_changed), NULL,
          "signal::child-exited",         G_CALLBACK (module_vte_cb_child_exited),         NULL,
     NULL);

     return GTK_WIDGET (obj);
}

/*!
  \fn void module_vte_load_shell_with_uri (ModuleVte *obj, gchar *uri)
  \brief Load a shell program

  \param obj A #ModuleVte object
  \param uri Working directory of the shell
 */
void module_vte_load_shell_with_uri (ModuleVte *obj, gchar *uri)
{
     gchar *shell = get_user_shell ();
     gchar *path = NULL;

     if (!g_str_equal (uri, "vte://"))
          path = &uri[6];

     obj->child_pid = vte_terminal_fork_command (VTE_TERMINAL (obj), shell,
                                     NULL, NULL, path,
                                     TRUE, TRUE, TRUE);
}

/*!
  \fn gchar *get_user_shell (void)
  \brief Get the user's default shell

  \return Path to the shell's binary
 */
gchar *get_user_shell (void)
{
     gchar *shell = global.cfg.vte.shell;

     if (shell == NULL)
          shell = getenv ("SHELL");
     if (shell == NULL)
          shell = strdup ("/bin/sh");
     return shell;
}

/* Signals */
static void module_vte_cb_status_line_changed (ModuleVte *obj, gpointer data)
{
     if (obj->status != NULL)
          g_free (obj->status);
     obj->status = g_strdup (vte_terminal_get_status_line (VTE_TERMINAL (obj)));

     g_signal_emit (
          G_OBJECT (obj),
          module_vte_signals[STATUS_CHANGED_SIGNAL],
          0,
          obj->status
     );
}

static void module_vte_cb_window_title_changed (ModuleVte *obj, gpointer data)
{
     const gchar *tmp = vte_terminal_get_window_title (VTE_TERMINAL (obj));
     if (obj->title != NULL)
          g_free (obj->title);

     obj->title = g_strdup (tmp ? tmp : "Terminal");

     g_signal_emit (
         G_OBJECT (obj),
         module_vte_signals[NEW_TITLE_SIGNAL],
         0,
         obj->title
    );
}

static void module_vte_cb_child_exited (ModuleVte *obj, gpointer data)
{
     return;
}

/*! @} */
