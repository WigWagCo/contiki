FRAMEZ_PROJECT =  WW-sensor-board

BURNER = WW-sensor-board_$(TARGET).bin
#BURNER = test-dht11_$(TARGET).bin


all: $(FRAMEZ_PROJECT)
	echo ECONOTAGBIN=$(shell pwd)/$(BURNER) > ~/.econotagsource

UIP_CONF_IPV6=1

APPS += frzprotocol
# for extra debug... parts of uIP debuggin are wrapped with #ifdef FRZ_DEBUG_2 

#CFLAGS+= -DFRZ_DEBUG_2

CFLAGS += -DUIP_CONF_IPV6_RPL -DUIP_CONF_ROUTER -DPROJECT_CONF_H=\"project-conf.h\" -ffunction-sections 
# -wrap,malloc to wrap malloc()  + include hook_malloc.c in TARGET_SOURCEFILES  ... used to track memory malloc()s
LDFLAGS += -Wl,--gc-sections,--undefined=_reset_vector__,--undefined=InterruptVectors,--undefined=_copy_data_init__,--undefined=_clear_bss_init__,--undefined=_end_of_init__,-wrap,malloc

# for some reason Contiki does not include assert.c now...
CONTIKI_TARGET_SOURCEFILES += assert.c hook_malloc.c 

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
