#ifndef __MODULES_H
#define __MODULES_H

#include <gtk/gtk.h>
#include <gmodule.h>

typedef struct ModulesCallbacksList
{
     GModule *module;
     char *modulename;
     guint module_id;

     void (*init) (void);
     void (*unload) (void);

     GtkWidget * (*webview_new) (void);

     void (*call) (const char *, ...);
} ModulesCallbacksList;

gboolean modules_init (void);
guint modules_load (const char *filename);

#endif /* __MODULES_H */
