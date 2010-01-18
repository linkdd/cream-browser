#include "local.h"

void signal_handler (int signum)
{
     switch (signum)
     {
          case SIGSEGV:
               fprintf (stderr, "Cream: Segmentation fault\n");
               cream_release (EXIT_FAILURE);
               break;
     }
}

void cb_cream_destroy (GtkWidget *emit, gpointer data)
{
     cream_release (EXIT_SUCCESS);
}
