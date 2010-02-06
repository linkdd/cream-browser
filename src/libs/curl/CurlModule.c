#include "CurlModule.h"

static GObject *curl_module_constructor (GType type, guint n_properties, GObjectConstructParam *properties);
static void curl_module_finalize (GObject *self);

G_DEFINE_TYPE (CurlModule, curl_module, G_TYPE_OBJECT)

static void curl_module_class_init (CurlModuleClass *class)
{
     GObjectClass *gobject_class = G_OBJECT_CLASS (class);

     gobject_class->constructor = curl_module_constructor;
     gobject_class->finalize    = curl_module_finalize;
     class->instance = NULL;
}

static void curl_module_init (CurlModule *self)
{
     g_return_if_fail (self != NULL);
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

