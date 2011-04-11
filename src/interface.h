#ifndef __INTERFACE_H
#define __INTERFACE_H

/*!
 * \defgroup interface Graphical User Interface
 * GUI's functions.
 *
 * \todo LUA bindings
 * \todo LUA functions to connect signal to objects.
 *
 * @{
 */

GtkWidget *interface_init (void);
void interface_show (GtkWidget *window);

/*! @} */

#endif /* __INTERFACE_H */
