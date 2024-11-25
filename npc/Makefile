VBIN=$(VERILATOR_ROOT)/bin
VERILOG_SRC=$(wildcard vsrc/*.v)
CSRC=$(wildcard csrc/*.cpp csrc/*.c csrc/*.h csrc/*.hpp)
TARGET=$(patsubst vsrc/%.v,V%,$(VERILOG_SRC))
BUILD_DIR=build

all: $(VBIN)/verilator $(TARGET)

build: $(TARGET)

$(VBIN)/verilator:
	@echo "Could not found verilator binary in $(VBIN)."
	@exit 1

$(TARGET):
	@mkdir -p $(BUILD_DIR)
	$(VBIN)/verilator --trace --cc --exe --build -j 0 -Wall --Mdir $(BUILD_DIR) $(CSRC) $(VERILOG_SRC)

.PHONY: sim
sim: $(TARGET)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	./build/$(TARGET)

clean:
	rm -rf build

test: $(VBIN)/verilator
	$(VBIN)/verilator --version

include ../Makefile
