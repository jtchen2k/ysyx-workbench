BUILD_DIR = ./build
.DEFAULT_GOAL := build

# verilator
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/npc

# load makefiles
include $(NVBOARD_HOME)/scripts/nvboard.mk
include $(NPC_HOME)/scripts/config.mk

FILELIST_MK = $(shell find -L ./csrc -name "filelist.mk")
include $(FILELIST_MK)

# prepare sources
VSRCS = $(abspath $(wildcard build/*.sv build/*.v))
SCALA_SRC = $(abspath $(shell find core/src -name "*.scala"))
CSRCS += $(SRC_AUTO_BIND)

# filter out directory specified by DIRS_BLACKLIST
CSRCS_ALL = $(abspath $(shell find csrc -name "*.cpp" -o -name "*.cc"))
CSRCS_BLACKLIST += $(shell find -L $(DIRS_BLACKLIST) -name "*.cpp" -o -name "*.cc")
CSRCS = $(filter-out $(CSRCS_BLACKLIST), $(CSRCS_ALL))
CHDRS = $(abspath $(shell find csrc -name "*.h" -o -name "*.hpp"))

TOPNAME = Top
NXDC_FILES = constr/$(TOPNAME).nxdc

ifneq ($(shell which bear),)
VCC = bear -- verilator
else
VCC = verilator
endif

VFLAGS = --build -cc -O3 --x-assign fast --x-initial fast --noassert --trace --build-jobs $(shell nproc)


INC_PATH += $(abspath csrc/include)
INCFLAGS  = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\"" -Wall -Wextra
# for aarch64
CXXFLAGS += -std=c++17 -isystem /usr/include/aarch64-linux-gnu/c++/13/ -isystem /usr/include/c++/13/
# for x86_64
CXXFLAGS += -isystem /usr/include/x86_64-linux-gnu/c++/13/
LDFLAGS  += -lreadline -ldl

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
$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE) $(CHDRS)
	@rm -rf $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(VCC) $(VFLAGS) \
		--top-module $(TOPNAME) $(filter-out $(CHDRS), $^) \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

sim: $(BIN)
	$(call git_commit, "sim npc") # DO NOT REMOVE THIS LINE!!!
	$(BIN)

build: $(BIN)

# chisel
PRJ = core
MILL = mill

test:
	$(MILL) -i $(PRJ).test

$(VSRCS): $(SCALA_SRC)
	@echo "Generating Verilog files..."
	@make verilog

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
	-rm -rf $(OBJ_DIR)
	-rm -rf $(BIN)

clean-all:
	-rm -rf $(BUILD_DIR)
	-rm -f compile_commands.json

NPCFLAGS += $(ARGS)
NPCFLAGS += -V
run: $(BIN)
	$(BIN) $(NPCFLAGS)

gdb: $(BIN)
	gdb -s $(BIN) --args $(BIN) $(NPCFLAGS)

count:
	cloc --include-ext=c,cc,cpp,hpp,h,scala,v,sv,py --fullpath --not-match-d='csrc/tools/*|./build/*' --by-file .

.PHONY: test verilog help reformat checkformat clean default build run count

-include ../Makefile