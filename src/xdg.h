#ifndef __XDG_H
#define __XDG_H

#define XDG_TYPE_CONFIG  0
#define XDG_TYPE_DATA    1
#define XDG_TYPE_CACHE   2

struct XDG_t
{
     char *env;
     char *val;
};

char *get_xdg_var (struct XDG_t xdg);
char *get_xdg_var_by_name (char *name);
char *find_xdg_file (int xdg_type, const char *filename);

#endif /* __XDG_H */
