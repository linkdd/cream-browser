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


#include "VTEModule.h"
#include <marshal.h>
#include <string.h>
#include "local.h"

enum
{
     SWITCH_MODULE_SIGNAL,
     NB_SIGNALS
};

static guint module_vte_signals[NB_SIGNALS] = { 0 };

/* Private data */

#define MODULE_VTE_GET_PRIVATE(obj)     (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
                                         module_vte_get_type (),                \
                                         ModuleVtePrivate))

typedef struct _ModuleVtePrivate ModuleVtePrivate;

/*! \struct _ModuleVtePrivate */
struct _ModuleVtePrivate
{
     GtkWidget *vte; /* Or GtkWidget ! */
};

/* End of private data */

static void module_vte_class_init (ModuleVteClass *class);
static void module_vte_init (ModuleVte *obj);
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
     g_type_class_add_private (class, sizeof (ModuleVtePrivate));

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

GtkWidget *module_vte_new (void)
{
     ModuleVte *obj = gtk_type_new (module_vte_get_type ());
     ModuleVtePrivate *priv = MODULE_VTE_GET_PRIVATE (obj);
     GdkColor fore, back, tint, highlight, cursor;
     const char *font = NULL;
     const char *termcap = NULL;
     /* Init VTE Terminal */
     priv->vte = vte_terminal_new ();

     if (global.cfg.vte.reverse == FALSE) {
	 back.red = back.green = back.blue = 0xffff;
	 fore.red = fore.green = fore.blue = 0x0000;
     } else {
	 back.red = back.green = back.blue = 0x0000;
	 fore.red = fore.green = fore.blue = 0xffff;
     }
     highlight.red = highlight.green = highlight.blue = 0xc000;
     cursor.red = 0xffff;
     cursor.green = cursor.blue = 0x8000;
     tint.red = tint.green = tint.blue = 0;
     tint = back;

     if (global.cfg.vte.doublebuffer == FALSE)
	 gtk_widget_set_double_buffered(priv->vte, FALSE);

     if (global.cfg.vte.hints == TRUE) {

     }

     /* Set some defaults */
     /* TODO: put some in config */
     vte_terminal_set_audible_bell(VTE_TERMINAL (priv->vte), FALSE);
     vte_terminal_set_visible_bell(VTE_TERMINAL (priv->vte), TRUE);
     vte_terminal_set_cursor_blink_mode(VTE_TERMINAL (priv->vte), VTE_CURSOR_BLINK_SYSTEM);
     vte_terminal_set_scroll_background(VTE_TERMINAL (priv->vte), TRUE);
     vte_terminal_set_scroll_on_output(VTE_TERMINAL (priv->vte), FALSE);
     vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL (priv->vte), TRUE);
     vte_terminal_set_scrollback_lines(VTE_TERMINAL (priv->vte), 500);
     vte_terminal_set_mouse_autohide(VTE_TERMINAL (priv->vte), TRUE);
     vte_terminal_set_background_tint_color(VTE_TERMINAL (priv->vte), &tint);
     vte_terminal_set_colors(VTE_TERMINAL (priv->vte),  &fore, &back, NULL, 0);
     vte_terminal_set_color_highlight(VTE_TERMINAL (priv->vte), &highlight);
     vte_terminal_set_color_cursor(VTE_TERMINAL (priv->vte), &cursor);
     /* antialias/termcap */
     /* XXX TODO: use config settings */
     if (termcap != NULL)
     	vte_terminal_set_emulation(VTE_TERMINAL (priv->vte), termcap);
     vte_terminal_set_font_from_string_full(VTE_TERMINAL (priv->vte), font, VTE_ANTI_ALIAS_USE_DEFAULT);



     return GTK_WIDGET (obj);
}

int module_vte_load_shell_with_uri (ModuleVte *obj, gchar *uri)
{
    /* TODO Add a gboolean exec to choice exec'ing a program or shell
     * TODO Add env in this function for exec'ing
     */
     ModuleVtePrivate *priv = MODULE_VTE_GET_PRIVATE (obj);
     gchar *shell;
     int pid_shell;
     shell = get_user_shell ();

     /* Execute shell in our new VTE */
     pid_shell = vte_terminal_fork_command(
	     VTE_TERMINAL (priv->vte),		/* VTE Terminal */
	     shell,		/* command */
	     NULL,		/* argv */
	     NULL,		/* envv */
	     uri,		/* directory to start */
	     TRUE,		/* logged in lastlog ? */
	     TRUE,		/* logged in utmp/utmpx ? */
	     TRUE		/* logged in wtmp/wtmpx ? */
	     );

     return pid_shell;
}

gchar *get_user_shell (void)
{
    gchar *shell;
    shell = global.cfg.vte.shell;
    if (shell == NULL)
	shell = getenv("SHELL");
	if (shell == NULL)
	    shell = strdup ("/bin/sh");
    return shell;
}
