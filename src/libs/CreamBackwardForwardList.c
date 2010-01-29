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

void cream_backward_forward_list_add_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item, gboolean prepend)
{
     GList *tmp = g_list_find_custom (list->backward, item, (GCompareFunc) list_compare_func);
     if (tmp == NULL)
     {
          if (prepend)
               list->backward = g_list_prepend (list->backward, item);
          else
               list->backward = g_list_append (list->backward, item);
     }
     else
     {
          CreamHistoryItem *tmp_item = g_list_nth_data (tmp, 0);
          g_free (tmp_item->title);
          tmp_item->title = g_strdup (item->title);
          tmp_item->timestamp = item->timestamp;
     }
}

void cream_backward_forward_list_del_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     list->backward = g_list_remove_all (list->backward, item);
}

void cream_backward_forward_list_add_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item, gboolean prepend)
{
     GList *tmp = g_list_find_custom (list->forward, item, (GCompareFunc) list_compare_func);
     if (tmp == NULL)
     {
          if (prepend)
               list->forward = g_list_prepend (list->forward, item);
          else
               list->forward = g_list_append (list->forward, item);
     }
     else
     {
          CreamHistoryItem *tmp_item = g_list_nth_data (tmp, 0);
          g_free (tmp_item->title);
          tmp_item->title = g_strdup (item->title);
          tmp_item->timestamp = item->timestamp;
     }
}

void cream_backward_forward_list_del_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
{
     list->forward = g_list_remove_all (list->forward, item);
}


