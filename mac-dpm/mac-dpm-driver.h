/**
 * A wrapper around a MAC driver which lowers the transmission power when it is possible.
 * MAC driver can be set by setting the symbolic constants from the mac-dpm-conf.h in the project-conf.h file.
 */
#include "net/mac/mac.h"

extern const struct mac_driver mac_dpm_driver;
