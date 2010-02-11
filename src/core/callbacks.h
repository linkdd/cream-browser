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


#ifndef __CALLBACKS_H
#define __CALLBACKS_H

gboolean control_socket (GIOChannel *channel);
gboolean control_client_socket (GIOChannel *channel);

void cb_cream_destroy (GtkWidget *emit, gpointer data);
void cb_cream_update_notebook_title (GtkWidget *child, gpointer data);
void cb_cream_notebook_close_page (GtkButton *button, GtkWidget *child);
gboolean cb_inputbox_keys (GtkEntry *inputbox, GdkEventKey *event, CreamTabbed *obj);

#endif /* __CALLBACKS_H */
