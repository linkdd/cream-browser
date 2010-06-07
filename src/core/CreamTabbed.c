/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
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
  @defgroup CreamTabbed Tabbed CreamView
  @brief Object which implement a #CreamView, a statusbar and an inputbox

  @{
 */

#include "local.h"

enum
{
     UPDATE_NOTEBOOK_TITLE_SIGNAL,
     NB_SIGNALS
};

/*!
  \var static guint cream_tabbed_signals[NB_SIGNALS]
  \brief Signal definitions
 */
static guint cream_tabbed_signals[NB_SIGNALS] = { 0 };

static void cream_tabbed_class_init (CreamTabbedClass *class);
static void cream_tabbed_init (CreamTabbed *obj);

static void cream_tabbed_init_statusbar (CreamTabbed *obj);
static void cream_tabbed_init_inputbox (CreamTabbed *obj);

static void cream_tabbed_uri_changed_cb (CreamView *view, gchar *uri, CreamTabbed *obj);
static void cream_tabbed_new_title_cb (CreamView *view, gchar *title, CreamTabbed *obj);
static void cream_tabbed_status_changed_cb (CreamView *view, gchar *status, CreamTabbed *obj);
static gboolean cream_tabbed_new_window_cb (CreamView *view, const gchar *uri, gpointer data);
static void cream_tabbed_scroll_cb (GtkAdjustment *vadjust, CreamTabbed *obj);

GtkType cream_tabbed_get_type (void)
{
     static GtkType cream_tabbed_type = 0;

     if (!cream_tabbed_type)
     {
          static const GtkTypeInfo cream_tabbed_info =
          {
               "CreamTabbed",
               sizeof (CreamTabbed),
               sizeof (CreamTabbedClass),
               (GtkClassInitFunc) cream_tabbed_class_init,
               (GtkObjectInitFunc) cream_tabbed_init,
               NULL, NULL,
               (GtkClassInitFunc) NULL
          };

          cream_tabbed_type = gtk_type_unique (gtk_vbox_get_type (), &cream_tabbed_info);
     }

     return cream_tabbed_type;
}

static void cream_tabbed_class_init (CreamTabbedClass *class)
{
     cream_tabbed_signals[UPDATE_NOTEBOOK_TITLE_SIGNAL] = g_signal_new ("update-notebook-title",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CreamTabbedClass, update_notebook_title),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);
}

static void cream_tabbed_init (CreamTabbed *obj)
{
     obj->creamview = NULL;

     obj->statusbar.eventbox = NULL;
     obj->statusbar.box      = NULL;
     obj->statusbar.url      = NULL;
     obj->statusbar.state    = NULL;

     obj->inputbox = NULL;
     obj->adjust_v = NULL;
}

/*!
  \fn GtkWidget *cream_tabbed_new (void)
  \brief Create a new CreamTabbed object which can be added to the notebook
  \return A new CreamTabbed object
 */
GtkWidget *cream_tabbed_new (void)
{
     CreamTabbed *obj = gtk_type_new (cream_tabbed_get_type ());

     obj->creamview = cream_view_new ();
     obj->adjust_v = cream_view_get_vadjustment (CREAM_VIEW (obj->creamview));

     cream_tabbed_init_statusbar (obj);
     cream_tabbed_init_inputbox (obj);

     gtk_box_pack_end (GTK_BOX (obj), obj->inputbox, FALSE, FALSE, 0);
     gtk_box_pack_end (GTK_BOX (obj), obj->statusbar.eventbox, FALSE, FALSE, 0);
     gtk_box_pack_start (GTK_BOX (obj), obj->creamview, TRUE, TRUE, 0);

     g_signal_connect (G_OBJECT (obj->adjust_v),  "value-changed",  G_CALLBACK (cream_tabbed_scroll_cb),         obj);
     g_object_connect (G_OBJECT (obj->creamview),
          "signal::uri-changed",     G_CALLBACK (cream_tabbed_uri_changed_cb),    obj,
          "signal::new-title",       G_CALLBACK (cream_tabbed_new_title_cb),      obj,
          "signal::status-changed",  G_CALLBACK (cream_tabbed_status_changed_cb), obj,
          "signal::new-window",      G_CALLBACK (cream_tabbed_new_window_cb),     NULL,
     NULL);

     cream_tabbed_scroll_cb (obj->adjust_v, obj);

     return GTK_WIDGET (obj);
}

