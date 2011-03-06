#ifndef __STRUCTS_H
#define __STRUCTS_H

/*! \struct GUI */
struct GUI
{
     GtkWidget *notebook;     /*!< Notebook widget */
     GtkWidget *webview;      /*!< Focused webview */
     GtkWidget *statusbar;    /*!< Statusbar widget */
     GtkWidget *promptbox;    /*!< Promptbox widget */
     GtkWidget *box;          /*!< Box containing all widgets */
};

/*! \struct Socket */
struct Socket
{
     gchar *path;             /*!< Path to the Unix socket */
     GIOChannel *channel;     /*!< #GIOChannel to use the socket */
};

/*!
 * \struct global_t
 * \brief Global structure to access in every point of the program to all usefull structures and widgets
 */
struct global_t
{
     struct GUI gui;          /*!< GUI */
     struct Socket sock;      /*!< Unix socket */

     gboolean log;            /*!< if TRUE log every errors */
     guint domain;            /*!< Global error domain */
};

/*! \struct Tab */
struct Tab
{
     gchar *title;            /*!< Title of the page loaded in the tab */
     gchar *uri;              /*!< URI of the page loaded in the tab */
     GtkWidget *webview;      /*!< Webview associated to the tab */
};

#endif /* __STRUCTS_H */
