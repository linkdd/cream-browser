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
 * \addtogroup modules
 *
 * @{
 */

/*!
 * @param g_class \class{GObject} class.
 * Initialize interface.
 */
static void cream_module_base_init (gpointer g_class)
{
     static gboolean is_initialized = FALSE;

     if (!is_initialized)
     {
          GParamSpec *pspec;

          pspec = g_param_spec_string ("name", "Name", "Module's name", "(none)", G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);
          g_object_interface_install_property (g_class, pspec);

          is_initialized = TRUE;
     }
}

/*! @return The interface's \class{GType} */
GType cream_module_get_type (void)
{
     static GType iface_type = 0;

     if (iface_type == 0)
     {
          static const GTypeInfo info =
          {
               sizeof (CreamModuleIface),
               cream_module_base_init,
               NULL
          };

          iface_type = g_type_register_static (G_TYPE_INTERFACE, "CreamModule", &info, 0);
     }

     return iface_type;
}

/*! Initialize modules. */
void modules_init (void)
{
     GObject *mod = NULL;

#ifdef HAVE_MOD_DUMMY
     mod = g_object_new (CREAM_MODULE_TYPE_DUMMY, "name", "dummy", NULL);
#endif
#ifdef HAVE_MOD_WEBKIT
     mod = g_object_new (CREAM_MODULE_TYPE_WEBKIT, "name", "webkit", NULL);
#endif

     (void) mod;
}

/*! @} */

/*!
 * \defgroup mod-members Members
 * \ingroup modules
 * @{
 */

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @return A new widget to be used in #WebView.
 */
GtkWidget *cream_module_webview_new (CreamModule *self)
{
     CreamModuleIface *iface;
     g_return_val_if_fail (CREAM_IS_MODULE (self), NULL);
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_val_if_fail (iface->webview_new != NULL, NULL);
     return iface->webview_new (self);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 * @param uri URI to load, see #UriScheme.
 */
void cream_module_load_uri (CreamModule *self, GtkWidget *webview, UriScheme *uri)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->load_uri != NULL);
     iface->load_uri (self, webview, uri);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 */
void cream_module_reload (CreamModule *self, GtkWidget *webview)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->reload != NULL);
     iface->reload (self, webview);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 */
gboolean cream_module_can_go_back (CreamModule *self, GtkWidget *webview)
{
     CreamModuleIface *iface;
     g_return_val_if_fail (CREAM_IS_MODULE (self), FALSE);
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_val_if_fail (iface->can_go_back != NULL, FALSE);
     return iface->can_go_back (self, webview);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 */
void cream_module_backward (CreamModule *self, GtkWidget *webview)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->backward != NULL);
     iface->backward (self, webview);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 */
gboolean cream_module_can_go_forward (CreamModule *self, GtkWidget *webview)
{
     CreamModuleIface *iface;
     g_return_val_if_fail (CREAM_IS_MODULE (self), FALSE);
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_val_if_fail (iface->can_go_forward != NULL, FALSE);
     return iface->can_go_forward (self, webview);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 */
void cream_module_forward (CreamModule *self, GtkWidget *webview)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->forward != NULL);
     iface->forward (self, webview);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param webview A webview.
 * @param text Text to look for.
 * @param forward Search forward or backward.
 * @return \c TRUE on success, \c FALSE otherwise.
 *
 * Looks for a specified string inside webview.
 */
gboolean cream_module_search (CreamModule *self, GtkWidget *webview, const gchar *text, gboolean forward)
{
     CreamModuleIface *iface;
     g_return_val_if_fail (CREAM_IS_MODULE (self), FALSE);
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_val_if_fail (iface->search != NULL, FALSE);
     return iface->search (self, webview, text, forward);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param uri Proxy URL.
 *
 * Set uri as a proxy in the module.
 */
void cream_module_proxy (CreamModule *self, const gchar *uri)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->proxy != NULL);
     iface->proxy (self, uri);
}

/*!
 * \public \memberof CreamModule
 * @param self The module to use.
 * @param ua Useragent.
 *
 * Set the useragent.
 */
void cream_module_useragent (CreamModule *self, const gchar *ua)
{
     CreamModuleIface *iface;
     g_return_if_fail (CREAM_IS_MODULE (self));
     iface = CREAM_MODULE_GET_INTERFACE (self);
     g_return_if_fail (iface->useragent != NULL);
     iface->useragent (self, ua);
}

/*! @} */
