#ifndef __STRUCTS_H
#define __STRUCTS_H

struct GUI
{
     GtkWidget *notebook;
     GtkWidget *webview;
     GtkWidget *statusbar;
     GtkWidget *promptbox;
};

struct Socket
{
     gchar *path;
     GIOChannel *channel;
};

#endif /* __STRUCTS_H */
