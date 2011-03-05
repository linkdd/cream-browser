#include "config.h"
#include "errors.h"

static guint domain = 0;

void libconfig_init (void)
{
     domain = error_domain_register ("config");
}
