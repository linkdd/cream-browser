/*
*        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
*
*        This file is part of Cream-Browser
*
*        Cream-Browser is free software; you can redistribute it and/or modify
*        it under the terms of the GNU General Public License as published by
*        the Free Software Foundation, either version 3 of the License, or
*        (at your option) any later version.
*
*        Cream-Browser is distributed in the hope that it will be useful
*        but WITHOUT ANY WARRANTY; without even the implied warranty of
*        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*        GNU General Public License for more details.
*
*        You should have received a copy of the GNU General Public License
*        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
*/

/*!
  \file CreamTabbed.c
  \brief CreamTabbed object
  \author David Delassus
 */

#include "CreamTabbed.h"

enum
{
     UPDATE_NOTEBOOK_TITLE_SIGNAL,
     NB_SIGNALS
};

static guint cream_tabbed_signals[NB_SIGNALS] = { 0 };

static void cream_tabbed_class_init (CreamTabbedClass *class);
static void cream_tabbed_init (CreamTabbed *obj);

static void cream_tabbed_init_statusbar (CreamTabbed *obj);
static void cream_tabbed_init_inputbox (CreamTabbed *obj);

static void cream_tabbed_uri_changed_cb (CreamView *view, gchar *uri, CreamTabbed *obj);
static void cream_tabbed_new_title_cb (CreamView *view, gchar *title, CreamTabbed *obj);
static void cream_tabbed_status_changed_cb (CreamView *view, gchar *status, CreamTabbed *obj);
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
     g_signal_connect (G_OBJECT (obj->creamview), "uri-changed",    G_CALLBACK (cream_tabbed_uri_changed_cb),    obj);
     g_signal_connect (G_OBJECT (obj->creamview), "new-title",      G_CALLBACK (cream_tabbed_new_title_cb),      obj);
     g_signal_connect (G_OBJECT (obj->creamview), "status-changed", G_CALLBACK (cream_tabbed_status_changed_cb), obj);

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
     gdk_color_parse ("#000000", &bg);
     gtk_widget_modify_bg (obj->statusbar.eventbox, GTK_STATE_NORMAL, &bg);

     gdk_color_parse ("#FFFFFF", &fg);
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
     font = pango_font_description_from_string ("monospace normal 8");
     gtk_widget_modify_font (obj->inputbox, font);
     pango_font_description_free (font);

     /* border */
     gtk_entry_set_inner_border (GTK_ENTRY (obj->inputbox), NULL);
     gtk_entry_set_has_frame (GTK_ENTRY (obj->inputbox), FALSE);

     g_signal_connect (G_OBJECT (obj->inputbox), "activate", G_CALLBACK (cb_inputbox), obj);
}

/*!
  \fn void cream_tabbed_load_uri (CreamTabbed *obj, gchar *uri)
  \brief Load a new URI in the CreamView

  \param obj The CreamTabbed object
  \param uri New URI to load
 */
void cream_tabbed_load_uri (CreamTabbed *obj, gchar *uri)
{
     cream_view_load_uri (CREAM_VIEW (obj->creamview), uri);
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
  \fn GtkWidget *cream_tabbed_get_favicon (CreamTabbed *obj)
  \brief Get the favicon of the loaded URI
  \return A GtkImage containing the favicon
 */
GtkWidget *cream_tabbed_get_favicon (CreamTabbed *obj)
{
     return cream_view_get_favicon (CREAM_VIEW (obj->creamview));
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
               "<span font=\"monospace bold 8\">%s</span> [%s%s]", uri, (back ? "+" : ""), (fwd ? "-" : ""))
          );
     }
     else
     {
          gtk_label_set_markup (GTK_LABEL (obj->statusbar.url), g_markup_printf_escaped (
               "<span font=\"monospace bold 8\">%s</span>", uri)
          );
     }

     gdk_color_parse ((ssl ? "#B0FF00" : "#000000"), &color);
     gtk_widget_modify_bg (obj->statusbar.eventbox, GTK_STATE_NORMAL, &color);

     gdk_color_parse ((ssl ? "#000000" : "#FFFFFF"), &color);
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
          "<span font=\"monospace bold 8\">%s</span>", status)
     );
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

     gtk_label_set_markup (GTK_LABEL (obj->statusbar.state), g_markup_printf_escaped ("<span font=\"monospace bold 8\">%s</span>", str));
}
