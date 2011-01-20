#include <libs/errors.h>

static SIMPLEQ_HEAD (, ErrorDomain)  domains   = SIMPLEQ_HEAD_INITIALIZER (domains);
static SIMPLEQ_HEAD (, CallbackList) callbacks = SIMPLEQ_HEAD_INITIALIZER (callbacks);

static id_t make_id (const char *data)
{
     id_t hash = 5381;

     while (*data != 0)
     {
          int c = *data;
          hash = ((hash << 5) + hash) + c;
          data++;
     };

     return hash;
}

int register_domain (const char *domainname)
{
     id_t id = make_id (domainname);
     ErrorDomain *tmp;

     SIMPLEQ_FOREACH (tmp, &domains, next)
          if (tmp->id == id)
               return id;

     tmp = malloc (sizeof (ErrorDomain));
     SIMPLEQ_INIT (&(tmp->errors));
     tmp->id = id;

     tmp->domainname = malloc (strlen (domainname));
     memset (tmp->domainname, 0, strlen (domainname));
     tmp->domainname = strcpy (tmp->domainname, domainname);

     SIMPLEQ_INSERT_TAIL (&domains, tmp, next);

     return id;
}

const char *get_domain (id_t id)
{
     ErrorDomain *tmp;

     SIMPLEQ_FOREACH (tmp, &domains, next)
          if (id == tmp->id)
               return tmp->domainname;

     return NULL;
}

void set_error (id_t id, ErrorLevel level, const char *fmt, ...)
{
     ErrorDomain *tmp;
     CallbackList *cb;
     va_list args;
     char *msg = NULL;

     va_start (args, fmt);
     g_vasprintf (&msg, fmt, args);
     va_end (args);

     ErrorList *err = malloc (sizeof (ErrorList));
     err->level = level;

     err->message = malloc (strlen (msg));
     memset (err->message, 0, strlen (msg));
     err->message = strcpy (err->message, msg);

     SIMPLEQ_FOREACH (tmp, &domains, next)
     {
          if (id == tmp->id)
          {
               SIMPLEQ_INSERT_TAIL (&(tmp->errors), err, next);
               break;
          }
     }

     SIMPLEQ_FOREACH (cb, &callbacks, next)
          cb->callback (id, level, msg, cb->data);
}

void add_callback (ReceiveError new_callback, gpointer data)
{
     CallbackList *cb = malloc (sizeof (CallbackList));
     cb->callback = new_callback;
     cb->data = data;

     SIMPLEQ_INSERT_TAIL (&callbacks, cb, next);
}
