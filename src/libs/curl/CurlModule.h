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
  \file CurlModule.h
  \brief Curl integration
  \author David Delassus
 */

#ifndef __CLASS_CURL_MODULE_H
#define __CLASS_CURL_MODULE_H

#include <glib-object.h>
#include <gnet.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#define TYPE_CURL_MODULE                (curl_module_get_type ())
#define CURL_MODULE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CURL_MODULE, CurlModule))
#define CURL_MODULE_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_CURL_MODULE, CurlModuleClass))
#define MODULE_IS_CURL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CURL_MODULE))
#define MODULE_CLASS_IS_CURL(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_CURL_MODULE))
#define CURL_MODULE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CURL_MODULE, CurlModuleClass))

/*!
  \enum CurlLoadStatus
  \brief Curl load information
 */
typedef enum
{
     CURL_LOAD_PROVISIONAL,   /*!< Before any loading */
     CURL_LOAD_COMMITTED,     /*!< A load is requested */
     CURL_LOAD_STARTED,       /*!< The load has been started */
     CURL_LOAD_FINISHED,      /*!< The load has been finished */
     CURL_LOAD_FAILED         /*!< The load has failed */
} CurlLoadStatus;

typedef struct _CurlModule CurlModule;
typedef struct _CurlModuleClass CurlModuleClass;

/*! \struct _CurlModule */
struct _CurlModule
{
     GObject parent;               /*!< Parent instance */

     GURI *uri;                    /*!< URI to load */
     gchar *status;                /*!< Status of the loaded URI */

     gfloat progress;              /*!< Loading's progress */
     CurlLoadStatus load_status;   /*!< Loading status */
};

/*! \struct _CurlModuleClass */
struct _CurlModuleClass
{
     GObjectClass parent;     /*!< Parant class */

     void (*load_committed) (CurlModule *obj);         /*!< The "load-committed" signal is emitted when the user requests to load a new URI */
     void (*load_started) (CurlModule *obj);           /*!< The "load-started" signal is emitted when the loading start */

     void (*load_progress_changed) (CurlModule *obj, gfloat progress);
     /*!< The "load-progress-changed" signal is emitted when CURL receive new data
       \param progress Percentage of the page's loading
      */

     void (*load_finished) (CurlModule *obj);          /*!< The "load-finished" signal is emitted when we can use the data completly downloaded by CURL */

     void (*load_error) (CurlModule *obj, gchar *uri, gpointer error);
     /*!< The "load-error" signal is emitted when CURL can't download data
       \param uri URI which failed
       \param error A GError which descriebe the error
      */

     void (*uri_changed) (CurlModule *obj, gchar *uri);
     /*!< The "uri-changed" signal is emitted when CURL request the loading of a new URI
       \param uri The new URI
      */

     void (*status_changed) (CurlModule *obj, gchar *status);
     /*!< The "status-changed" signal is emitted when the page's status change.
       \param status The status text (can be printed in a statusbar)
      */

     /*< private >*/
     GObject *instance;  /*!< Associated instance */
};

GType curl_module_get_type (void);

CurlModule *curl_module_new (void);
void curl_module_load_uri (CurlModule *obj, gchar *uri);

const gchar *curl_module_get_uri (CurlModule *obj);
GURI *curl_module_get_guri (CurlModule *obj);
const gchar *curl_module_get_status (CurlModule *obj);
CurlLoadStatus curl_module_get_load_status (CurlModule *obj);
const gchar *curl_module_get_content (CurlModule *obj);

#endif /* __CLASS_CURL_MODULE_H */
