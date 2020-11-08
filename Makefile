CONTIKI_PROJECT = udp-client udp-server
MAC_DPM_MODULE = mac-dpm
CONTIKI=../contiki-ng

all: $(CONTIKI_PROJECT)

mac_dpm: distclean
	cp -rf $(MAC_DPM_MODULE) $(CONTIKI)/os/net/mac


MODULES += os/net/mac/$(MAC_DPM_MODULE)

include $(CONTIKI)/Makefile.include
