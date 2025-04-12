LIBCAPSTONE = csrc/tools/capstone/repo/libcapstone.so.5
csrc/utils/disasm.c: $(LIBCAPSTONE)

INC_PATH += $(abspath csrc/tools/capstone/repo/include)
$(LIBCAPSTONE):
	$(MAKE) -C csrc/tools/capstone

INC_PATH += $(abspath csrc/tools)
CXXFLAGS += -D__LIBCAPSTONE_PATH__=\\\"$(abspath $(LIBCAPSTONE))\\\"
DIRS_BLACKLIST += $(abspath csrc/tools/capstone)

DIFFTEST_REF_SO = $(NEMU_HOME)/build/riscv32-nemu-interpreter-so

ifeq (wildcard $(DIFFTEST_REF_SO),)
$(error nemu ref not found, please build nemu first)
endif

CXXFLAGS += -D__DIFFTEST_REF_SO__=\\\"$(abspath $(DIFFTEST_REF_SO))\\\"

ifeq ($(CONFIG_DIFFTEST),y)
@echo "DIFFTEST_REF_SO: $(DIFFTEST_REF_SO)"
endif

