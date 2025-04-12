COLOR_RED := $(shell echo "\033[1;31m")
COLOR_END := $(shell echo "\033[0m")

ifeq ($(wildcard csrc/.config),)
$(warning $(COLOR_RED)Warning: .config does not exist!$(COLOR_END))
$(warning $(COLOR_RED)To build the project, first run 'make menuconfig'.$(COLOR_END))
endif

KCONFIG_PATH := $(NPC_HOME)/csrc/tools/kconfig
Kconfig := $(NPC_HOME)/Kconfig
rm-distclean += csrc/include/generated csrc/include/config csrc/.config csrc/.config.old


Q      := @
CONF   := $(KCONFIG_PATH)/build/conf
MCONF  := $(KCONFIG_PATH)/build/mconf


$(CONF):
	$(Q)$(MAKE) $(silent) -C $(KCONFIG_PATH) NAME=conf

$(MCONF):
	$(Q)$(MAKE) $(silent) -C $(KCONFIG_PATH) NAME=mconf


menuconfig: $(MCONF) $(CONF)
	$(Q)cd csrc && $(MCONF) $(Kconfig)
	$(Q)cd csrc && $(CONF) $(silent) --syncconfig $(Kconfig)

distclean: clean
	-@rm -rf $(rm-distclean)

.PHONY: menuconfig distclean