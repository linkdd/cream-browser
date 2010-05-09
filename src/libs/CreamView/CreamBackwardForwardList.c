/*
 *  Copyright (c) 2010, David Delassus <linkdd@ydb.me>
 *
 *  This file is part of Cream-Browser
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following
 *  conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  @defgroup CreamBackwardForwardList Cream History
  @ingroup CreamView
  @brief Functions to use the history

  @{
 */

#include "CreamBackwardForwardList.h"

static gint list_compare_func (CreamHistoryItem *a, CreamHistoryItem *b)
{
     return g_strcmp0 (a->uri, b->uri);
}

/*!
  \fn CreamBackwardForwardList *cream_backward_forward_list_new (void)
  \brief Initialise a new history

  \return A new history
 */
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

/*!
  \fn CreamHistoryItem *cream_history_item_new (gchar *uri, gchar *title, time_t timestamp)
  \brief Create a new history item

  \param uri URI of the new hisotry item
  \param title Title of the new history item
  \param timestamp Timestamp when the page was last accessed
  \return A new history item
 */
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

/*!
  \fn void cream_backward_forward_list_add_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
  \brief Add a new history item in the backward history

  \param list The history
  \param item The history item
 */
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

/*!
  \fn CreamHistoryItem *cream_backward_forward_list_del_head_backward_item (CreamBackwardForwardList *list)
  \brief Delete an item from the backward history

  \param list The history
  \return The item deleted
 */
CreamHistoryItem *cream_backward_forward_list_del_head_backward_item (CreamBackwardForwardList *list)
{
     GList *tmp = g_list_first (list->backward);
     list->backward = g_list_remove_link (list->backward, tmp);
     return (CreamHistoryItem *) g_list_nth_data (tmp, 0);
}

/*!
  \fn void cream_backward_forward_list_add_head_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item)
  \brief Add a new history item in the forward history

  \param list The history
  \param item The history item
 */
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

/*!
  \fn CreamHistoryItem *cream_backward_forward_list_del_forward_item (CreamBackwardForwardList *list)
  \brief Delete an item from the forward history

  \param list The history
  \return Ther item deleted
 */
CreamHistoryItem *cream_backward_forward_list_del_forward_item (CreamBackwardForwardList *list)
{
     GList *tmp = g_list_first (list->forward);
     list->forward = g_list_remove_link (list->forward, tmp);
     return (CreamHistoryItem *) g_list_nth_data (tmp, 0);

}

/*! @} */
