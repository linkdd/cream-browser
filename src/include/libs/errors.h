#ifndef __ERRORS_H
#define __ERRORS_H

#include <sys/queue.h>
#include <glib.h>
#include <string.h>
#include <stdarg.h>

typedef unsigned long id_t;

typedef enum { ERROR_FATAL = 0, ERROR_CRITICAL, ERROR_WARNING } ErrorLevel;

typedef struct ErrorList
{
     ErrorLevel level;
     char *message;

     SIMPLEQ_ENTRY (ErrorList) next;
} ErrorList;

typedef struct ErrorDomain
{
     char *domainname;
     id_t id;

     SIMPLEQ_HEAD (, ErrorList) errors;

     SIMPLEQ_ENTRY (ErrorDomain) next;
} ErrorDomain;

typedef void (*ReceiveError) (id_t, ErrorLevel, const char *, gpointer);

typedef struct CallbackList
{
     ReceiveError callback;
     gpointer data;

     SIMPLEQ_ENTRY (CallbackList) next;
} CallbackList;

int register_domain (const char *domainname);
const char *get_domain (id_t id);
void set_error (id_t id, ErrorLevel level, const char *fmt, ...);
void add_callback (ReceiveError new_callback, gpointer data);

#endif /* __ERRORS_H */
