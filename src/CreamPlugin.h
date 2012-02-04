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

#ifndef __CREAM_PLUGIN_H
#define __CREAM_PLUGIN_H

/*!
 * \defgroup plugins Plugins
 * Plugins class definition.
 * @{
 */

#include <gtk/gtk.h>

typedef struct
{
     gchar *cmd;
     GCallback cb;
     gpointer data;
} CreamPluginCommand;

G_BEGIN_DECLS

#define CREAM_TYPE_PLUGIN          (cream_plugin_get_type ())
#define CREAM_PLUGIN(obj)          (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_PLUGIN, CreamPlugin))
#define CREAM_IS_PLUGIN(obj)       (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_PLUGIN))
#define CREAM_PLUGIN_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST (klass, CREAM_TYPE_PLUGIN, CreamPluginClass))

typedef struct _CreamPlugin CreamPlugin;
typedef struct _CreamPluginClass CreamPluginClass;

/*!
 * \class CreamPlugin
 * Manage plugins
 */
struct _CreamPlugin
{
     GObject parent;

     gchar *name;

     GSList *commands;
     GSList *menuentries;
     GList *hooks;

     GObject *creamicon;
};

struct _CreamPluginClass
{
     GObjectClass parent;
};

GType cream_plugin_get_type (void);

G_END_DECLS

/*! @} */

#endif /* __CREAM_PLUGIN_H */
