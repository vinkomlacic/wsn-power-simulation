#include "net/linkaddr.h"

#include <stdbool.h>


/* Starts minimizing the TX power with the link on the specified address. */
void init_minimization_process(linkaddr_t const *);


/* Notifies minimization process that an ACK has been received. */
void notify_minimization_about_ack(linkaddr_t const *);


/* Returns true if minimization is running. */
bool minimization_is_running(void);


/* Exits minimization process. */
void stop_minimization_process(void);
