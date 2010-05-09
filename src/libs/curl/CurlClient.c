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

/*!
  @defgroup curl Curl integration
  @ingroup libcream
  @brief Object which define a Curl instance

  @{
 */

#include "CurlClient.h"
#include <marshal.h>
#include <string.h>

#define CURL_CLIENT_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_CURL_CLIENT, CurlClientPrivate))

typedef struct _CurlClientPrivate CurlClientPrivate;

/*!
  \struct _CurlClientPrivate
  \brief CurlClient private data
 */
struct _CurlClientPrivate
{
     CURL *context;      /*!< Curl object */
     CURLcode result;    /*!< Result of CURL loading */
     GString *content;   /*!< Content downloaded */
};

static GObject *curl_client_constructor (GType type, guint n_properties, GObjectConstructParam *properties);
static void curl_client_finalize (GObject *self);

enum
{
     LOAD_COMMITTED_SIGNAL,
     LOAD_STARTED_SIGNAL,
     LOAD_PROGRESS_CHANGED_SIGNAL,
     LOAD_FINISHED_SIGNAL,
     LOAD_ERROR_SIGNAL,

     URI_CHANGED_SIGNAL,
     STATUS_CHANGED_SIGNAL,
     DOWNLOAD_REQUESTED_SIGNAL,

     NB_SIGNALS
};

/*!
  \var static guint curl_client_signals[NB_SIGNALS]
  \brief CurlClient signals definition
 */
static guint curl_client_signals[NB_SIGNALS] = {0};

/*!
  \enum CurlClientError
  \brief CurlClient errors definition
 */
typedef enum
{
     CURL_CLIENT_ERROR_CURL_INIT,
     CURL_CLIENT_ERROR_CURL_ERROR,
     CURL_CLIENT_ERROR_THREAD,
     CURL_CLIENT_ERROR_FAILED
} CurlClientError;

#define CURL_CLIENT_ERROR     (curl_client_error_quark ())

static GQuark curl_client_error_quark (void)
{
     static GQuark curl_client_error = 0;

     if (!curl_client_error)
     {
          curl_client_error = g_quark_from_string ("CURL_CLIENT_ERROR");
     }

     return curl_client_error;
}

/* Define object */
G_DEFINE_TYPE (CurlClient, curl_client, G_TYPE_OBJECT)

static void curl_client_class_init (CurlClientClass *class)
{
     GObjectClass *gobject_class = G_OBJECT_CLASS (class);

     g_type_class_add_private (class, sizeof (CurlClientPrivate));

     curl_client_signals[LOAD_COMMITTED_SIGNAL] = g_signal_new (
               "load-committed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, load_committed),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_client_signals[LOAD_STARTED_SIGNAL] = g_signal_new (
               "load-started",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, load_started),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_client_signals[LOAD_PROGRESS_CHANGED_SIGNAL] = g_signal_new (
               "load-progress-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, load_progress_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__FLOAT,
               G_TYPE_NONE,
               1, G_TYPE_FLOAT);

     curl_client_signals[LOAD_FINISHED_SIGNAL] = g_signal_new (
               "load-finished",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, load_finished),
               NULL, NULL,
               g_cclosure_marshal_VOID__VOID,
               G_TYPE_NONE,
               0, G_TYPE_NONE);

     curl_client_signals[LOAD_ERROR_SIGNAL] = g_signal_new (
               "load-error",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, load_error),
               NULL, NULL,
               g_cclosure_user_marshal_VOID__STRING_POINTER,
               G_TYPE_NONE,
               2, G_TYPE_STRING, G_TYPE_POINTER);

     curl_client_signals[URI_CHANGED_SIGNAL] = g_signal_new (
               "uri-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, uri_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     curl_client_signals[STATUS_CHANGED_SIGNAL] = g_signal_new (
               "status-changed",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, status_changed),
               NULL, NULL,
               g_cclosure_marshal_VOID__STRING,
               G_TYPE_NONE,
               1, G_TYPE_STRING);

     curl_client_signals[DOWNLOAD_REQUESTED_SIGNAL] = g_signal_new (
               "download-requested",
               G_TYPE_FROM_CLASS (class),
               G_SIGNAL_RUN_LAST,
               G_STRUCT_OFFSET (CurlClientClass, download_requested),
               NULL, NULL,
               g_cclosure_user_marshal_BOOLEAN__STRING,
               G_TYPE_BOOLEAN,
               1, G_TYPE_STRING);

     gobject_class->constructor  = curl_client_constructor;
     gobject_class->finalize     = curl_client_finalize;
     class->instance = NULL;
}

