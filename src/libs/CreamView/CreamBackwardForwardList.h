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

#ifndef __CREAM_BACKWARD_FORWARD_LIST_H
#define __CREAM_BACKWARD_FORWARD_LIST_H

/*!
  @addtogroup CreamBackwardForwardList
  @{
 */

#include <glib.h>
#include <time.h>

typedef struct _CreamBackwardForwardList CreamBackwardForwardList;
typedef struct _CreamHistoryItem CreamHistoryItem;

/*! \struct _CreamBackwardForwardList */
struct _CreamBackwardForwardList
{
     GList *backward;    /*!< Backward history */
     GList *forward;     /*!< Forward history */
};

/*! \struct _CreamHistoryItem */
struct _CreamHistoryItem
{
     gchar *uri;         /*!< URI of the history item */
     gchar *title;       /*!< Title of the history item */
     time_t timestamp;   /*!< Timestamp when the page was last accessed */
};

CreamBackwardForwardList *cream_backward_forward_list_new (void);
CreamHistoryItem *cream_history_item_new (gchar *uri, gchar *title, time_t timestamp);

void cream_backward_forward_list_add_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);
CreamHistoryItem *cream_backward_forward_list_del_head_backward_item (CreamBackwardForwardList *list);

void cream_backward_forward_list_add_head_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);
CreamHistoryItem *cream_backward_forward_list_del_head_forward_item (CreamBackwardForwardList *list);

/*! @} */

#endif /* __CREAM_BACKWARD_FORWARD_LIST_H */
