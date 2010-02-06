#ifndef __CLASS_CURL_MODULE_H
#define __CLASS_CURL_MODULE_H

#include <glib-object.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#define TYPE_CURL_MODULE                (module_curl_get_type ())
#define CURL_MODULE(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CURL_MODULE, CurlModule))
#define CURL_MODULE_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_CURL_MODULE, CurlModuleClass))
#define MODULE_IS_CURL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CURL_MODULE))
#define MODULE_CLASS_IS_CURL(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_CURL_MODULE))
#define CURL_MODULE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CURL_MODULE, CurlModuleClass))

typedef enum
{
     CURL_LOAD_PROVISIONAL,
     CURL_LOAD_COMMITTED,
     CURL_LOAD_STARTED,
     CURL_LOAD_FINISHED,
     CURL_LOAD_FAILED
} CurlLoadStatus;

typedef struct _CurlModule CurlModule;
typedef struct _CurlModuleClass CurlModuleClass;

struct _CurlModule
{
     GObject parent;

     CURL *context;
     CURLcode result;
     GString *content;

     gchar *uri;
     gchar *title;
     gchar *status;

     CurlLoadStatus load_status;
};

struct _CurlModuleClass
{
     GObjectClass parent;

     /*< private >*/
     GObject *instance;
};

GType curl_module_get_type (void);

#endif /* __CLASS_CURL_MODULE_H */
