#ifndef __CONFIG_H
#define __CONFIG_H

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <glib.h>

struct cfg_t
{
     struct
     {
          struct
          {
               char *font;
          } global;

          struct
          {
               char *font;
               char *bg_normal;
               char *bg_secure;
               char *fg_normal;
               char *fg_secure;
          } statusbar;

          struct
          {
               char *font;
               char *border;
               char *bg_normal;
               char *bg_focus;
               char *fg_normal;
               char *fg_focus;
          } tab;

          struct
          {
               char *font;
               char *bg_normal;
               char *bg_error;
               char *fg_normal;
               char *fg_error;
          } prompt;
     } theme;

     GList *modules;

     struct
     {
          gboolean enable;
          char *url;
     } proxy;

     gboolean javascript;
     gboolean cookies;
     gboolean session;

     char *encodage;
     char *homepage;

     GList *widgets;

     struct
     {
          char **modifiers;
     } buttons;
};

void libconfig_init (void);

#endif /* __CONFIG_H */
