#ifndef __SCHEME_H
#define __SCHEME_H

/*!
 * \defgroup uri URI's Utilities
 * \ingroup cream-browser
 * All about URIs.
 *
 * @{
 */

#include <glib.h>

/*!
 * \struct UriScheme
 * Parsed URL.
 */
typedef struct
{
     guint proto;        /*!< Integer identifier of the protocol used. */
     char *scheme;       /*!< Protocol used. */
     char *userinfo;     /*!< User informations. */
     char *hostname;     /*!< Hostname, or IP adresse. */
     int port;           /*!< Port, or -1. */
     char *path;         /*!< Path of the website's page. */
     char *query;        /*!< ie: <code>page.php?action=send</code> */
     char *fragment;     /*!< Anchor link, ie: <code>page.html#section</code> */
} UriScheme;

gboolean uri_scheme_parse (UriScheme *u, const gchar *uri);

/*! @} */

#endif /* __SCHEME_H */
