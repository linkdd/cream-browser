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
 * \addtogroup plugins
 * @{
 */

static void cream_plugin_free_cmdlist (CreamPluginCommand *data)
{
     g_free (data->cmd);
     g_free (data);
}

static void cream_plugin_free_menulist (GtkMenuItem *data)
{
     gtk_widget_destroy (GTK_WIDGET (data));
}

G_DEFINE_TYPE (CreamPlugin, cream_plugin, G_TYPE_OBJECT)

CreamPlugin *cream_plugin_register (const gchar *name)
{
     CreamPlugin *plugin = g_object_new (CREAM_TYPE_PLUGIN, NULL);
     plugin->name = g_strdup (name);
     return plugin;
}

/*!
 * \fn static void cream_plugin_finalize (GObject *obj)
 * @param obj #CreamPlugin's object.
 * Free memory used by the object.
 */
static void cream_plugin_finalize (GObject *obj)
{
     CreamPlugin *self = CREAM_PLUGIN (obj);

     g_slist_free_full (self->commands, cream_plugin_free_cmdlist);
     g_slist_free_full (self->menuentries, cream_plugin_free_menulist);

     g_free (self->name);

     G_OBJECT_CLASS (cream_plugin_parent_class)->finalize (obj);
}

/*!
 * \fn static void cream_plugin_class_init (CreamPluginClass *klass)
 * @param klass #CreamPlugin's class structure.
 *
 * Initialize #CreamPlugin's class.
 */
static void cream_plugin_class_init (CreamPluginClass *klass)
{
     GObjectClass *gclass = G_OBJECT_CLASS (klass);

     gclass->finalize = cream_plugin_finalize;
}

/*!
 * \fn static void cream_plugin_init (CreamPlugin *self)
 * @param self #CreamPlugin's instance structure.
 *
 * Initialize #CreamPlugin's instance.
 */
static void cream_plugin_init (CreamPlugin *self)
{
     self->name        = NULL;
     self->commands    = NULL;
     self->menuentries = NULL;
     self->hooks       = NULL;
     self->creamicon   = NULL;
}

/*! @} */

/*!
 * \defgroup plugins-members Members
 * \ingroup plugins
 * @{
 */

void cream_plugin_add_command (CreamPlugin *obj, const gchar *command, GCallback cb, gpointer user_data)
{
     CreamPluginCommand *el;

     g_return_if_fail (CREAM_IS_PLUGIN (obj));

     el = g_new0 (CreamPluginCommand, 1);
     el->cmd  = g_strdup (command);
     el->cb   = cb;
     el->data = user_data;

     obj->commands = g_slist_append (obj->commands, el);
}

void cream_plugin_add_menuitem (CreamPlugin *obj, const gchar *lbl, GCallback cb, gpointer user_data)
{
     GtkWidget *menuitem = NULL;

     g_return_if_fail (CREAM_IS_PLUGIN (obj));

     menuitem = gtk_menu_item_new_with_label (lbl);
     g_signal_connect (G_OBJECT (menuitem), "activate", cb, user_data);

     obj->menuentries = g_slist_append (obj->menuentries, menuitem);
}

/*! @} */
