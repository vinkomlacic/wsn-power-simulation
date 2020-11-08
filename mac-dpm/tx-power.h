#include "net/netstack.h"

/* Gets the TX power from the underlying radio driver. */
radio_value_t get_tx_power(void);

/* Gets the maximum TX power from the underlying radio driver. */
radio_value_t get_max_tx_power(void);

/* Gets the minimum TX power from the underlying radio driver. */
radio_value_t get_min_tx_power(void);

/* Sets the TX power using the underlying radio driver. */
void set_tx_power(radio_value_t);

/* Sets the TX power the the maximum power using the underlying radio driver. */
void reset_tx_power(void);
