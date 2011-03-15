#include "local.h"

/*!
 * \addtogroup errors
 * @{
 */

static GList *domains   = NULL;
static GList *callbacks = NULL;

/*!
 * \fn guint error_domain_register (const char *domainname)
 * @param domainname The name of the error domain.
 * @return An integer identifier for the domain.
 *
 * Register a new error domain. A \class{GQueue} is allocated and every
 * errors pushed on this domain name are stored in this queue.
 * If the domain already exists, just return its identifier.
 *
 */
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

/*!
 * \fn const char *error_domain_get (guint id)
 * @param id Error domain identifier.
 * @return The domain name.
 *
 * Return the name of the error domain associated to the \a id.
 *
 */
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

/*!
 * \fn void error_send (guint id, ErrorLevel level, const char *fmt, ...)
 * @param id The error domain identifier
 * @param level Type of error (see #ErrorLevel)
 * @param fmt Error message (same syntax as printf)
 * @param ... Data to put on the string.
 *
 * Send an error on the specified domain.
 */
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

/*!
 * \fn void error_add_callback (ReceiveError new_callback, gpointer data)
 * @param new_callback Callback function. (see #ReceiveError)
 * @param data User data to pass to the callback function.
 *
 * Add a new function to call when an error is received.
 * Functions are called in the exact order they were added.
 */
void error_add_callback (ReceiveError new_callback, gpointer data)
{
     CallbackList *cb = g_malloc (sizeof (CallbackList));
     cb->callback = new_callback;
     cb->data = data;

     callbacks = g_list_append (callbacks, cb);
}

/*! @} */
