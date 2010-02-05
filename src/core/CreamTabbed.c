#include "CreamTabbed.h"

static void cream_tabbed_class_init (CreamTabbedClass *class);
static void cream_tabbed_init (CreamTabbed *obj);

static void cream_tabbed_init_statusbar (CreamTabbed *obj);
static void cream_tabbed_init_inputbox (CreamTabbed *obj);

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
     return;
}

static void cream_tabbed_init (CreamTabbed *obj)
{
     obj->creamview = cream_view_new ();

     obj->statusbar.eventbox = gtk_event_box_new ();
     obj->statusbar.box      = gtk_hbox_new (FALSE, 0);
     obj->statusbar.url      = gtk_label_new (NULL);
     obj->statusbar.state    = gtk_label_new (NULL);

     obj->inputbox = gtk_entry_new ();
}

GtkWidget *cream_tabbed_new (void)
{
     CreamTabbed *obj = gtk_type_new (cream_tabbed_get_type ());

     cream_tabbed_init_statusbar (obj);
     cream_tabbed_init_inputbox (obj);

     gtk_box_pack_end (GTK_BOX (obj), obj->inputbox, FALSE, FALSE, 0);
     gtk_box_pack_end (GTK_BOX (obj), obj->statusbar.eventbox, FALSE, FALSE, 0);
     gtk_box_pack_start (GTK_BOX (obj), obj->creamview, TRUE, TRUE, 0);

     return GTK_WIDGET (obj);
}

static void cream_tabbed_init_statusbar (CreamTabbed *obj)
{
     GdkColor bg, fg;

     obj->statusbar.box      = gtk_hbox_new (FALSE, 0);
     obj->statusbar.eventbox = gtk_event_box_new ();
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

     gtk_label_set_selectable (GTK_LABEL (obj->statusbar.state), TRUE);

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
}
