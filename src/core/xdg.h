/*
 *        Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *        This file is part of Cream-Browser
 *
 *        Cream-Browser is free software; you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by
 *        the Free Software Foundation, either version 3 of the License, or
 *        (at your option) any later version.
 *
 *        Cream-Browser is distributed in the hope that it will be useful
 *        but WITHOUT ANY WARRANTY; without even the implied warranty of
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *        GNU General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License
 *        along with Cream-Browser. If not, see <http://www.gnu.org/licenses/>.
 */

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
