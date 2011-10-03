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

#ifndef __CREAM_BROWSER_H
#define __CREAM_BROWSER_H

/*!
 * \defgroup cream-browser Cream-Browser Application
 * @{
 */

#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include "lua.h"
#include "modules.h"
#include "keybinds.h"
#include "interface.h"
#include "theme.h"
#include "socket.h"
#include "cache.h"

G_BEGIN_DECLS

#define GAPP_TYPE_CREAM_BROWSER         (cream_browser_get_type ())
#define CREAM_BROWSER(obj)              (G_TYPE_CHECK_INSTANCE_CAST (obj, GAPP_TYPE_CREAM_BROWSER, CreamBrowser))
#define GAPP_IS_CREAM_BROWSER(obj)      (G_TYPE_CHECK_INSTANCE_TYPE (obj, GAPP_IS_CREAM_BROWSER))
#define CREAM_BROWSER_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST (klass, GAPP_TYPE_CREAM_BROWSER, CreamBrowserClass))
#define CREAM_BROWSER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS (obj, GAPP_TYPE_CREAM_BROWSER, CreamBrowserClass))

typedef struct _CreamBrowser CreamBrowser;
typedef struct _CreamBrowserClass CreamBrowserClass;
typedef struct _CreamBrowserPrivate CreamBrowserPrivate;

/*!
 * \class CreamBrowser
 * CreamBrowser application class
 */
struct _CreamBrowser
{
     GtkApplication parent;

     /*< private >*/
     gchar *url;
     gchar *config;
     gchar *sockpath;
     gchar *cmd;

     gchar *profile;

     gboolean log;
     gboolean version;
     gboolean checkconf;

     GApplicationCommandLine *gappcmdline;
     gboolean cmdline;

     struct
     {
          GtkWidget *window;
          GtkWidget *vimsplit;
          GtkWidget *statusbar;
          GtkWidget *inputbox;
          GtkWidget *box;

          GtkWidget *fwebview;
     } gui;

     /*< public >*/
     CreamMode mode;          /*!< Cream-Browser's state */
     FILE *flog;              /*!< Cream-Browser's log */
     GHashTable *protocols;   /*!< Cream-Browser's protocols */
     Socket *sock;            /*!< Socket server */
     Theme *theme;            /*!< Theme engine */
     lua_State *luavm;        /*!< Lua VM state */
};

struct _CreamBrowserClass
{
     GtkApplicationClass parent;

     void (*error) (CreamBrowser *, gboolean, GError *);
     void (*startup) (CreamBrowser *);
};

GType cream_browser_get_type (void);
CreamBrowser *cream_browser_new (void);

GtkWidget *cream_browser_get_focused_webview (CreamBrowser *self);
void cream_browser_set_focused_webview (CreamBrowser *self, GtkWidget *webview);

void cream_browser_add_protocol (CreamBrowser *self, const gchar *scheme, GObject *mod);
void cream_browser_del_protocol (CreamBrowser *self, GObject *mod);
GObject *cream_browser_get_protocol (CreamBrowser *self, const gchar *scheme);

void cream_browser_exit (CreamBrowser *self, int exit_code);

/*! @} */

#endif /* __CREAM_BROWSER_H */
