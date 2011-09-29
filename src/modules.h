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

#include "scheme.h"

/*!
 * \defgroup modules Modules
 * Manage modules.
 *
 * \todo Create modules using WebKit, FTP, and to manage <code>about:</code> pages.
 *
 * @{
 */

G_BEGIN_DECLS

#define CREAM_TYPE_MODULE                    (cream_module_get_type ())
#define CREAM_MODULE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST (obj, CREAM_TYPE_MODULE, CreamModule))
#define CREAM_IS_MODULE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE (obj, CREAM_TYPE_MODULE))
#define CREAM_MODULE_GET_INTERFACE(inst)     (G_TYPE_INSTANCE_GET_INTERFACE (inst, CREAM_TYPE_MODULE, CreamModuleIface))

typedef struct _CreamModule CreamModule;
typedef struct _CreamModuleIface CreamModuleIface;

/*!
 * \class CreamModule
 * Virtual module class.
 */

/*!
 * \interface CreamModuleIface
 * #CreamModule interface
 */
struct _CreamModuleIface
{
     GTypeInterface parent;

     GtkWidget * (*webview_new) (CreamModule *self);
     void (*load_uri) (CreamModule *self, GtkWidget *, UriScheme *);
     void (*reload) (CreamModule *self, GtkWidget *);
     gboolean (*can_go_back) (CreamModule *self, GtkWidget *);
     void (*backward) (CreamModule *self, GtkWidget *);
     gboolean (*can_go_forward) (CreamModule *self, GtkWidget *);
     void (*forward) (CreamModule *self, GtkWidget *);
     gboolean (*search) (CreamModule *self, GtkWidget *, const gchar *, gboolean);
     void (*proxy) (CreamModule *self, const gchar *);
     void (*useragent) (CreamModule *self, const gchar *);
     void (*load_favicon) (CreamModule *self, GtkWidget *);
};

GType cream_module_get_type (void);
void modules_init (void);

GtkWidget *cream_module_webview_new (CreamModule *self);
void cream_module_load_uri (CreamModule *self, GtkWidget *webview, UriScheme *uri);
void cream_module_reload (CreamModule *self, GtkWidget *webview);
gboolean cream_module_can_go_back (CreamModule *self, GtkWidget *webview);
void cream_module_backward (CreamModule *self, GtkWidget *webview);
gboolean cream_module_can_go_forward (CreamModule *self, GtkWidget *webview);
void cream_module_forward (CreamModule *self, GtkWidget *webview);
gboolean cream_module_search (CreamModule *self, GtkWidget *webview, const gchar *text, gboolean forward);
void cream_module_proxy (CreamModule *self, const gchar *uri);
void cream_module_useragent (CreamModule *self, const gchar *ua);

/*!
 * \def CREAM_DEFINE_MODULE (ctype, fn_prefix)
 * @param ctype Module's C type.
 * @param fn_prefix Prefix used in functions declarations.
 *
 * This macro is used to declare a new module.
 * It defines the GObject, implements the interface, declares all functions,
 * properties and signals.
 */
