#ifndef __LOCAL_H
#define __LOCAL_H

#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <webkit/webkit.h>

#include <CreamView.h>

#include "structs.h"
#include "interface.h"
#include "callbacks.h"

#define CREAM_FILE(path)      g_build_filename (g_get_home_dir (), ".cream-browser", path, NULL)

extern struct global_t global;

gboolean cream_init (int *argc, char ***argv, GError **error);
void cream_release (int exit_code);

#endif /* __LOCAL_H */
