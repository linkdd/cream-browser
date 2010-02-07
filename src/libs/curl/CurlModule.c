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
  \file CurlModule.c
  \brief Curl integration
  \author David Delassus
 */

#include "CurlModule.h"
#include <marshal.h>
#include <string.h>

#define CURL_MODULE_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_CURL_MODULE, CurlModulePrivate))

typedef struct _CurlModulePrivate CurlModulePrivate;

/*!
  \struct _CurlModulePrivate
  \brief CurlModule private data
 */
struct _CurlModulePrivate
{
     CURL *context;      /*!< Curl object */
     CURLcode result;    /*!< Result of CURL loading */
     GString *content;   /*!< Content downloaded */
};

static GObject *curl_module_constructor (GType type, guint n_properties, GObjectConstructParam *properties);
static void curl_module_finalize (GObject *self);

/*! \brief CurlModule signals definition */
enum
{
     LOAD_COMMITTED_SIGNAL,
     LOAD_STARTED_SIGNAL,
     LOAD_PROGRESS_CHANGED_SIGNAL,
     LOAD_FINISHED_SIGNAL,
     LOAD_ERROR_SIGNAL,

     URI_CHANGED_SIGNAL,
     STATUS_CHANGED_SIGNAL,

     NB_SIGNALS
};

static guint curl_module_signals[NB_SIGNALS] = {0};

/*!
  \enum CurlModuleError
  \brief CurlModule errors definition
 */
typedef enum
{
     CURL_MODULE_ERROR_CURL_INIT,
     CURL_MODULE_ERROR_CURL_ERROR,
     CURL_MODULE_ERROR_THREAD,
     CURL_MODULE_ERROR_FAILED
} CurlModuleError;

#define CURL_MODULE_ERROR     (curl_module_error_quark ())

static GQuark curl_module_error_quark (void)
{
     static GQuark curl_module_error = 0;

     if (!curl_module_error)
     {
          curl_module_error = g_quark_from_string ("CURL_MODULE_ERROR");
     }

     return curl_module_error;
}

/* Define object */
G_DEFINE_TYPE (CurlModule, curl_module, G_TYPE_OBJECT)

static void curl_module_class_init (CurlModuleClass *class)
{
     GObjectClass *gobject_class = G_OBJECT_CLASS (class);

     g_type_class_add_private (class, sizeof (CurlModulePrivate));

     curl_module_signals[LOAD_COMMITTED_SIGNAL] = g_signal_new (
               "load-committed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, load_committed),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_module_signals[LOAD_STARTED_SIGNAL] = g_signal_new (
               "load-started",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, load_started),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_module_signals[LOAD_PROGRESS_CHANGED_SIGNAL] = g_signal_new (
               "load-progress-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, load_progress_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__FLOAT,
               G_TYPE_NONE,
               1, G_TYPE_FLOAT);

     curl_module_signals[LOAD_FINISHED_SIGNAL] = g_signal_new (
               "load-finished",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, load_finished),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_module_signals[LOAD_ERROR_SIGNAL] = g_signal_new (
               "load-error",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, load_error),
               NULL, NULL,
               g_cclosure_user_marshal_VOID__STRING_POINTER,
               G_TYPE_NONE,
               2, G_TYPE_STRING, G_TYPE_POINTER);

     curl_module_signals[URI_CHANGED_SIGNAL] = g_signal_new (
               "uri-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     curl_module_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
               "status-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlModuleClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     gobject_class->constructor  = curl_module_constructor;
     gobject_class->finalize     = curl_module_finalize;
     class->instance = NULL;
}

static void curl_module_init (CurlModule *self)
{
     CurlModulePrivate *priv = CURL_MODULE_GET_PRIVATE (self);

     self->load_status = CURL_LOAD_PROVISIONAL;
     self->uri = NULL;
     self->status = NULL;
     self->progress = 0;

     priv->context = NULL;
     priv->content = g_string_new ("");
}

static GObject *curl_module_constructor (GType type, guint n_properties, GObjectConstructParam *properties)
{
     CurlModuleClass *class = NULL;

     class = g_type_class_peek (TYPE_CURL_MODULE);
     if (class->instance == NULL)
     {
          class->instance = G_OBJECT_CLASS (curl_module_parent_class)->constructor (type, n_properties, properties);
     }
     else
     {
          class->instance = g_object_ref (class->instance);
     }

     return class->instance;
}

static void curl_module_finalize (GObject *self)
{
     CurlModuleClass *class = NULL;

     class = g_type_class_peek (TYPE_CURL_MODULE);
     class->instance = NULL;
     G_OBJECT_CLASS (curl_module_parent_class)->finalize (self);
}

/*!
  \fn CurlModule *curl_module_new (void)
  \brief Create a new CurlModule object

  \return The new CURL context
 */
CurlModule *curl_module_new (void)
{
     CurlModule *obj = g_object_new (TYPE_CURL_MODULE, NULL);

     return obj;
}

static size_t curl_module_fn_write (void *ptr, size_t size, size_t nmemb, void *stream)
{
     CurlModule *obj = (CurlModule *) stream;
     CurlModulePrivate *priv = CURL_MODULE_GET_PRIVATE (obj);
     GString *content = priv->content;

     priv->content = g_string_append (content, (gchar *) ptr);

     g_free (obj->status);
     obj->status = g_strdup_printf ("Transfering data from %s ...", obj->uri);
     g_signal_emit (
          G_OBJECT (obj),
          curl_module_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );

     return (size_t) (size * nmemb);
}

