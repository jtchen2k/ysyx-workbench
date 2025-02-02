BUILD_DIR = ./build

.DEFAULT_GOAL := build

# verilator
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

VSRCS = $(abspath $(wildcard build/*.sv build/*.v))
CSRCS = $(abspath $(wildcard csrc/*.cc csrc/*.c csrc/*.h csrc/*.hpp csrc/*.cpp))
CSRCS += $(SRC_AUTO_BIND)

TOPNAME = Light
NXDC_FILES = constr/$(TOPNAME).nxdc

ifneq ($(shell which bear),)
VCC = bear -- verilator
else
VCC = verilator
endif

VFLAGS = --build -cc -O3 --x-assign fast --x-initial fast --noassert --trace

include $(NVBOARD_HOME)/scripts/nvboard.mk

INC_PATH ?=
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""


# NVBoard Constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	mkdir -p $(OBJ_DIR)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# Binary Target
$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE) verilog
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(VCC) $(VFLAGS) \
		--top-module $(TOPNAME) $(filter-out verilog, $^) \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

sim: $(BIN)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	$(BIN)

build: $(BIN)

# chisel
PRJ = playground
MILL = ./mill
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

.PHONY: test verilog help reformat checkformat clean default build
-include ../Makefile
