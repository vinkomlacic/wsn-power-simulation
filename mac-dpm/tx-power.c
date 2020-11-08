#include "net/mac/mac-dpm/tx-power.h"

#include "net/netstack.h"

#include "sys/log.h"
#define LOG_MODULE "TX POWER"
#define LOG_LEVEL LOG_LEVEL_MAC


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


void
set_tx_power(radio_value_t const tx_power) {
    radio_result_t result = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, tx_power);
    if (result != RADIO_RESULT_OK) {
        LOG_ERR("Problem setting the TX power: %d\n", result);
    }
}


void
reset_tx_power(void) {
    radio_value_t max_tx_power = get_max_tx_power();
    set_tx_power(max_tx_power);
}