static void cream_tabbed_init_statusbar (CreamTabbed *obj)
{
     GdkColor bg, fg;

     obj->statusbar.eventbox = gtk_event_box_new ();
     obj->statusbar.box      = gtk_hbox_new (FALSE, 0);
     obj->statusbar.url      = gtk_label_new (NULL);
     obj->statusbar.state    = gtk_label_new (NULL);

     /* change statusbar color */
     gdk_color_parse (global.cfg.gui.statusbar.bg.normal, &bg);
     gtk_widget_modify_bg (obj->statusbar.eventbox, GTK_STATE_NORMAL, &bg);

     gdk_color_parse (global.cfg.gui.statusbar.fg.normal, &fg);
     gtk_widget_modify_fg (obj->statusbar.url, GTK_STATE_NORMAL, &fg);
     gtk_widget_modify_fg (obj->statusbar.state, GTK_STATE_NORMAL, &fg);

     /* align the URL and the state */
     gtk_misc_set_alignment (GTK_MISC (obj->statusbar.url), 0.0, 0.0);
     gtk_misc_set_alignment (GTK_MISC (obj->statusbar.state), 1.0, 0.0);

     gtk_label_set_selectable (GTK_LABEL (obj->statusbar.url), TRUE);

     gtk_box_pack_start (GTK_BOX (obj->statusbar.box), obj->statusbar.url, TRUE, TRUE, 2);
     gtk_box_pack_start (GTK_BOX (obj->statusbar.box), obj->statusbar.state, FALSE, FALSE, 2);

     gtk_container_add (GTK_CONTAINER (obj->statusbar.eventbox), obj->statusbar.box);
}

static void cream_tabbed_init_inputbox (CreamTabbed *obj)
{
     PangoFontDescription *font;

     obj->inputbox = gtk_entry_new ();

     /* Change inputbox style */
     /* font */
     font = pango_font_description_from_string (global.cfg.gui.inputbox.font);
     gtk_widget_modify_font (obj->inputbox, font);
     pango_font_description_free (font);

     /* border */
     gtk_entry_set_inner_border (GTK_ENTRY (obj->inputbox), NULL);
     gtk_entry_set_has_frame (GTK_ENTRY (obj->inputbox), FALSE);

     g_signal_connect (G_OBJECT (obj->inputbox), "activate",        G_CALLBACK (cb_inputbox),      obj);
     g_signal_connect (G_OBJECT (obj->inputbox), "key-press-event", G_CALLBACK (cb_inputbox_keys), obj);
}

/*!
  \fn void cream_tabbed_load_uri (CreamTabbed *obj, const gchar *uri)
  \brief Load a new URI in the CreamView

  \param obj The CreamTabbed object
  \param uri New URI to load
 */
void cream_tabbed_load_uri (CreamTabbed *obj, const gchar *uri)
{
     GtkWidget *content;

     cream_view_load_uri (CREAM_VIEW (obj->creamview), uri);

     content = cream_view_get_content (CREAM_VIEW (obj->creamview));
     if (MODULE_IS_WEB_VIEW (content))
     {
          WebKitWebSettings *settings = module_web_view_get_settings (MODULE_WEB_VIEW (content));

          /* setting up encoding */
          if (!g_str_equal (global.cfg.global.encoding, "default"))
          {
               g_object_set (settings, "default-encoding", global.cfg.global.encoding, NULL);
          }

          /* enable/disable Javascript */
          g_object_set (settings, "enable-scripts", global.cfg.global.javascript, NULL);
          /* set UserAgent */
          set_user_agent (settings, uri);

          /* Apply modifications */
          webkit_web_view_set_settings (WEBKIT_WEB_VIEW (content), settings);
     }
}

/*!
  \fn const gchar *cream_tabbed_get_uri (CreamTabbed *obj)
  \brief Get the loaded URI of the CreamView
 */
const gchar *cream_tabbed_get_uri (CreamTabbed *obj)
{
     return cream_view_get_uri (CREAM_VIEW (obj->creamview));
}

/*!
  \fn const gchar *cream_tabbed_get_title (CreamTabbed *obj)
  \brief Get the title of the loaded URI
 */
const gchar *cream_tabbed_get_title (CreamTabbed *obj)
{
     return cream_view_get_title (CREAM_VIEW (obj->creamview));
}

/*!
  \fn const gchar *cream_tabbed_get_status (CreamTabbed *obj)
  \brief Get the status message of the loaded URI
 */
const gchar *cream_tabbed_get_status (CreamTabbed *obj)
{
     return cream_view_get_status (CREAM_VIEW (obj->creamview));
}

