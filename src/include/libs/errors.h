#ifndef __ERRORS_H
#define __ERRORS_H

#include <string.h>
#include <stdarg.h>

#include <glib.h>
#include <glib/gprintf.h>

typedef enum { ERROR_FATAL = 0, ERROR_CRITICAL, ERROR_WARNING } ErrorLevel;

typedef struct ErrorList
{
     ErrorLevel level;
     char *message;
} ErrorList;

typedef struct ErrorDomain
{
     char *domainname;
     guint id;

     GQueue *errors;
} ErrorDomain;

typedef void (*ReceiveError) (guint, ErrorLevel, const char *, gpointer);

typedef struct CallbackList
{
     ReceiveError callback;
     gpointer data;
} CallbackList;

guint error_domain_register (const char *domainname);
const char *error_domain_get (guint id);
void error_send (guint id, ErrorLevel level, const char *fmt, ...);
void error_add_callback (ReceiveError new_callback, gpointer data);

#endif /* __ERRORS_H */
