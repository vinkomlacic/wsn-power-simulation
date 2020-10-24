#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY  1
#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/

static int
validate_tx_power_in_range(radio_value_t tx_power) {
  radio_value_t min_tx_power, max_tx_power;
  radio_result_t result;

  result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &min_tx_power);
  if (result != RADIO_RESULT_OK) {
    LOG_ERR("Problem with getting TX power min: %d\n", result);
    return -1;
  }

  result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &max_tx_power);
  if (result != RADIO_RESULT_OK) {
    LOG_ERR("Problem with getting TX power max: %d\n", result);
    return -1;
  }

  if (tx_power >= min_tx_power && tx_power <= max_tx_power) {
    return 0;
  } else {
    return -1;
  }
}

static void
set_tx_power(radio_value_t tx_power) {
  if (validate_tx_power_in_range(tx_power)) {
    LOG_ERR("tx power out of range: %d\n", tx_power);
    return;
  }

  radio_result_t result;
  result = NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, tx_power);
  if (result != RADIO_RESULT_OK) {
    LOG_ERR("Problem with setting TX power: %d\n", result);
  }
}

static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{

  LOG_INFO("Received response '%.*s' from ", datalen, (char *) data);
  LOG_INFO_6ADDR(sender_addr);
#if LLSEC802154_CONF_ENABLED
  LOG_INFO_(" LLSEC LV:%d", uipbuf_get_attr(UIPBUF_ATTR_LLSEC_LEVEL));
#endif
  LOG_INFO_("\n");

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;

  static radio_value_t rssi, tx_max, tx_min, current_tx_power;
  static radio_result_t result;
  static unsigned int counter = 0;
  radio_value_t linkquality;

  PROCESS_BEGIN();

  /* Initialize UDP connection */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() && NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {
      /* Send to DAG root */
      LOG_INFO("Sending request %u to ", count);

      /* Printing radio important parameters */
      result = NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, &rssi);
      LOG_INFO("%d, RSSI: %d\n", result, rssi);
      result = NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_LINK_QUALITY, &linkquality);
      LOG_INFO("%d, LQI: %u\n", result, linkquality);
      result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MAX, &tx_max);
      LOG_INFO("%d, Maximum TX power: %d\n", result, tx_max);
      result = NETSTACK_RADIO.get_value(RADIO_CONST_TXPOWER_MIN, &tx_min);
      LOG_INFO("%d, Minimum TX power: %d\n", result, tx_min);
      result = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &current_tx_power);
      LOG_INFO("%d, Current TX power: %d\n", result, current_tx_power);

      /* Changing TX power */
      LOG_INFO("Trying to set the TX power to: %d\n", tx_max - counter);
      set_tx_power(tx_max - counter);
      counter = (counter+1) % (tx_max - tx_min + 1);
      result = NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &current_tx_power);
      LOG_INFO("%d, Current TX power after modifying: %d\n", result, current_tx_power);

      LOG_INFO_6ADDR(&dest_ipaddr);
      LOG_INFO("\n");
      snprintf(str, sizeof(str), "hello %d", count);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);
      count++;
    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Add some jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