static int curl_module_fn_progress (void *ptr, double dl_total, double dl_now, double up_total, double up_now)
{
     CurlModule *obj = (CurlModule *) ptr;
     obj->progress = (gfloat) (dl_now * 100.0 / dl_total);
     g_signal_emit (
          G_OBJECT (obj),
          curl_module_signals[LOAD_PROGRESS_CHANGED_SIGNAL],
          0, obj->progress
     );
     return 0;
}

static void *curl_module_load_uri_thread (void *data)
{
     CurlModule *obj = (CurlModule *) data;
     CurlModulePrivate *priv = CURL_MODULE_GET_PRIVATE (obj);
     GError *error = NULL;

     priv->context = curl_easy_init ();
     if (priv->context)
     {
          g_free (obj->status);
          obj->status = g_strdup_printf ("Connecting to %s ...", obj->uri);
          g_signal_emit (
               G_OBJECT (obj),
               curl_module_signals[STATUS_CHANGED_SIGNAL],
               0, obj->status
          );

          curl_easy_setopt (priv->context, CURLOPT_URL, obj->uri);

          curl_easy_setopt (priv->context, CURLOPT_WRITEFUNCTION, curl_module_fn_write);
          curl_easy_setopt (priv->context, CURLOPT_WRITEDATA, obj);

          curl_easy_setopt (priv->context, CURLOPT_NOPROGRESS, 0L);
          curl_easy_setopt (priv->context, CURLOPT_PROGRESSFUNCTION, curl_module_fn_progress);
          curl_easy_setopt (priv->context, CURLOPT_PROGRESSDATA, obj);

          obj->load_status = CURL_LOAD_STARTED;
          g_signal_emit (
               G_OBJECT (obj),
               curl_module_signals[LOAD_STARTED_SIGNAL],
               0
          );

          priv->result = curl_easy_perform (priv->context);
          if (priv->result != CURLE_OK)
          {
               g_free (obj->status);
               obj->status = g_strdup (curl_easy_strerror (priv->result));
               obj->load_status = CURL_LOAD_FAILED;

               error = g_error_new (CURL_MODULE_ERROR, CURL_MODULE_ERROR_CURL_ERROR, "Error, cannot access to '%s': %s.", obj->uri, obj->status);

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_module_signals[LOAD_ERROR_SIGNAL],
                    0, obj->uri, error
               );

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_module_signals[STATUS_CHANGED_SIGNAL],
                    0, obj->status
               );
          }
          else
          {
               obj->load_status = CURL_LOAD_FINISHED;
               g_signal_emit (
                    G_OBJECT (obj),
                    curl_module_signals[LOAD_FINISHED_SIGNAL],
                    0
               );

               g_free (obj->status);
               obj->status = g_strdup (obj->uri);

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_module_signals[STATUS_CHANGED_SIGNAL],
                    0, obj->status
               );
          }

          curl_easy_cleanup (priv->context);
          priv->context = NULL;
     }

     return NULL;
}

/*!
  \fn void curl_module_load_uri (CurlModule *obj, gchar *uri)
  \brief Load a new URI into the CURL context

  \param obj A CurlModule object
  \param uri The URI to load
 */
void curl_module_load_uri (CurlModule *obj, gchar *uri)
{
     g_free (obj->uri);
     obj->uri = g_strdup (uri);

     obj->load_status = CURL_LOAD_COMMITTED;
     g_signal_emit (
          G_OBJECT (obj),
          curl_module_signals[LOAD_COMMITTED_SIGNAL],
          0
     );

     if (!g_thread_create (&curl_module_load_uri_thread, obj, FALSE, NULL) != 0)
     {
          GError *error = NULL;

          error = g_error_new (CURL_MODULE_ERROR, CURL_MODULE_ERROR_THREAD, "Error, cannot initialize thread");

          g_signal_emit (
               G_OBJECT (obj),
               curl_module_signals[LOAD_ERROR_SIGNAL],
               0, uri, error
          );
     }
}

/*!
  \fn const gchar *curl_module_get_uri (CurlModule *obj)
  \brief Get the URI loaded by CURL
  \return The loaded URI
 */
const gchar *curl_module_get_uri (CurlModule *obj)
{
     return (const gchar *) obj->uri;
}

/*!
  \fn const gchar *curl_module_get_status (CurlModule *obj)
  \brief Get the status message
  \return The status of the loaded URI
 */
const gchar *curl_module_get_status (CurlModule *obj)
{
     return (const gchar *) obj->status;
}

/*!
  \fn CurlLoadStatus curl_module_get_load_status (CurlModule *obj)
  \brief Get the load information
  \return Load information of the loaded URI
 */
CurlLoadStatus curl_module_get_load_status (CurlModule *obj)
{
     return obj->load_status;
}

/*!
  \fn const gchar *curl_module_get_content (CurlModule *obj)
  \brief Get the content downloaded by CURL
  \return A string containing the content or NULL if the load is not finished
 */
const gchar *curl_module_get_content (CurlModule *obj)
{
     gchar *ret = NULL;

     if (obj->load_status == CURL_LOAD_FINISHED)
     {
          CurlModulePrivate *priv = CURL_MODULE_GET_PRIVATE (obj);

          ret = priv->content->str;
     }

     return (const gchar *) ret;
}

