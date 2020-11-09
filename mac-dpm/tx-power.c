#include "net/mac/mac-dpm/tx-power.h"

#include "net/netstack.h"

#include "sys/log.h"
#define LOG_MODULE "TX POWER"
#define LOG_LEVEL LOG_LEVEL_MAC

#ifdef ENERGEST_CONF_ON
#include "sys/energest.h"

static inline energest_type_t get_energest_type_for_pa_level(int8_t const pa_level) {
    switch (pa_level) {
        case 0:
        return ENERGEST_TYPE_TX_0;

        case -1:
        return ENERGEST_TYPE_TX_1;

        case -3:
        return ENERGEST_TYPE_TX_3;

        case -5:
        return ENERGEST_TYPE_TX_5;

        case -7:
        return ENERGEST_TYPE_TX_7;

        case -10:
        return ENERGEST_TYPE_TX_10;

        case -15:
        return ENERGEST_TYPE_TX_15;

        case -25:
        return ENERGEST_TYPE_TX_25;

        default:
        LOG_ERR("Invalid PA level: %d\n", pa_level);
        return ENERGEST_TYPE_MAX;

    }
}

#endif /* ENERGEST_CONF_ON */



radio_value_t
get_tx_power(void) {
    radio_value_t tx_power;
    radio_result_t result = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &tx_power);
    if (result != RADIO_RESULT_OK) {
        LOG_ERR("Problem fetching the TX power: %d\n", result);
    }    

    return tx_power;
}


radio_value_t
get_max_tx_power(void) {
    radio_value_t max_tx_power;
    radio_result_t result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &max_tx_power);
    if (result != RADIO_RESULT_OK) {
        LOG_ERR("Problem fetching the maximum TX power: %d\n", result);
    }    

    return max_tx_power;
}


radio_value_t
get_min_tx_power(void) {
    radio_value_t min_tx_power;
    radio_result_t result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &min_tx_power);
    if (result != RADIO_RESULT_OK) {
        LOG_ERR("Problem fetching the minimum TX power: %d\n", result);
    }

    return min_tx_power;
}


void
set_tx_power(radio_value_t const tx_power) {
#ifdef ENERGEST_CONF_ON
    radio_value_t current_tx_power = get_tx_power();
    energest_type_t current_energest_type = get_energest_type_for_pa_level(current_tx_power);
#endif /* ENERGEST_CONF_ON */

    radio_result_t result = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, tx_power);
    if (result != RADIO_RESULT_OK) {
        LOG_ERR("Problem setting the TX power: %d\n", result);
    }

#ifdef ENERGEST_CONF_ON
    radio_value_t new_tx_power = get_tx_power();
    energest_type_t new_energest_type = get_energest_type_for_pa_level(new_tx_power);
    ENERGEST_SWITCH(current_energest_type, new_energest_type);
#endif /* ENERGEST_CONF_ON */
}


void
reset_tx_power(void) {
    radio_value_t max_tx_power = get_max_tx_power();
    set_tx_power(max_tx_power);
}
