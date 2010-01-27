#ifndef __CLASS_FAVICON_H
#define __CLASS_FAVICON_H

#include <gtk/gtk.h>

typedef struct _Favicon Favicon;

struct _Favicon
{
     gchar *uri;
     gchar *file;
     GdkPixbuf *ico;
     GtkWidget *img;

     GHashTable *memory;
     gchar *cache_path;
};

Favicon *favicon_new (const gchar *uri);
void favicon_destroy (Favicon *obj);

#endif /* __CLASS_FAVICON_H */
