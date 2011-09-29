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

#include "scheme.h"
#include <string.h>

/*!
 * \addtogroup utils
 * @{
 */

/*!
 * @param u A pointer to #UriScheme structure.
 * @param uri A valid URI to parse.
 * @return \c TRUE on success, \c FALSE otherwise.
 *
 * Parse a valid URI into a structure to access easily to useful
 * URI's data (see #UriScheme).
 *
 * \code
 * UriScheme uri;
 * uri_scheme_parse (&uri, "http://user:password@google.com:847/sub/page.html?a=connect#anchor");
 *
 * printf ("Scheme:   %s\n", uri.scheme);
 * printf ("Userinfo: %s\n", uri.userinfo);
 * printf ("Hostname: %s\n", uri.hostname);
 * printf ("Port:     %d\n", uri.port);
 * printf ("Path:     %s\n", uri.path);
 * printf ("Query:    %s\n", uri.query);
 * printf ("Fragment: %s\n", uri.fragment);
 * \endcode
 */
gboolean uri_scheme_parse (UriScheme *u, const gchar *uri)
{
     GString *scheme = NULL;
     GString *userinfo = NULL;
     GString *hostname = NULL;
     GString *path = NULL;
     GString *query = NULL;
     GString *fragment = NULL;
     guint port = 0;

     const gchar *p, *tmp;

     g_return_val_if_fail (u != NULL, FALSE);
     g_return_val_if_fail (uri != NULL, FALSE);

     /* skip whitespaces */
     for (p = uri; *p && g_ascii_isspace (*p); ++p);
     g_return_val_if_fail (*p != '\0', FALSE);

     /* scheme */
     if (p[0] == '/')
          scheme = g_string_new ("file");
     else
     {
          for (tmp = p; *p && *p != ':' && *p != '/' && *p != '?' && *p != '#'; ++p);

          if (*p == ':')
          {
               scheme = g_string_new_len (tmp, (gssize) (p - tmp));
               ++p;
          }
          else /* This char is NUL, /, ?, or # */
               p = tmp;
     }

     /* Authority */
     if (*p == '/' && p[1] == '/')
          p += 2;

     /* userinfo */
     for (tmp = p; *p && *p != '@' && *p != '/'; ++p); /* Look for @ or / */

     if (*p == '@')
     {
          userinfo = g_string_new_len (tmp, (gssize) (p - tmp));
          ++p;
     }
     else
          p = tmp;

     /* hostname */

     /* check for no hostname at all (e.g. file:// URIs) */
     if (*p == '/')
          goto _path;

     /* check for IPv6 cannonical hostname in brackets */
     if (*p == '[')
     {
          ++p; /* skip [ */

          for (tmp = p; *p && *p != ']'; ++p);
          g_return_val_if_fail ((p - tmp) != 0, FALSE);

          hostname = g_string_new_len (tmp, (gssize) (p - tmp));

          if (*p) ++p; /* skip ] */
     }
     else
     {
          for (tmp = p; *p && *p != '/' && *p != '?' && *p != '#' && *p != ':'; ++p);
          g_return_val_if_fail ((p - tmp) != 0, FALSE);

          hostname = g_string_new_len (tmp, (gssize) (p - tmp));
     }

     /* port */
     if (*p == ':')
          for (++p; g_ascii_isdigit ((int) *p); ++p)
               port = port * 10 + (*p - '0');

     /* path */
_path:
     for (tmp = p; *p && *p != '?' && *p != '#'; ++p);

     if (p != tmp)
          path = g_string_new_len (tmp, (gssize) (p - tmp));

     /* query */
     if (*p == '?')
     {
          for (tmp = p + 1; *p && *p != '#'; ++p);
          query = g_string_new_len (tmp, (gssize) (p - tmp));
     }

     /* Fragment */
     if (*p == '#')
     {
          ++p;
          fragment = g_string_new_len (p, strlen (p));
     }

     /* put all of this into the structure. */
     u->string   = g_strdup (uri);
     u->scheme   = (scheme ? g_string_free (scheme, FALSE) : NULL);
     u->userinfo = (userinfo ? g_string_free (userinfo, FALSE) : NULL);
     u->hostname = (hostname ? g_string_free (hostname, FALSE) : NULL);
     u->port     = port;
     u->path     = (path ? g_string_free (path, FALSE) : NULL);
     u->query    = (query ? g_string_free (query, FALSE) : NULL);
     u->fragment = (fragment ? g_string_free (fragment, FALSE) : NULL);

     return TRUE;
}

/*! @} */
