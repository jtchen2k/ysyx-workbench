LIBCAPSTONE = csrc/tools/capstone/repo/libcapstone.so.5
csrc/utils/disasm.c: $(LIBCAPSTONE)

INC_PATH += $(abspath csrc/tools/capstone/repo/include)
$(LIBCAPSTONE):
	$(MAKE) -C csrc/tools/capstone

INC_PATH += $(abspath csrc/tools)
DIRS_BLACKLIST += $(abspath csrc/tools/capstone)