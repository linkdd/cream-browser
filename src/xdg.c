#include "local.h"

/*!
 * \addtogroup xdg
 * @{
 */

static struct XDG_t XDG[] =
{
     { "XDG_CONFIG_HOME",     "~/.config" },
     { "XDG_DATA_HOME",       "~/.local/share" },
     { "XDG_CACHE_HOME",      "~/.cache" },
     { "XDG_CONFIG_DIRS",     PREFIX "/etc/xdg:/etc/xdg" },
     { "XDG_DATA_DIRS",       PREFIX "/share:/usr/local/share:/usr/share" },
     { NULL, NULL }
};

/*!
 * \fn char *get_xdg_var (struct XDG_t xdg)
 * @param xdg A #XDG_t field.
 * @return A newly allocated string.
 *
 * Get the value of a XDG variable. See get_xdg_var_by_name().
 */
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

/*!
 * \fn char *get_xdg_var_by_name (char *name)
 * @param name Name of the variable to get the value of.
 * @return A newly allocated string.
 *
 * Get the value of a XDG variable.
 */
char *get_xdg_var_by_name (char *name)
{
     int i;

     for (i = 0; XDG[i].env != NULL; ++i)
          if (strcmp (XDG[i].env, name) == 0)
               return get_xdg_var (XDG[i]);

     return NULL;
}

/*!
 * char *find_xdg_file (XDG_Type xdg_type, const char *filename)
 * @param xdg_type Type of the XDG file to find. See #XDG_Type
 * @param filename Basename of the wanted file.
 * @return The absolute path of \a filename.
 *
 * Search a file in all XDG directories.
 */
char *find_xdg_file (XDG_Type xdg_type, const char *filename)
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
          char *file = g_build_path (xdgv[i], "cream-browser", filename, NULL);
          if (g_file_test (file, G_FILE_TEST_EXISTS))
          {
               g_strfreev (xdgv);
               return file;
          }
     }

     g_strfreev (xdgv);
     return NULL;
}

/*! @} */
