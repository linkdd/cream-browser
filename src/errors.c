#include "errors.h"

static GList *domains   = NULL;
static GList *callbacks = NULL;

guint error_domain_register (const char *domainname)
{
     guint id = g_str_hash (domainname);
     ErrorDomain *el;
     GList *tmp;

     for (tmp = domains; tmp != NULL; tmp = tmp->next)
     {
          el = tmp->data;

          if (el->id == id)
               return id;
     }

     el = g_malloc (sizeof (ErrorDomain));
     el->domainname = g_strdup (domainname);
     el->errors = g_queue_new ();
     el->id = id;

     domains = g_list_append (domains, el);

     return id;
}

const char *error_domain_get (guint id)
{
     GList *tmp;

     for (tmp = domains; tmp != NULL; tmp = tmp->next)
     {
          ErrorDomain *el = tmp->data;
          if (id == el->id)
               return el->domainname;
     }

     return NULL;
}

void error_send (guint id, ErrorLevel level, const char *fmt, ...)
{
     ErrorDomain *el;
     ErrorList *err;
     GList *tmp;

     va_list args;
     char *msg = NULL;

     va_start (args, fmt);
     g_vasprintf (&msg, fmt, args);
     va_end (args);

     err = g_malloc (sizeof (ErrorList));
     err->message = g_strdup (msg);
     err->level = level;

     for (tmp = domains; tmp != NULL; tmp = tmp->next)
     {
          el = tmp->data;
          if (id == el->id)
          {
               g_queue_push_tail (el->errors, err);
               break;
          }
     }

     for (tmp = callbacks; tmp != NULL; tmp = tmp->next)
     {
          CallbackList *cb = tmp->data;
          cb->callback (id, level, msg, cb->data);
     }
}

void error_add_callback (ReceiveError new_callback, gpointer data)
{
     CallbackList *cb = g_malloc (sizeof (CallbackList));
     cb->callback = new_callback;
     cb->data = data;

     callbacks = g_list_append (callbacks, cb);
}