static void curl_client_init (CurlClient *self)
{
     CurlClientPrivate *priv = CURL_CLIENT_GET_PRIVATE (self);

     self->load_status = CURL_LOAD_PROVISIONAL;
     self->uri = NULL;
     self->status = NULL;
     self->progress = 0;

     priv->context = NULL;
     priv->content = g_string_new ("");
}

static GObject *curl_client_constructor (GType type, guint n_properties, GObjectConstructParam *properties)
{
     CurlClientClass *class = NULL;

     class = g_type_class_peek (TYPE_CURL_CLIENT);
     if (class->instance == NULL)
     {
          class->instance = G_OBJECT_CLASS (curl_client_parent_class)->constructor (type, n_properties, properties);
     }
     else
     {
          class->instance = g_object_ref (class->instance);
     }

     return class->instance;
}

static void curl_client_finalize (GObject *self)
{
     CurlClientClass *class = NULL;

     class = g_type_class_peek (TYPE_CURL_CLIENT);
     class->instance = NULL;
     G_OBJECT_CLASS (curl_client_parent_class)->finalize (self);
}

/*!
  \fn CurlClient *curl_client_new (void)
  \brief Create a new CurlClient object

  \return The new CURL context
 */
CurlClient *curl_client_new (void)
{
     CurlClient *obj = g_object_new (TYPE_CURL_CLIENT, NULL);

     return obj;
}

static size_t curl_client_fn_write (void *ptr, size_t size, size_t nmemb, void *stream)
{
     CurlClient *obj = (CurlClient *) stream;
     CurlClientPrivate *priv = CURL_CLIENT_GET_PRIVATE (obj);
     GString *content = priv->content;

     priv->content = g_string_append (content, (gchar *) ptr);

     g_free (obj->status);
     obj->status = g_strdup_printf ("Transfering data from %s ...", obj->uri->hostname);
     g_signal_emit (
          G_OBJECT (obj),
          curl_client_signals[STATUS_CHANGED_SIGNAL],
          0, obj->status
     );

     return (size_t) (size * nmemb);
}

static int curl_client_fn_progress (void *ptr, double dl_total, double dl_now, double up_total, double up_now)
{
     CurlClient *obj = (CurlClient *) ptr;
     obj->progress = (gfloat) (dl_now * 100.0 / dl_total);
     g_signal_emit (
          G_OBJECT (obj),
          curl_client_signals[LOAD_PROGRESS_CHANGED_SIGNAL],
          0, obj->progress
     );
     return 0;
}

