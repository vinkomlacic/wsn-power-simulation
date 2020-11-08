/**
 * Creates MAC DPM frames and sends them using the underlying MAC driver.
 */
#ifndef MAC_DPM_OUTPUT_
#define MAC_DPM_OUTPUT_

#include "net/linkaddr.h"

/**
 * Sends a MAC DPM beacon to every neighbor in the vicinity.
 */
void send_mac_dpm_beacon(void);

/**
 * Sends a MAC DPM response to the specified link layer address.
 */
void send_mac_dpm_response(linkaddr_t const *);

/**
 * Sends a MAC DPM status request to a single node.
 */
void send_mac_dpm_status_request(linkaddr_t const *);

/**
 * Sends a MAC DPM status response from a single node.
 */
void send_mac_dpm_status_response(linkaddr_t const *);

/**
 * Sends an updating request.
 */
void send_mac_dpm_updating_request(void);

/**
 * Sends a MAC DPM updating response.
 */
void send_mac_dpm_updating_response(linkaddr_t const *);

#endif /* MAC_DPM_OUTPUT_ */
