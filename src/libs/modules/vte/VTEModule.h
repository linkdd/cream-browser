/*
 *  Copyright (c) 2010, Marc Lagrange <rhaamo@gruik.at>
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

#ifndef __CLASS_VTE_MODULE_H
#define __CLASS_VTE_MODULE_H

/*!
  @addtogroup VTE
  @{
 */

#include <gtk/gtk.h>
#include <gtk/gtkwidget.h>
#include <vte/vte.h>

G_BEGIN_DECLS

#define MODULE_VTE(obj)            \
     GTK_CHECK_CAST (obj, module_vte_get_type (), ModuleVte)
#define MODULE_VTE_CLASS(klass)    \
     GTK_CHECK_CLASS_CAST (obj, module_vte_get_type (), ModuleVte)
#define MODULE_IS_VTE(obj)         \
     GTK_CHECK_TYPE (obj, module_vte_get_type ())

/*! \typedef struct _ModuleVte ModuleVte */
/*! \typedef struct _ModuleVteClass ModuleVteClass */

typedef struct _ModuleVte ModuleVte;
typedef struct _ModuleVteClass ModuleVteClass;

/*! \struct _ModuleVte */
struct _ModuleVte
{
     VteTerminal parent;           /*!< Parent instance */
     pid_t child_pid;              /*!< PID of the child program */

     gchar *uri;                   /*!< Working directory (unused) */
     gchar *title;                 /*!< Program title */
     gchar *status;                /*!< Status line from VTE Terminal */

     gboolean view_source_mode;    /*!< Unused */
};

/*! \struct _ModuleVteClass */
struct _ModuleVteClass
{
     VteTerminalClass parent_class;     /*!< Parent class */

     void (*uri_changed) (ModuleVte *obj, gchar *uri); /*!< Unused */
     void (*new_title) (ModuleVte *obj, gchar *title);
     void (*status_changed) (ModuleVte *obj, gchar *status);
     gboolean (*new_download) (ModuleVte *obj, GObject *download); /*!< Unused */
     gboolean (*switch_module) (ModuleVte *obj, gchar *new_uri);
};

GtkType module_vte_get_type (void);
GtkWidget *module_vte_new (void);
void module_vte_load_shell_with_uri (ModuleVte *obj, gchar *uri);

G_END_DECLS

/*! @} */

#endif /* __CLASS_VTE_MODULE_H */
