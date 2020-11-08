#include "net/mac/mac-dpm/mac-dpm-output.h"
#include "net/mac/mac-dpm/mac-dpm.h"
#include "net/mac/mac-dpm/mac-dpm-conf.h"

#include "net/linkaddr.h"
#include "net/packetbuf.h"
#include "net/netstack.h"

#include "sys/log.h"
#define LOG_MODULE "MAC DPM OUTPUT"
#define LOG_LEVEL LOG_LEVEL_MAC_DPM_OUTPUT


static void
packet_sent(void * ptr, int status, int transmission) {
  LOG_DBG("MAC DPM packet successfully sent.\n");
}


static void
send_mac_dpm_packet(dpm_data_object_type_t dpm_data_object_type, linkaddr_t const * const dest) {
  /* Set the link layer destination address for the packet as a
   * packetbuf attribute. The MAC layer can access the destination
   * address with the function packetbuf_addr(PACKETBUF_ADDR_RECEIVER).
   */
  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, dest);

  /* Copy the data object to the packetbuf. */
  dpm_data_object_t dpm_data_object = {dpm_data_object_type};
  int copied_bytes = packetbuf_copyfrom(&dpm_data_object, sizeof(dpm_data_object_t));
  if (copied_bytes != sizeof(dpm_data_object_t)) {
      LOG_ERR("The data object is too big to be sent.");
      return;
  }

  /* Provide a callback function to receive the result of
     a packet transmission. */
  NETSTACK_MAC.send(&packet_sent, NULL);
}


void
send_mac_dpm_beacon(void) {
  send_mac_dpm_packet(MAC_DPM_BROADCAST, &linkaddr_null);
}


void
send_mac_dpm_response(linkaddr_t const * const receiver_address) {
  send_mac_dpm_packet(MAC_DPM_RESPONSE, receiver_address);
}


void
send_mac_dpm_status_request(linkaddr_t const * const receiver_address) {
  send_mac_dpm_packet(MAC_DPM_STATUS_REQUEST, receiver_address);
}


void
send_mac_dpm_status_response(linkaddr_t const * const receiver_address) {
  send_mac_dpm_packet(MAC_DPM_STATUS_RESPONSE, receiver_address);
}


void
send_mac_dpm_updating_request(void) {
  send_mac_dpm_packet(MAC_DPM_UPDATING, &linkaddr_null);
}


void
send_mac_dpm_updating_response(linkaddr_t const * const receiver_address) {
  send_mac_dpm_packet(MAC_DPM_UPDATING_RESPONSE, receiver_address);
}
