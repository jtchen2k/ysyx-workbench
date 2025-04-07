BUILD_DIR = ./build

.DEFAULT_GOAL := build

# verilator
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/npc

VSRCS = $(abspath $(wildcard build/*.sv build/*.v))
CSRCS = $(abspath $(shell find csrc -name "*.cpp" -o -name "*.cc"))
HEADERS = $(abspath $(shell find csrc -name "*.h" -o -name "*.hpp"))
CSRCS += $(SRC_AUTO_BIND)
SCALA_SRC = $(abspath $(shell find core/src -name "*.scala"))

TOPNAME = Top
NXDC_FILES = constr/$(TOPNAME).nxdc

ifneq ($(shell which bear),)
VCC = bear -- verilator
else
VCC = verilator
endif

VFLAGS = --build -cc -O3 --x-assign fast --x-initial fast --noassert --trace -j $(shell nproc)

include $(NVBOARD_HOME)/scripts/nvboard.mk

INC_PATH += $(abspath csrc/include)
INC_PATH += $(abspath csrc/thirdparty)
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
# for aarch64
CXXFLAGS += -std=c++17 -isystem /usr/include/aarch64-linux-gnu/c++/13/ -isystem /usr/include/c++/13/

DEBUG ?= 0
ifeq ($(DEBUG), 1)
CXXFLAGS += -fsanitize=address -g
LDFLAGS += -fsanitize=address -g
endif

# NVBoard Constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	mkdir -p $(OBJ_DIR)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# Binary Target
$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(VCC) $(VFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

$(VSRCS): verilog

sim: $(BIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$(BIN)

build: $(BIN)

# chisel
PRJ = core
MILL = mill

test:
	$(MILL) -i $(PRJ).test

verilog:
	$(call git_commit, "generate verilog")
	mkdir -p $(BUILD_DIR)
	$(MILL) -i $(PRJ).runMain Elaborate --target-dir $(BUILD_DIR)

help:
	$(MILL) -i $(PRJ).runMain Elaborate --help

reformat:
	$(MILL) -i __.reformat

checkformat:
	$(MILL) -i __.checkFormat

bsp:
	$(MILL) -i mill.bsp.BSP/install

idea:
	$(MILL) -i mill.idea.GenIdea/idea

clean:
	-rm -rf $(BUILD_DIR)
	-rm -f compile_commands.json

NPCFLAGS += $(ARGS)
run: $(BIN)
	$(BIN) $(NPCFLAGS)

.PHONY: test verilog help reformat checkformat clean default build run
-include ../Makefile
