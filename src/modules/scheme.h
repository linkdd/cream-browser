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
     char *scheme;       /*!< Protocol used. */
     char *userinfo;     /*!< User informations. */
     char *hostname;     /*!< Hostname, or IP adresse. */
     int port;           /*!< Port, or -1. */
     char *path;         /*!< Path of the website's page. */
     char *query;        /*!< ie: \verbatim page.php?action=send \endverbatim  */
     char *fragment;     /*!< Anchor link, ie: \verbatim page.html#section \endverbatim  */
} UriScheme;

gboolean uri_scheme_parse (UriScheme *u, const gchar *uri);

/*! @} */

#endif /* __SCHEME_H */
