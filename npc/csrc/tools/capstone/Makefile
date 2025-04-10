REPO_PATH = repo
ifeq ($(wildcard $(REPO_PATH)/include/capstone/capstone.h),)
  $(shell git clone --depth=1 -b 5.0.3 git@github.com:capstone-engine/capstone.git $(REPO_PATH))
endif

CAPSTONE = $(REPO_PATH)/build/libcapstone.so.5
$(CAPSTONE):
	cd $(REPO_PATH) && CAPSTONE_ARCHS="riscv" bash make.sh

clean:
	$(MAKE) -C $(REPO_PATH) clean
	rm -rf $(REPO_PATH)

all: $(CAPSTONE)

.DEFAULT_GOAL := all

.PHONY: all clean