#define CREAM_DEFINE_MODULE(ctype,fn_prefix)                                                                            \
     static void fn_prefix##_interface_init (CreamModuleIface *iface);                                                  \
     static GtkWidget* fn_prefix##_webview_new (CreamModule *self);                                                     \
     static void fn_prefix##_load_uri (CreamModule *self, GtkWidget *webview, UriScheme *uri);                          \
     static void fn_prefix##_reload (CreamModule *self, GtkWidget *webview);                                            \
     static gboolean fn_prefix##_can_go_back (CreamModule *self, GtkWidget *webview);                                   \
     static void fn_prefix##_backward (CreamModule *self, GtkWidget *webview);                                          \
     static gboolean fn_prefix##_can_go_forward (CreamModule *self, GtkWidget *webview);                                \
     static void fn_prefix##_forward (CreamModule *self, GtkWidget *webview);                                           \
     static gboolean fn_prefix##_search (CreamModule *self, GtkWidget *webview, const gchar *text, gboolean forward);   \
     static void fn_prefix##_proxy (CreamModule *self, const gchar *uri);                                               \
     static void fn_prefix##_useragent (CreamModule *self, const gchar *ua);                                            \
                                                                                                                        \
     enum { PROP_0, PROP_NAME };                                                                                        \
     enum                                                                                                               \
     {                                                                                                                  \
          SIGNAL_URI_CHANGED,                                                                                           \
          SIGNAL_TITLE_CHANGED,                                                                                         \
          SIGNAL_FAVICON_CHANGED,                                                                                       \
          SIGNAL_PROGRESS_CHANGED,                                                                                      \
          SIGNAL_STATE_CHANGED,                                                                                         \
          SIGNAL_DOWNLOAD,                                                                                              \
          NB_SIGNAL                                                                                                     \
     };                                                                                                                 \
     static guint fn_prefix##_signals[NB_SIGNAL] = { 0 };                                                               \
                                                                                                                        \
     G_DEFINE_TYPE_WITH_CODE (ctype, fn_prefix, G_TYPE_OBJECT,                                                          \
                              G_IMPLEMENT_INTERFACE (CREAM_TYPE_MODULE,                                                 \
                                                     fn_prefix##_interface_init                                         \
                              )                                                                                         \
     );                                                                                                                 \
                                                                                                                        \
     static void fn_prefix##_set_property (GObject *object, guint propid, const GValue *value, GParamSpec *pspec)       \
     {                                                                                                                  \
          ctype *self = G_TYPE_CHECK_INSTANCE_CAST (object, fn_prefix##_get_type (), ctype);                            \
                                                                                                                        \
          switch (propid)                                                                                               \
          {                                                                                                             \
               case PROP_NAME:                                                                                          \
                    g_free (self->name);                                                                                \
                    self->name = g_value_dup_string (value);                                                            \
                    break;                                                                                              \
                                                                                                                        \
               default:                                                                                                 \
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);                                          \
                    break;                                                                                              \
          }                                                                                                             \
     }                                                                                                                  \
                                                                                                                        \
     static void fn_prefix##_get_property (GObject *object, guint propid, GValue *value, GParamSpec *pspec)             \
     {                                                                                                                  \
          ctype *self = G_TYPE_CHECK_INSTANCE_CAST (object, fn_prefix##_get_type (), ctype);                            \
                                                                                                                        \
          switch (propid)                                                                                               \
          {                                                                                                             \
               case PROP_NAME:                                                                                          \
                    g_value_set_string (value, self->name);                                                             \
                    break;                                                                                              \
                                                                                                                        \
               default:                                                                                                 \
                    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, propid, pspec);                                          \
                    break;                                                                                              \
          }                                                                                                             \
     }                                                                                                                  \
                                                                                                                        \
     static void fn_prefix##_interface_init (CreamModuleIface *iface)                                                   \
     {                                                                                                                  \
          iface->webview_new    = fn_prefix##_webview_new;                                                              \
          iface->load_uri       = fn_prefix##_load_uri;                                                                 \
          iface->reload         = fn_prefix##_reload;                                                                   \
          iface->can_go_back    = fn_prefix##_can_go_back;                                                              \
          iface->backward       = fn_prefix##_backward;                                                                 \
          iface->can_go_forward = fn_prefix##_can_go_forward;                                                           \
          iface->forward        = fn_prefix##_forward;                                                                  \
          iface->search         = fn_prefix##_search;                                                                   \
          iface->proxy          = fn_prefix##_proxy;                                                                    \
          iface->useragent      = fn_prefix##_useragent;                                                                \
     }                                                                                                                  \
                                                                                                                        \
     static void fn_prefix##_class_init (ctype##Class *klass)                                                           \
     {                                                                                                                  \
          GObjectClass *gclass = G_OBJECT_CLASS (klass);                                                                \
                                                                                                                        \
          gclass->set_property = fn_prefix##_set_property;                                                              \
          gclass->get_property = fn_prefix##_get_property;                                                              \
                                                                                                                        \
          g_object_class_override_property (gclass, PROP_NAME, "name");                                                 \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_URI_CHANGED] = g_signal_new (                                                      \
                    "uri-changed",                                                                                      \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,                                                               \
                    G_STRUCT_OFFSET (ctype##Class, uri_changed),                                                        \
                    NULL, NULL,                                                                                         \
                    cream_marshal_VOID__OBJECT_STRING,                                                                  \
                    G_TYPE_NONE,                                                                                        \
                    2, G_TYPE_OBJECT, G_TYPE_STRING);                                                                   \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_TITLE_CHANGED] = g_signal_new (                                                    \
                    "title-changed",                                                                                    \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,                                                               \
                    G_STRUCT_OFFSET (ctype##Class, title_changed),                                                      \
                    NULL, NULL,                                                                                         \
                    cream_marshal_VOID__OBJECT_STRING,                                                                  \
                    G_TYPE_NONE,                                                                                        \
                    2, G_TYPE_OBJECT, G_TYPE_STRING);                                                                   \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_FAVICON_CHANGED] = g_signal_new (                                                  \
                    "favicon-changed",                                                                                  \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,                                                               \
                    G_STRUCT_OFFSET (ctype##Class, favicon_changed),                                                    \
                    NULL, NULL,                                                                                         \
                    cream_marshal_VOID__OBJECT_OBJECT,                                                                  \
                    G_TYPE_NONE,                                                                                        \
                    2, G_TYPE_OBJECT, GDK_TYPE_PIXBUF);                                                                 \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_PROGRESS_CHANGED] = g_signal_new (                                                 \
                    "progress-changed",                                                                                 \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,                                                               \
                    G_STRUCT_OFFSET (ctype##Class, progress_changed),                                                   \
                    NULL, NULL,                                                                                         \
                    cream_marshal_VOID__OBJECT_DOUBLE,                                                                  \
                    G_TYPE_NONE,                                                                                        \
                    2, G_TYPE_OBJECT, G_TYPE_DOUBLE);                                                                   \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_STATE_CHANGED] = g_signal_new (                                                    \
                    "state-changed",                                                                                    \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,                                                                \
                    G_STRUCT_OFFSET (ctype##Class, state_changed),                                                      \
                    NULL, NULL,                                                                                         \
                    cream_marshal_VOID__OBJECT_INT,                                                                     \
                    G_TYPE_NONE,                                                                                        \
                    2, G_TYPE_OBJECT, G_TYPE_INT);                                                                      \
                                                                                                                        \
          fn_prefix##_signals[SIGNAL_DOWNLOAD] = g_signal_new (                                                         \
                    "download",                                                                                         \
                    G_TYPE_FROM_CLASS (klass),                                                                          \
                    G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,                                                                \
                    G_STRUCT_OFFSET (ctype##Class, download),                                                           \
                    NULL, NULL,                                                                                         \
                    cream_marshal_BOOLEAN__OBJECT_STRING,                                                               \
                    G_TYPE_BOOLEAN,                                                                                     \
                    2, G_TYPE_OBJECT, G_TYPE_STRING);                                                                   \
     }

