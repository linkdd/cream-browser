#ifndef __CREAM_BACKWARD_FORWARD_LIST_H
#define __CREAM_BACKWARD_FORWARD_LIST_H

#include <glib.h>
#include <time.h>

typedef struct _CreamBackwardForwardList CreamBackwardForwardList;
typedef struct _CreamHistoryItem CreamHistoryItem;

struct _CreamBackwardForwardList
{
     GList *backward;
     GList *forward;
};

struct _CreamHistoryItem
{
     gchar *uri;
     gchar *title;
     time_t timestamp;
};

CreamBackwardForwardList *cream_backward_forward_list_new (void);
CreamHistoryItem *cream_history_item_new (gchar *uri, gchar *title, time_t timestamp);

void cream_backward_forward_list_add_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);
CreamHistoryItem *cream_backward_forward_list_del_head_backward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);

void cream_backward_forward_list_add_head_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);
CreamHistoryItem *cream_backward_forward_list_del_head_forward_item (CreamBackwardForwardList *list, CreamHistoryItem *item);

#endif /* __CREAM_BACKWARD_FORWARD_LIST_H */
