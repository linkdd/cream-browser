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

/*!
  \file CreamBackwardForwardList.h
  \brief History managment's header
  \author David Delassus
 */

#ifndef __CREAM_BACKWARD_FORWARD_LIST_H
#define __CREAM_BACKWARD_FORWARD_LIST_H

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

#endif /* __CREAM_BACKWARD_FORWARD_LIST_H */
