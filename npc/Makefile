VBIN := $(VERILATOR_ROOT)/bin
VCC := $(VBIN)/verilator
VERILOG_SRC := $(wildcard vsrc/*.v)
CSRC := $(wildcard csrc/*.cpp csrc/*.c csrc/*.h csrc/*.hpp)
TARGET := $(patsubst vsrc/%.v,V%,$(VERILOG_SRC))
BUILD_DIR := build

.PHONY: all build sim clean

all: build

build: $(TARGET)

$(VCC):
	@echo "Could not found verilator binary at $(VCC)."
	@exit 1

$(TARGET): $(VERILOG_SRC) $(CSRC)
	@mkdir -p $(BUILD_DIR)
	$(VCC) --trace --cc --exe --build -j 0 -Wall --Mdir $(BUILD_DIR) $(CSRC) $(VERILOG_SRC)

sim: $(TARGET)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	./build/$(TARGET)

clean:
	rm -rf $(TARGET)

test: $(VCC)
	$(VBIN)/verilator --version

include ../Makefile
