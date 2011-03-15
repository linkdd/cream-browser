#ifndef __XDG_H
#define __XDG_H

/*!
 * \defgroup xdg XDG Utilities
 * \ingroup cream-browser
 * Functions relative to XDG.
 *
 * @{
 */

/*!
 * \enum XDG_Type
 * Used for find_xdg_file(), it determines the type of the wanted file.
 */
typedef enum
{
     XDG_TYPE_CONFIG = 0,     /*!< For configuration files. (ie: <code>/etc/xdg/...</code>) */
     XDG_TYPE_DATA,           /*!< For data files (ie: <code>/usr/share/...</code>) */
     XDG_TYPE_CACHE           /*!< For cache files (ie: <code>~/.cache/...</code>) */
} XDG_Type;

/*!
 * \struct XDG_t
 * Define a default value associated to a XDG variable.
 */
struct XDG_t
{
     char *env;     /*!< XDG variable's name */
     char *val;     /*!< Default XDG variable's value. */
};

char *get_xdg_var (struct XDG_t xdg);
char *get_xdg_var_by_name (char *name);
char *find_xdg_file (XDG_Type xdg_type, const char *filename);


/*! @} */

#endif /* __XDG_H */
