#include "local.h"

static guint domain = 0;

void libconfig_init (void)
{
     domain = error_domain_register ("config");
}
