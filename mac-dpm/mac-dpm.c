#include "net/mac/mac-dpm/mac-dpm.h"
#include "net/mac/mac-dpm/mac-dpm-nbr.h"
#include "net/mac/mac-dpm/mac-dpm-conf.h"
#include "net/mac/mac-dpm/mac-dpm-output.h"
#include "net/mac/mac-dpm/mac-dpm-minimization.h"
#include "net/mac/mac-dpm/tx-power.h"

#include "net/netstack.h"
#include "net/linkaddr.h"

#include "sys/log.h"
#define LOG_MODULE "MAC DPM MAIN"
#define LOG_LEVEL LOG_LEVEL_MAC_DPM_MAIN

#include <stdlib.h>
#include <stdbool.h>


static state_t mac_dpm_state = {DISCOVERY, 0, NULL, false, false};


/********************************************************************/
PROCESS(mac_dpm_process, "MAC DPM process");
/********************************************************************/



static void
node_discovery(void) {
    LOG_INFO("DISCOVERY: Broadcasting a discovery beacon.\n");
    send_mac_dpm_beacon();
}


static void
tx_power_minimization(void) {
    LOG_INFO("MINIMIZATION: Minimizing TX power.\n");
    mac_dpm_state.nbr_count = count_neighbors();
    mac_dpm_state.weakest_nbr = min_link_stats_neighbor();
    init_minimization_process(&mac_dpm_state.weakest_nbr->linkaddr);
}


static void
updating_the_neighbor_table(void) {
    LOG_INFO("UPDATING: Broadcasting another beacon to check nodes around me.\n");
    send_mac_dpm_updating_request();
}


static void
verification(void) {
    LOG_INFO("VERIFICATION: Verifying state changes.\n");
    int nbr_count = count_verified_neighbors();
    mac_dpm_neighbor_t * const weakest_nbr = min_link_stats_neighbor();

    if (nbr_count != mac_dpm_state.nbr_count) {
        LOG_INFO("Neighbor count changed from %d to %d.\n", mac_dpm_state.nbr_count, nbr_count);
        mac_dpm_state.nbr_count = nbr_count;
        mac_dpm_state.nbr_count_changed = true;
        return;
    }

    revoke_nbr_verification();

    if (!linkaddr_cmp(&mac_dpm_state.weakest_nbr->linkaddr, &mac_dpm_state.weakest_nbr->linkaddr)) {
        mac_dpm_state.weakest_nbr = weakest_nbr;
        mac_dpm_state.weakest_nbr_changed = true;
        LOG_INFO("Weakes neighbor changed.\n");
    }
}

static void
next_state(void) {
    switch (mac_dpm_state.stage) {
        case DISCOVERY:
        mac_dpm_state.stage = MINIMIZATION;
        return;
        
        case MINIMIZATION:
        mac_dpm_state.stage = UPDATING;
        return;

        case UPDATING:
        mac_dpm_state.stage = VERIFICATION;
        return;

        case VERIFICATION:
        break;  
    }

    /* VERIFICATION */
    if (mac_dpm_state.nbr_count_changed) {    
        mac_dpm_state.nbr_count_changed = false;
        reset_tx_power();
        mac_dpm_state.stage = DISCOVERY;

    } else if (mac_dpm_state.weakest_nbr_changed) {
        mac_dpm_state.weakest_nbr_changed = false;
        mac_dpm_state.stage = MINIMIZATION;

    } else {
        mac_dpm_state.stage = UPDATING;
    }
}


static void
process_state(void) {
    switch (mac_dpm_state.stage) {
    case DISCOVERY:
    node_discovery();
    break;

    case MINIMIZATION:
    tx_power_minimization();
    break;

    case UPDATING:
    updating_the_neighbor_table();
    break;

    case VERIFICATION:
    verification();
    break;

    default:
    LOG_WARN("State has not been implemented yet.\n");
    }
}


void
init_mac_dpm(void) {
    LOG_INFO("Starting MAC DPM process.\n");
    process_start(&mac_dpm_process, NULL);
    init_mac_dpm_nbr();
}


char const * dpm_data_object_type_to_str(dpm_data_object_type_t const dpm_data_object_type) {
    switch (dpm_data_object_type) {
        case MAC_DPM_BROADCAST:
        return "MAC DPM beacon";

        case MAC_DPM_RESPONSE:
        return "MAC DPM beacon response";

        case MAC_DPM_STATUS_REQUEST:
        return "MAC DPM status request";

        case MAC_DPM_STATUS_RESPONSE:
        return "MAC DPM status response";

        case MAC_DPM_UPDATING:
        return "MAC DPM updating request";

        case MAC_DPM_UPDATING_RESPONSE:
        return "MAC DPM updating response";

        default:
        LOG_ERR("MAC DPM message type unsupported.\n");
        return "unknown message type";
    }
}


PROCESS_THREAD(mac_dpm_process, ev, data) {
    static struct etimer timer;

    PROCESS_BEGIN();
    etimer_set(&timer, CLOCK_SECOND * MAC_DPM_BEACON_INTERVAL_SECONDS);

    while (1) {
        process_state();
        next_state();
        print_nbr_table();

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
        /** If minimization phase is still running after the specified period exit it and log a warning. */
        if (minimization_is_running()) {
            stop_minimization_process();
            LOG_WARN("Minimization phase still running after the allocated time period. Killing the minimization process.\n");
            LOG_WARN("Please set MAC_DPM_WAIT_FOR_ACK_PERIOD_SECONDS so that it doesn't exceed this period.\n");
        }
        etimer_reset(&timer);
    }

    PROCESS_END();
}

