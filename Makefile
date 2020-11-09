CONTIKI_PROJECT = udp-client udp-server
MAC_DPM_MODULE = mac-dpm
TX_ENERGEST_MODULE = tx-energest-service
CONTIKI=../contiki-ng

all: $(CONTIKI_PROJECT)

mac_dpm: distclean
	cp -rf $(MAC_DPM_MODULE) $(CONTIKI)/os/net/mac

tx_energest: distclean
	cp -rf $(TX_ENERGEST_MODULE) $(CONTIKI)/os/services


MODULES += os/net/mac/$(MAC_DPM_MODULE)
MODULES += os/services/$(TX_ENERGEST_MODULE)

include $(CONTIKI)/Makefile.include
