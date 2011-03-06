#include "local.h"

static struct XDG_t XDG[] =
{
     { "XDG_CONFIG_HOME",     "~/.config" },
     { "XDG_DATA_HOME",       "~/.local/share" },
     { "XDG_CACHE_HOME",      "~/.cache" },
     { "XDG_CONFIG_DIRS",     PREFIX "/etc/xdg:/etc/xdg" },
     { "XDG_DATA_DIRS",       PREFIX "/share:/usr/local/share:/usr/share" },
     { NULL, NULL }
};

char *get_xdg_var (struct XDG_t xdg)
{
     const gchar *actual_val = getenv (xdg.env);
     const gchar *home = g_get_home_dir ();
     gchar *ret;

     if (!actual_val || !strlen (actual_val))
     {
          if (xdg.val)
               ret = str_replace ("~", home, xdg.val);
          else
               ret = NULL;
     }
     else
          ret = str_replace ("~", home, actual_val);

     return ret;
}

char *get_xdg_var_by_name (char *name)
{
     int i;

     for (i = 0; XDG[i].env != NULL; ++i)
          if (strcmp (XDG[i].env, name) == 0)
               return get_xdg_var (XDG[i]);

     return NULL;
}

char *find_xdg_file (int xdg_type, const char *filename)
{
     char *xdg_var = NULL;
     char **xdgv;
     int i;

     switch (xdg_type)
     {
          case XDG_TYPE_CONFIG:
               xdg_var = g_strconcat (get_xdg_var_by_name ("XDG_CONFIG_HOME"), ":",
                                      get_xdg_var_by_name ("XDG_CONFIG_DIRS"), NULL);
               break;

          case XDG_TYPE_DATA:
               xdg_var = g_strconcat (get_xdg_var_by_name ("XDG_DATA_HOME"), ":",
                                      get_xdg_var_by_name ("XDG_DATA_DIRS"), NULL);
               break;

          case XDG_TYPE_CACHE:
               xdg_var = get_xdg_var_by_name ("XDG_CACHE_HOME");
     }

     xdg_var = str_replace ("~", g_get_home_dir (), xdg_var);
     xdgv = g_strsplit (xdg_var, ":", -1);

     for (i = 0; xdgv[i] != NULL; ++i)
     {
          char *file = g_build_path (xdgv[i], filename, NULL);
          if (g_file_test (file, G_FILE_TEST_EXISTS))
          {
               g_strfreev (xdgv);
               return file;
          }
     }

     g_strfreev (xdgv);
     return NULL;
}