static void *curl_client_load_uri_thread (void *data)
{
     CurlClient *obj = (CurlClient *) data;
     CurlClientPrivate *priv = CURL_CLIENT_GET_PRIVATE (obj);
     GError *error = NULL;

     priv->context = curl_easy_init ();
     priv->content = g_string_new ("");
     if (priv->context)
     {
          gchar *content_type = NULL;

          g_free (obj->status);
          obj->status = g_strdup_printf ("Connecting to %s ...", obj->uri->hostname);
          g_signal_emit (
               G_OBJECT (obj),
               curl_client_signals[STATUS_CHANGED_SIGNAL],
               0, obj->status
          );

          curl_easy_setopt (priv->context, CURLOPT_URL, gnet_uri_get_string (obj->uri));

          curl_easy_setopt (priv->context, CURLOPT_WRITEFUNCTION, curl_client_fn_write);
          curl_easy_setopt (priv->context, CURLOPT_WRITEDATA, obj);

          curl_easy_setopt (priv->context, CURLOPT_NOPROGRESS, 0L);
          curl_easy_setopt (priv->context, CURLOPT_PROGRESSFUNCTION, curl_client_fn_progress);
          curl_easy_setopt (priv->context, CURLOPT_PROGRESSDATA, obj);

          obj->load_status = CURL_LOAD_STARTED;
          g_signal_emit (
               G_OBJECT (obj),
               curl_client_signals[LOAD_STARTED_SIGNAL],
               0
          );

          priv->result = curl_easy_perform (priv->context);
          if (priv->result != CURLE_OK)
          {
               g_free (obj->status);
               obj->status = g_strdup (curl_easy_strerror (priv->result));
               obj->load_status = CURL_LOAD_FAILED;

               error = g_error_new (CURL_CLIENT_ERROR, CURL_CLIENT_ERROR_CURL_ERROR, "Error, cannot access to %s%s : %s.",
                         obj->uri->hostname,
                         (obj->uri->path ? g_strconcat (" - ", obj->uri->path, NULL) : ""),
                         obj->status);

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_client_signals[LOAD_ERROR_SIGNAL],
                    0, gnet_uri_get_string (obj->uri), error
               );

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_client_signals[STATUS_CHANGED_SIGNAL],
                    0, obj->status
               );
          }
          else
          {
               priv->result = curl_easy_getinfo (priv->content, CURLINFO_CONTENT_TYPE, &content_type);
               if (CURLE_OK == priv->result && content_type)
               {
                    gboolean ret = FALSE;

                    g_signal_emit (
                         G_OBJECT (obj),
                         curl_client_signals[DOWNLOAD_REQUESTED_SIGNAL],
                         0, gnet_uri_get_string (obj->uri),
                         &ret
                    );

                    if (!ret)
                         printf ("Download requested: %s (%s)\n", gnet_uri_get_string (obj->uri), content_type);
               }

               obj->load_status = CURL_LOAD_FINISHED;
               g_signal_emit (
                    G_OBJECT (obj),
                    curl_client_signals[LOAD_FINISHED_SIGNAL],
                    0
               );

               g_free (obj->status);
               obj->status = g_strdup (gnet_uri_get_string (obj->uri));

               g_signal_emit (
                    G_OBJECT (obj),
                    curl_client_signals[STATUS_CHANGED_SIGNAL],
                    0, obj->status
               );
          }

          curl_easy_cleanup (priv->context);
          priv->context = NULL;
     }

     return NULL;
}

/*!
  \fn void curl_client_load_uri (CurlClient *obj, gchar *uri)
  \brief Load a new URI into the CURL context

  \param obj A CurlClient object
  \param uri The URI to load
 */
void curl_client_load_uri (CurlClient *obj, gchar *uri)
{
     obj->uri = gnet_uri_new (uri);

     obj->load_status = CURL_LOAD_COMMITTED;
     g_signal_emit (
          G_OBJECT (obj),
          curl_client_signals[LOAD_COMMITTED_SIGNAL],
          0
     );

     if (!g_thread_create (&curl_client_load_uri_thread, obj, FALSE, NULL) != 0)
     {
          GError *error = NULL;

          error = g_error_new (CURL_CLIENT_ERROR, CURL_CLIENT_ERROR_THREAD, "Error, cannot initialize thread");

          g_signal_emit (
               G_OBJECT (obj),
               curl_client_signals[LOAD_ERROR_SIGNAL],
               0, uri, error
          );
     }
}

/*!
  \fn const gchar *curl_client_get_uri (CurlClient *obj)
  \brief Get the URI loaded by CURL
  \return The loaded URI
 */
const gchar *curl_client_get_uri (CurlClient *obj)
{
     return (const gchar *) gnet_uri_get_string (obj->uri);
}

/*!
  \fn GURI *curl_client_get_guri (CurlClient *obj)
  \brief Get the GURI object which represent the URI loaded by CURL
  \return A GURI object
 */
GURI *curl_client_get_guri (CurlClient *obj)
{
     return obj->uri;
}

/*!
  \fn const gchar *curl_client_get_status (CurlClient *obj)
  \brief Get the status message
  \return The status of the loaded URI
 */
const gchar *curl_client_get_status (CurlClient *obj)
{
     return (const gchar *) obj->status;
}

/*!
  \fn CurlLoadStatus curl_client_get_load_status (CurlClient *obj)
  \brief Get the load information
  \return Load information of the loaded URI
 */
CurlLoadStatus curl_client_get_load_status (CurlClient *obj)
{
     return obj->load_status;
}

/*!
  \fn const gchar *curl_client_get_content (CurlClient *obj)
  \brief Get the content downloaded by CURL
  \return A string containing the content or NULL if the load is not finished
 */
const gchar *curl_client_get_content (CurlClient *obj)
{
     gchar *ret = NULL;

     if (obj->load_status == CURL_LOAD_FINISHED)
     {
          CurlClientPrivate *priv = CURL_CLIENT_GET_PRIVATE (obj);

          ret = priv->content->str;
     }

     return (const gchar *) ret;
}

/*! @} */
