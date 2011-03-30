#ifndef __STRUCTS_H
#define __STRUCTS_H

/*! \struct GUI */
struct GUI
{
     GtkWidget *notebook;     /*!< Notebook widget (#CreamTab object) */
     GtkWidget *viewarea;     /*!< WebView area (#ViewArea object) */
     GtkWidget *statusbar;    /*!< Statusbar widget */
     GtkWidget *promptbox;    /*!< Promptbox widget */
     GtkWidget *box;          /*!< Box containing all widgets */
};

/*! \struct Socket */
struct Socket
{
     gchar *path;             /*!< Path to the Unix socket */
     int fd;                  /*!< Socket file descriptor */
     GIOChannel *channel;     /*!< \class{GIOChannel} to use the socket */
};

/*!
 * \struct global_t
 * \brief Global structure to access in every point of the program to all usefull structures and widgets
 */
struct global_t
{
     struct GUI gui;          /*!< GUI */
     struct Socket sock;      /*!< Unix socket */

     gboolean log;            /*!< if <code>TRUE</code> log every errors */
     guint domain;            /*!< Global error domain */
     gchar *prgname;          /*!< Program's name */

     GList *protocols;        /*!< List of protocols and asscoiated modules */

     lua_State *luavm;        /*!< Lua VM state */
};

#endif /* __STRUCTS_H */
