#include "local.h"

static guint domain = 0;

void config_init (void)
{
     domain = error_domain_register ("config");
}
