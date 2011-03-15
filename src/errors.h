#ifndef __ERRORS_H
#define __ERRORS_H

#include <string.h>
#include <stdarg.h>

#include <glib.h>
#include <glib/gprintf.h>

/*!
 * \defgroup errors Reporting errors system.
 * Functions to send and receive errors from modules.
 *
 * @{
 */

/*!
 * \enum ErrorLevel
 * Used to determine the type of error.
 */
typedef enum
{
     ERROR_FATAL = 0,    /*!< The error is fatal, Cream-Browser can't continue. */
     ERROR_CRITICAL,     /*!< The error is critical but Cream-Browser can continue. */
     ERROR_WARNING       /*!< No error, but have to be noticed anyway. */
} ErrorLevel;

/*!
 * \struct ErrorList
 * \implements ErrorLevel
 * One error (level and message). This structure is added to a \class{GQueue} object.
 */
typedef struct ErrorList
{
     /*< private >*/
     ErrorLevel level;
     char *message;
} ErrorList;

/*!
 * \struct ErrorDomain
 * The domain of error.
 */
typedef struct ErrorDomain
{
     /*< private >*/
     char *domainname;
     guint id;

     GQueue *errors;
} ErrorDomain;

/*!
 * \fn typedef void (*ReceiveError) (guint domain, ErrorLevel level, const char *msg, gpointer data)
 * @param domain The domain identifier.
 * @param level The error level. (see #ErrorLevel)
 * @param msg The error message.
 * @param data User data passed to the callback.
 *
 * Used to define a function to be called when an error occurs.
 */
typedef void (*ReceiveError) (guint, ErrorLevel, const char *, gpointer);

/*!
 * \struct CallBackList
 * \implements ReceiveError
 */
typedef struct CallbackList
{
     ReceiveError callback;   /*!< The error callback function. (see #ReceiveError) */
     gpointer data;           /*!< Data passed to the function. */
} CallbackList;

guint error_domain_register (const char *domainname);
const char *error_domain_get (guint id);
void error_send (guint id, ErrorLevel level, const char *fmt, ...);
void error_add_callback (ReceiveError new_callback, gpointer data);

/*! @} */

#endif /* __ERRORS_H */
