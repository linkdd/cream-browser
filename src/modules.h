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

#ifndef __MODULES_H
#define __MODULES_H

#include <gtk/gtk.h>
#include <gmodule.h>

#include "modules/scheme.h"

/*!
 * \defgroup modules Modules
 * Manage modules (and \class{GModule}).
 *
 * \todo Create modules using WebKit, FTP, and to manage <code>about:</code> pages.
 *
 * @{
 */

/*!
 * \fn typedef void (*CreamModuleInitFunc) (void)
 * Initialize the loaded module.
 */
typedef void (*CreamModuleInitFunc) (void);

/*!
 * \fn typedef void (*CreamModuleUnloadFunc) (void)
 * Free memory use by the module and unload it.
 */
typedef void (*CreamModuleUnloadFunc) (void);

/*!
 * \fn typedef GtkWidget * (*CreamModuleWebviewNewFunc) (void)
 * @return A newly allocated \class{GtkWidget}
 *
 * Create a new widget using the module.
 */
typedef GtkWidget * (*CreamModuleWebviewNewFunc) (void);

/*!
 * \fn typedef void (*CreamModuleCallFunc) (const char *, gpointer, ...)
 * @param id Function's name to call.
 * @param ret The return value.
 * @param ... A NULL ended list of arguments to pass to the function.
 *
 * Call a specific function from the module.
 */
typedef void (*CreamModuleCallFunc) (const char *, gpointer, ...);

/*!
 * \class CreamModule
 *
 * Object, based on \class{GModule}, which load external libraries to use
 * a specific protocol (ie: HTTP/HTTPS - WebKit).
 */
typedef struct CreamModule
{
     /*< private >*/
     GModule *module;                   /*!< \private \class{GModule} object */
     char *modulename;                  /*!< \private Module's name */
     guint module_id;                   /*!< \private Module's identifier */

     /*< public >*/

     CreamModuleInitFunc       init;         /*!< Init function */
     CreamModuleUnloadFunc     unload;       /*!< Unload function */
     CreamModuleWebviewNewFunc webview_new;  /*!< New webview function */
     CreamModuleCallFunc       call;         /*!< Call function */
} CreamModule;

gboolean modules_init (GError **err);
CreamModule *modules_load (const char *filename, GError **err);
void modules_unload (CreamModule *mod);

/*! @} */

#endif /* __MODULES_H */
