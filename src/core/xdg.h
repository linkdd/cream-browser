#ifndef __XDG_H
#define __XDG_H

#define XDG_TYPE_CONFIG 0
#define XDG_TYPE_DATA   1
#define XDG_TYPE_CACHE  2

struct XDG_t
{
     gchar *env;
     gchar *val;
};

gchar *get_xdg_var (struct XDG_t xdg);
gchar *get_xdg_var_by_name (gchar *name);
gchar *find_xdg_file (int xdg_type, const char *filename);

extern struct XDG_t XDG[];

#endif /* __XDG_H */
