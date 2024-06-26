#include "net/linkaddr.h"
#include "net/netstack.h"

#include "net/mac/mac-dpm/mac-dpm.h"
#include "net/mac/mac-dpm/mac-dpm-conf.h"
#include "net/mac/mac-dpm/mac-dpm-output.h"
#include "net/mac/mac-dpm/tx-power.h"

#include "sys/log.h"
#define LOG_MODULE "MAC DPM MINIMIZATION"
#define LOG_LEVEL LOG_LEVEL_MAC_DPM_MINIMIZATION

#include <stdlib.h>


/********************************************************************/
PROCESS(mac_dpm_minimization_process, "MAC DPM minimization process");
/********************************************************************/


linkaddr_t const * WEAKEST_NBR = NULL;
#ifdef MAC_DPM_MINIMIZATION_SEQUENCE
int8_t MINIMIZATION_SEQUENCE[] = MAC_DPM_MINIMIZATION_SEQUENCE;
size_t MINIMIZATION_SEQUENCE_LEN = sizeof(MINIMIZATION_SEQUENCE);
#else
int8_t * MINIMIZATION_SEQUENCE;
size_t MINIMIZATION_SEQUENCE_LEN;
#endif /* MAC_DPM_MINIMIZATION_SEQUENCE */


#ifndef MAC_DPM_MINIMIZATION_SEQUENCE
static void
initialize_minimization_sequence(void) {

    radio_value_t min_tx_power = get_min_tx_power();
    radio_value_t max_tx_power = get_max_tx_power();
    MINIMIZATION_SEQUENCE_LEN = abs(max_tx_power - min_tx_power) + 1;
    MINIMIZATION_SEQUENCE = (int8_t *) malloc(MINIMIZATION_SEQUENCE_LEN * sizeof(int8_t));

    int i;
    for (i = max_tx_power; i <= min_tx_power; i--) {
        MINIMIZATION_SEQUENCE[i] = i;
    }
}
#endif /* MAC_DPM_MINIMIZATION_SEQUENCE */


void
init_minimization_process(linkaddr_t const * weakest_nbr_address) {
    WEAKEST_NBR = weakest_nbr_address;
#ifndef MAC_DPM_MINIMIZATION_SEQUENCE
    initialize_minimization_sequence();
#endif /* MAC_DPM_MINIMIZATION_SEQUENCE */
    process_start(&mac_dpm_minimization_process, NULL);
}


void
notify_minimization_about_ack(linkaddr_t const * const source_addr) {
    process_post(&mac_dpm_minimization_process, PROCESS_EVENT_MAC_DPM_RECEIVED_ACK, (void *) source_addr);
}


bool
minimization_is_running(void) {
    return process_is_running(&mac_dpm_minimization_process);
}


void
stop_minimization_process(void) {
    process_exit(&mac_dpm_minimization_process);
}


static bool
ack_received(process_event_t event, process_data_t data) {
    linkaddr_t const * const source_addr = (linkaddr_t const *) data; 

    if (event == PROCESS_EVENT_MAC_DPM_RECEIVED_ACK && linkaddr_cmp(source_addr, WEAKEST_NBR)) {
        LOG_INFO("Received ack from ");
        LOG_INFO_LLADDR(source_addr);
        LOG_INFO_("\n");
        return true;
    }

    return false;
}


PROCESS_THREAD(mac_dpm_minimization_process, ev, data) {
    static radio_value_t tx_power;
    static struct etimer timer;
    static bool connection_alive = true;
    static int power_level = 0;
    
    PROCESS_BEGIN();

    etimer_set(&timer, CLOCK_SECOND * MAC_DPM_WAIT_FOR_ACK_PERIOD_SECONDS);
    reset_tx_power();
    tx_power = MINIMIZATION_SEQUENCE[power_level];
        
    /* Minimize the power until connection is lost. */
    while (connection_alive || (power_level == MINIMIZATION_SEQUENCE_LEN - 1)) {
        LOG_DBG("TX POWER: %d\n", tx_power);
        set_tx_power(tx_power);
        tx_power = MINIMIZATION_SEQUENCE[++power_level];
        send_mac_dpm_status_request(WEAKEST_NBR);

        while (true) {
            PROCESS_YIELD();

            if (ack_received(ev, data)) {
                /* Connection is still alive. Break from event processing loop and go to TX power decrementing loop. */
                etimer_reset(&timer);
                break;
            }

            if (etimer_expired(&timer)) {
                /* Assuming we lost the connection */
                connection_alive = false;
                etimer_reset(&timer);
                break;
            }

            LOG_DBG("Ignoring event: %d\n", ev);
        }
    }
    LOG_INFO("Lost connection at %d TX power level. Incrementing to get it back.\n", get_tx_power());

    /* Increment the power until connection is restored. */
    while (!connection_alive || power_level == 0) {
        LOG_DBG("TX POWER: %d\n", tx_power);
        set_tx_power(tx_power);
        tx_power = MINIMIZATION_SEQUENCE[--power_level];
        send_mac_dpm_status_request(WEAKEST_NBR);

        while (true) {
            PROCESS_YIELD();
            LOG_INFO("Received event: %d\n", ev);

            if (etimer_expired(&timer)) {
                /* Assuming connection still lost. Break from event processing loop and go to TX power incrementing loop. */
                etimer_reset(&timer);
                break;
            }

            if (ack_received(ev, data)) {
                /* Connection restored. End incrementing TX power. */
                connection_alive = true;
                break;
            }

            LOG_DBG("Ignoring event: %d\n", ev);
        }
    }
    LOG_INFO("Connection restored at minimum level of %d.\n", get_tx_power());

    PROCESS_END();
}
