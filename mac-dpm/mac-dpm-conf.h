#ifndef MAC_DPM_CONF_
#define MAC_DPM_CONF_

#ifdef UNDERLYING_MAC_DRIVER
#ifndef UNDERLYING_MAC_DRIVER_PATH
#error "If you specify a custom MAC driver, you need to change UNDERLYING_MAC_DRIVER_PATH too."
#endif /* UNDERLYING_MAC_DRIVER_PATH */
#endif /* UNDERLYING_MAC_DRIVER */


/* Set underlying MAC driver header path. Otherwise, CSMA driver path is used. */
#ifndef UNDERLYING_MAC_DRIVER_PATH
#define UNDERLYING_MAC_DRIVER_PATH "net/mac/csma/csma.h"
#endif /* UNDERLYING_MAC_DRIVER_PATH */

#include UNDERLYING_MAC_DRIVER_PATH

/* Set underlying MAC driver. The default is the CSMA. */
#ifndef UNDERLYING_MAC_DRIVER
#define UNDERLYING_MAC_DRIVER csma_driver /* The default MAC driver */
#endif /* UNDERLYING_MAC_DRIVER */


/* The MAC DPM beacon interval determines the period of each phase in the MAC DPM lifecycle. */
#ifdef MAC_DPM_CONF_BEACON_INTERVAL_SECONDS
#define MAC_DPM_BEACON_INTERVAL_SECONDS MAC_DPM_CONF_BEACON_INTERVAL_SECONDS
#else
#define MAC_DPM_BEACON_INTERVAL_SECONDS 60
#endif /* MAC_DPM_CONF_BEACON_INTERVAL_SECONDS */


/* MAC DPM wait for ACK period determines the amount of time that is waited until a message is considered lost. */
#ifdef MAC_DPM_CONF_WAIT_FOR_ACK_PERIOD_SECONDS
#define MAC_DPM_WAIT_FOR_ACK_PERIOD_SECONDS MAC_DPM_CONF_WAIT_FOR_ACK_PERIOD_SECONDS
#else
#define MAC_DPM_WAIT_FOR_ACK_PERIOD_SECONDS 2
#endif /* MAC_DPM_CONF_WAIT_FOR_ACK_PERIOD_SECONDS */


/* This constant sets all MAC DPM logging modules to it. */
#ifdef MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#define LOG_LEVEL_CONF_MAC_DPM_MAIN MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#define LOG_LEVEL_CONF_DPM_NBR MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#define LOG_LEVEL_CONF_DPM_OUTPUT MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#define LOG_LEVEL_CONF_DPM_DRIVER MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#define LOG_LEVEL_CONF_MAC_DPM_MINIMIZATION MAC_DPM_CONF_LOG_LEVEL_MAC_DPM
#endif /* MAC_DPM_CONF_LOG_LEVEL_MAC_DPM */

/* Sets the main MAC DPM process logging module. */
#ifdef LOG_LEVEL_CONF_MAC_DPM_MAIN
#define LOG_LEVEL_MAC_DPM_MAIN LOG_LEVEL_CONF_MAC_DPM_MAIN
#else
#define LOG_LEVEL_MAC_DPM_MAIN LOG_LEVEL_NONE
#endif /* LOG_LEVEL_CONF_MAC_DPM_MAIN */


/* Sets the logging module which handles the neighbor table. */
#ifdef LOG_LEVEL_CONF_MAC_DPM_NBR
#define LOG_LEVEL_MAC_DPM_NBR LOG_LEVEL_CONF_MAC_DPM_NBR
#else
#define LOG_LEVEL_MAC_DPM_NBR LOG_LEVEL_NONE
#endif /* LOG_LEVEL_CONF_MAC_DPM_NBR */


/* Sets the logging level of the module which handles sending MAC DPM messages. */
#ifdef LOG_LEVEL_CONF_MAC_DPM_OUTPUT
#define LOG_LEVEL_MAC_DPM_OUTPUT LOG_LEVEL_CONF_MAC_DPM_OUTPUT
#else
#define LOG_LEVEL_MAC_DPM_OUTPUT LOG_LEVEL_NONE
#endif /* LOG_LEVEL_CONF_MAC_DPM_OUTPUT */


/* Sets the logging level of the module which handles processing incoming MAC DPM messages. */
#ifdef LOG_LEVEL_CONF_MAC_DPM_DRIVER
#define LOG_LEVEL_MAC_DPM_DRIVER LOG_LEVEL_CONF_MAC_DPM_DRIVER
#else
#define LOG_LEVEL_MAC_DPM_DRIVER LOG_LEVEL_NONE
#endif /* LOG_LEVEL_CONF_MAC_DPM_DRIVER */


/* Sets the logging level of the module which handles minimization process. */
#ifdef LOG_LEVEL_CONF_MAC_DPM_MINIMIZATION
#define LOG_LEVEL_MAC_DPM_MINIMIZATION LOG_LEVEL_CONF_MAC_DPM_MINIMIZATION
#else
#define LOG_LEVEL_MAC_DPM_MINIMIZATION LOG_LEVEL_NONE
#endif /* LOG_LEVEL_CONF_MAC_DPM_MINIMIZATION */

#endif /* MAC_DPM_CONF_ */