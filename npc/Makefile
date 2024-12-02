# Environment
TOPNAME = light
NXDC_FILES = constr/light.nxdc
SHELL = /bin/sh

VCC := verilator
VFLAGS = --build -cc -O3 --x-assign fast --x-initial fast --noassert --trace

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

# Project Src
VSRCS = $(abspath $(wildcard vsrc/*.v))
CSRCS = $(abspath $(wildcard csrc/*.cpp csrc/*.c csrc/*.h csrc/*.hpp))
CSRCS += $(SRC_AUTO_BIND)

.PHONY: build sim clean

$(shell mkdir -p $(BUILD_DIR))

default: build

include $(NVBOARD_HOME)/scripts/nvboard.mk

# Constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

INC_PATH ?=

INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VCC) $(VFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

build: $(BIN)

# $(TARGET): $(VERILOG_SRC) $(CSRC)
# 	@mkdir -p $(BUILD_DIR)
# 	$(VCC) --trace --cc --exe --build -j 0 -Wall --Mdir $(BUILD_DIR) $(CSRC) $(VERILOG_SRC)

sim: $(BIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$^

clean:
	rm -rf $(BIN)
	rm -rf $(BUILD_DIR)

test:
	$(VCC) --version

include ../Makefile