G_END_DECLS

/*! @} */

/*!
 * \defgroup mod-signals Signals
 * \ingroup modules
 *
 * \b Signals:
 * - \ref mod-uri-changed
 * - \ref mod-title-changed
 * - \ref mod-favicon-changed
 * - \ref mod-progress-changed
 * - \ref mod-state-changed
 * - \ref mod-download
 *
 * <hr>
 *
 * \section mod-uri-changed "uri-changed"
 * \code void uri_changed (CreamModule *self, GtkWidget *webview, const gchar *uri, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param uri The new URI.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when a webview, owned by the #CreamModule object, loads a new URI.
 *
 * <hr>
 *
 * \section mod-title-changed "title-changed"
 * \code void title_changed (CreamModule *self, GtkWidget *webview, const gchar *title, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param title The new title.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when a webview, owned by the #CreamModule object, got a new title.
 *
 * <hr>
 *
 * \section mod-favicon-changed "favicon-changed"
 * \code void favicon_changed (CreamModule *self, GtkWidget *webview, GdkPixbuf *favicon, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param favicon The favicon's \class{GdkPixbuf}.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when the webview's favicon is loaded.
 *
 * <hr>
 *
 * \section mod-progress-changed "progress-changed"
 * \code void progress_changed (CreamModule *self, GtkWidget *webview, gdouble progress, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param progress The page's load.
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when a webview, owned by the #CreamModule object, is loading a page.
 *
 * <hr>
 *
 * \section mod-state-changed "state-changed"
 * \code void state_changed (CreamModule *self, GtkWidget *webview, CreamMode state, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param state Browser's state (see #CreamMode).
 * @param user_data Data to pass to the handler.
 *
 * This signal is emitted when a webview, owned by the #CreamModule object, try to change the
 * borwser's state.
 *
 * <hr>
 *
 * \section mod-download "download"
 * \code gboolean download (CreamModule *self, GtkWidget *webview, const gchar *file_uri, gpointer user_data) \endcode
 * @param self A #CreamModule object.
 * @param webview A webview owned by the #CreamModule object.
 * @param file_uri The file's URI.
 * @param user_data Data to pass to the handler.
 * @return \c TRUE if the signal was handled (will stop all other handlers).
 *
 * This signal is emitted when a webview, owned by the #CreamModule object, requests to
 * download a file.
 */

#endif /* __MODULES_H */
