#ifndef __STRUCTS_H
#define __STRUCTS_H

struct global_t
{
     struct
     {
          gchar *config;
          gchar *url;
     } cmdline;

     struct
     {
          gchar *homepage;
          gchar *encoding;
          gboolean javascript;

          SoupCookieJar *cookies;
     } browser;
};

#endif /* __STRUCTS_H */
