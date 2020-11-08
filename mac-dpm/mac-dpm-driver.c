#include "net/mac/mac-dpm/mac-dpm.h"
#include "net/mac/mac-dpm/mac-dpm-nbr.h"
#include "net/mac/mac-dpm/mac-dpm-output.h"
#include "net/mac/mac-dpm/mac-dpm-driver.h"
#include "net/mac/mac-dpm/mac-dpm-conf.h"

#include "net/mac/mac.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/linkaddr.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "MAC_DPM DRIVER"
#define LOG_LEVEL LOG_LEVEL_MAC_DPM_DRIVER


static void
log_message_reception_from(linkaddr_t const * const sender_address, char const * const message_type) {
    LOG_INFO("Received %s message from ", message_type);
    LOG_INFO_LLADDR(sender_address);
    LOG_INFO_("\n");
}


static void
process_mac_dpm_request(linkaddr_t const * const sender_address) {
    add_linkaddr_to_nbr_table(sender_address);
    send_mac_dpm_response(sender_address);
}


static void
process_mac_dpm_response(linkaddr_t const * const sender_address) {
    add_linkaddr_to_nbr_table(sender_address);
}


static void
process_mac_dpm_status_request(linkaddr_t const * const sender_address) {
    send_mac_dpm_status_response(sender_address);
}


static void
process_mac_dpm_status_response(linkaddr_t const * const sender_address) {
    notify_minimization_about_ack(sender_address);
}


static void
process_mac_dpm_updating_request(linkaddr_t const * const sender_address) {
    send_mac_dpm_updating_response(sender_address);
}


static void
process_mac_dpm_updating_response(linkaddr_t const * const sender_address) {
    verify_nbr_with_linkaddr(sender_address);
}


static void
process_mac_dpm_frame(void) {
    /* Fill in the packetbuf attributes. */
    NETSTACK_FRAMER.parse();

    /* Get the data in the packetbuf. */
    dpm_data_object_t * const dpm_data_object = (dpm_data_object_t *) packetbuf_dataptr();
    dpm_data_object_type_t const message_type = dpm_data_object->dpm_data_object_type;
    linkaddr_t const * const sender_address = packetbuf_addr(PACKETBUF_ADDR_SENDER);

    log_message_reception_from(sender_address, dpm_data_object_type_to_str(message_type));

    switch (message_type) {
        case MAC_DPM_BROADCAST:
        process_mac_dpm_request(sender_address);
        break;

        case MAC_DPM_RESPONSE:
        process_mac_dpm_response(sender_address);
        break;

        case MAC_DPM_STATUS_REQUEST:
        process_mac_dpm_status_request(sender_address);
        break;

        case MAC_DPM_STATUS_RESPONSE:
        process_mac_dpm_status_response(sender_address);
        break;

        case MAC_DPM_UPDATING:
        process_mac_dpm_updating_request(sender_address);
        break;

        case MAC_DPM_UPDATING_RESPONSE:
        process_mac_dpm_updating_response(sender_address);
        break;

        default:
        LOG_ERR("MAC DPM message type unsupported.\n");
        return;
    }
}


static bool
is_mac_dpm_frame(void) {
    size_t const frame_header_length = NETSTACK_FRAMER.length();
    size_t const data_length = packetbuf_datalen();

    if (data_length == 0) {
        return false;
    }

    return (data_length - frame_header_length) == MAC_DPM_OBJECT_SIZE;
}


static void
init(void) {
    UNDERLYING_MAC_DRIVER.init();
    init_mac_dpm();
}


static void
send(mac_callback_t sent, void * ptr) {
    UNDERLYING_MAC_DRIVER.send(sent, ptr);
}


static void
input(void) {
    if (is_mac_dpm_frame()) {
        process_mac_dpm_frame();

    } else {
        UNDERLYING_MAC_DRIVER.input();   

    }
}


static int
on(void) {
    return UNDERLYING_MAC_DRIVER.on();
}

static int
off(void) {
    return UNDERLYING_MAC_DRIVER.off();
}


static int
max_payload(void) {
    return UNDERLYING_MAC_DRIVER.max_payload();
}


const struct mac_driver mac_dpm_driver = {
    "MAC DPM",
    init,
    send,
    input,
    on,
    off,
    max_payload,
};
