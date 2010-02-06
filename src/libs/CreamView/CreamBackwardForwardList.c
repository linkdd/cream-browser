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

#include "CreamBackwardForwardList.h"

static gint list_compare_func (CreamHistoryItem *a, CreamHistoryItem *b)
{
     return g_strcmp0 (a->uri, b->uri);
}

CreamBackwardForwardList *cream_backward_forward_list_new (void)
{
     CreamBackwardForwardList *ret = g_malloc (sizeof (CreamBackwardForwardList));

     if (ret != NULL)
     {
          ret->backward = NULL;
          ret->forward = NULL;
     }

     return ret;
}

CreamHistoryItem *cream_history_item_new (gchar *uri, gchar *title, time_t timestamp)
{
     CreamHistoryItem *ret = g_malloc (sizeof (CreamHistoryItem));

     if (ret != NULL)
     {
          ret->uri = g_strdup (uri);
          ret->title = g_strdup (uri);
          ret->timestamp = timestamp;
     }

     return ret;
}

void cream_backward_forward_list_add_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     GList *tmp = g_list_find_custom (list->backward, item, (GCompareFunc) list_compare_func);
     if (tmp == NULL)
          list->backward = g_list_prepend (list->backward, item);
     else
     {
          CreamHistoryItem *tmp_item = g_list_nth_data (tmp, 0);
          g_free (tmp_item->title);
          tmp_item->title = g_strdup (item->title);
          tmp_item->timestamp = item->timestamp;
     }
}

CreamHistoryItem *cream_backward_forward_list_del_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     GList *tmp = g_list_first (list->backward);
     list->backward = g_list_remove_link (list->backward, tmp);
     return (CreamHistoryItem *) g_list_nth_data (tmp, 0);
}

void cream_backward_forward_list_add_head_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     GList *tmp = g_list_find_custom (list->forward, item, (GCompareFunc) list_compare_func);
     if (tmp == NULL)
          list->forward = g_list_prepend (list->forward, item);
     else
     {
          CreamHistoryItem *tmp_item = g_list_nth_data (tmp, 0);
          g_free (tmp_item->title);
          tmp_item->title = g_strdup (item->title);
          tmp_item->timestamp = item->timestamp;
     }
}

CreamHistoryItem *cream_backward_forward_list_del_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     GList *tmp = g_list_first (list->forward);
     list->forward = g_list_remove_link (list->forward, tmp);
     return (CreamHistoryItem *) g_list_nth_data (tmp, 0);

}