/*!
  \fn GdkPixbuf *cream_tabbed_get_favicon (CreamTabbed *obj)
  \brief Get the favicon of the loaded URI
  \return A GdkPixbuf containing the favicon
 */
GdkPixbuf *cream_tabbed_get_favicon (CreamTabbed *obj)
{
     return cream_view_get_favicon (CREAM_VIEW (obj->creamview));
}

/*!
  \fn void echo (CreamTabbed *obj, const gchar *format, ...)
  \brief Display text into the inputbox. This function has the same syntax as printf()
  \param obj The CreamTabbed object which contain the inputbox
 */
void echo (CreamTabbed *obj, const gchar *format, ...)
{
     va_list arg;
     gchar *str;

     va_start (arg, format);
     str = g_strdup_vprintf (format, arg);
     va_end (arg);

     gtk_entry_set_text (GTK_ENTRY (obj->inputbox), str);
     g_free (str);
}

/* signals */
static void cream_tabbed_uri_changed_cb (CreamView *view, gchar *uri, CreamTabbed *obj)
{
     GdkColor color;
     gboolean ssl = g_str_has_prefix (uri, "https://");
     gboolean back = cream_view_can_go_back (CREAM_VIEW (obj->creamview));
     gboolean fwd = cream_view_can_go_forward (CREAM_VIEW (obj->creamview));

     if (back || fwd)
     {
          gtk_label_set_markup (GTK_LABEL (obj->statusbar.url), g_markup_printf_escaped (
               "<span font=\"%s\">%s</span> [%s%s]",
                    global.cfg.gui.statusbar.font,
                    uri, (back ? "+" : ""), (fwd ? "-" : ""))
          );
     }
     else
     {
          gtk_label_set_markup (GTK_LABEL (obj->statusbar.url), g_markup_printf_escaped (
               "<span font=\"%s\">%s</span>",
                    global.cfg.gui.statusbar.font,
                    uri)
          );
     }

     gdk_color_parse ((ssl ? global.cfg.gui.statusbar.bg.ssl : global.cfg.gui.statusbar.bg.normal), &color);
     gtk_widget_modify_bg (obj->statusbar.eventbox, GTK_STATE_NORMAL, &color);

     gdk_color_parse ((ssl ? global.cfg.gui.statusbar.fg.ssl : global.cfg.gui.statusbar.fg.normal), &color);
     gtk_widget_modify_fg (obj->statusbar.url, GTK_STATE_NORMAL, &color);
     gtk_widget_modify_fg (obj->statusbar.state, GTK_STATE_NORMAL, &color);

     g_signal_emit (
          G_OBJECT (obj),
          cream_tabbed_signals[UPDATE_NOTEBOOK_TITLE_SIGNAL],
          0
     );
}

static void cream_tabbed_new_title_cb (CreamView *view, gchar *title, CreamTabbed *obj)
{
     g_signal_emit (
          G_OBJECT (obj),
          cream_tabbed_signals[UPDATE_NOTEBOOK_TITLE_SIGNAL],
          0
     );
}

static void cream_tabbed_status_changed_cb (CreamView *view, gchar *status, CreamTabbed *obj)
{
     gtk_label_set_markup (GTK_LABEL (obj->statusbar.url), g_markup_printf_escaped (
          "<span font=\"%s\">%s</span>", global.cfg.gui.statusbar.font, status)
     );
}

static gboolean cream_tabbed_new_window_cb (CreamView *view, const gchar *uri, gpointer data)
{
     notebook_append_page (uri);
     gtk_widget_show_all (GTK_WIDGET (global.notebook));
     return TRUE;
}

static void cream_tabbed_scroll_cb (GtkAdjustment *vadjust, CreamTabbed *obj)
{
     gint max = gtk_adjustment_get_upper (vadjust) - gtk_adjustment_get_page_size (vadjust);
     gint val = (gint) (gtk_adjustment_get_value (vadjust) / max * 100);
     gchar *str;

     if (max == 0) /* If we see all the page */
          str = g_strdup ("All");
     else if (val == 0) /* we are on the page's top */
          str = g_strdup ("Top");
     else if (val == 100) /* we are on the page's bottom */
          str = g_strdup ("Bot");
     else
          str = g_strdup_printf ("%d%%", val);

     gtk_label_set_markup (GTK_LABEL (obj->statusbar.state),
          g_markup_printf_escaped ("<span font=\"%s\">%s</span>",
               global.cfg.gui.statusbar.font,
               str
          )
     );
}

/*! @} */
