#ifndef MAC_DPM_H_
#define MAC_DPM_H_

#include "net/mac/mac-dpm/mac-dpm-nbr.h"

#include <stdbool.h>

#define PROCESS_EVENT_MAC_DPM_RECEIVED_ACK 127



/* States of the MAC DPM state machine */
typedef enum stage {
    DISCOVERY = 0,
    MINIMIZATION = 1,
    UPDATING = 2,
    VERIFICATION = 3,
} stage_t;


typedef struct state {
    stage_t stage;
    unsigned int nbr_count;
    mac_dpm_neighbor_t * weakest_nbr;
    bool nbr_count_changed;
    bool weakest_nbr_changed;
} state_t;


/* Type definitions */
typedef enum dpm_data_object_type_t {

    /* Broadcasts a message requiring link-layer addresses from all nodes in the vicinity. */
    MAC_DPM_BROADCAST = 0,

    /* Response to the broadcast message which provides the node's link-layer address. */
    MAC_DPM_RESPONSE = 1,

    /* Requests a single neighbor for status. Used in minimization for alive checks. */
    MAC_DPM_STATUS_REQUEST = 2,

    /* Response to a status request. Used in minimization for alive checks. */
    MAC_DPM_STATUS_RESPONSE = 3,

    /* Secondary beacon round. Used in updating phase. */
    MAC_DPM_UPDATING = 4,

    /* Response to secondary beacon round. Used in updating phase. */
    MAC_DPM_UPDATING_RESPONSE = 5,
} dpm_data_object_type_t;


/* Logical representation of the DPM data object message. */
typedef struct dpm_data_object {
    dpm_data_object_type_t dpm_data_object_type;
} dpm_data_object_t;

#define MAC_DPM_OBJECT_SIZE sizeof(dpm_data_object_t)


void init_mac_dpm(void);


/* Notifies the minimization process about received acknowledgment from the specified address. */
void notify_minimization_about_ack(linkaddr_t const *);



/* Returns a string representation of DPM data object type. */
char const * dpm_data_object_type_to_str(dpm_data_object_type_t);


#endif /* MAC_DPM_H_ */
