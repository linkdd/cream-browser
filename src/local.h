#ifndef __LOCAL_H
#define __LOCAL_H

#include <config_build.h>

#include <webkit/webkit.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* For sockets */
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#include "xdg.h"
#include "structs.h"

#include "errors.h"
#include "modules.h"
#include "lua.h"
#include "config.h"

extern struct global_t global;

void socket_init (void);

char *str_replace (const char *search, const char *replace, const char *string);

#endif /* __LOCAL_H */
