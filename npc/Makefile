# 0.verilator export：
VERILATOR = $(VERILATOR_ROOT)/bin/verilator

# 1.files info:
VSRCS += $(shell find $(abspath ./vsrc) -name "*.v" -not -path "$(abspath ./vsrc/tb)/*")
CSRCS += $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
TOP_MODULE ?= top

# 2.yosys-sta info：
PROJ_PATH = $(shell pwd)
LIB_PATH = $(PROJ_PATH)/yosys
SDC_FILE ?= $(PROJ_PATH)/sdc/$(TOP_MODULE).sdc
RTL_FILES ?= $(shell find $(PROJ_PATH)/vsrc -name "*.v")
export CLK_FREQ_MHZ ?= 500
RESULT_DIR = $(LIB_PATH)/result/$(TOP_MODULE)-$(CLK_FREQ_MHZ)MHz
SCRIPT_DIR = $(LIB_PATH)/scripts
NETLIST_SYN_V   = $(RESULT_DIR)/$(TOP_MODULE).netlist.syn.v
NETLIST_FIXED_V = $(RESULT_DIR)/$(TOP_MODULE).netlist.fixed.v
TIMING_RPT = $(RESULT_DIR)/$(TOP_MODULE).rpt

# 3.run info: 
IMG ?= ../am-kernels/tests/cpu-tests/build/dummy-riscv32-nemu.bin	# program img, run on npc.
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
INC_PATH = $(OBJ_DIR)
BIN = $(BUILD_DIR)/$(TOP_MODULE)
RUN_FLAG = --img=${IMG}
NPC_EXEC = $(BIN) $(RUN_FLAG)

# 4.verilator flags:
INCFLAGS = $(addprefix -I, $(INC_PATH)) $(addprefix -I, ./vsrc) -I$(abspath ./csrc/include) -I$(abspath ./csrc/include/memory)
CXXFLAGS += $(INCFLAGS) -DTOP_MODULE="\"V$(TOP_MODULE)\""
VERILATOR_CFLAGS += -MMD --build -cc --trace -j -O3 --x-assign fast --x-initial fast --noassert

com: $(VSRCS) $(CSRCS)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOP_MODULE) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

default: com
$(shell mkdir -p $(BUILD_DIR))

# wave simulation
run: com
	@$(NPC_EXEC)
# gtkwave dump.vcd

#yosys simulation
syn: $(NETLIST_SYN_V)
$(NETLIST_SYN_V): $(RTL_FILES) $(SCRIPT_DIR)/yosys.tcl
	mkdir -p $(@D)
	echo tcl $(SCRIPT_DIR)/yosys.tcl $(TOP_MODULE) \"$(RTL_FILES)\" $@ | yosys -l $(@D)/yosys.log -s -

fix-fanout: $(NETLIST_FIXED_V)
$(NETLIST_FIXED_V): $(SCRIPT_DIR)/fix-fanout.tcl $(SDC_FILE) $(NETLIST_SYN_V)
	$(LIB_PATH)/bin/iEDA -script $^ $(TOP_MODULE) $@ 2>&1 | tee $(RESULT_DIR)/fix-fanout.log

sta: $(TIMING_RPT)
$(TIMING_RPT): $(SCRIPT_DIR)/sta.tcl $(SDC_FILE) $(NETLIST_FIXED_V)
	$(LIB_PATH)/bin/iEDA -script $^ $(TOP_MODULE) 2>&1 | tee $(RESULT_DIR)/sta.log

# yosys simulation
yos: MODULE ?= exu
yos:
	yowasp-yosys -p "read_verilog -sv ./vsrc/$(MODULE).v; hierarchy -top $(MODULE); proc; flatten; show -format dot -prefix $(MODULE); stat"

clean:
	rm -rf $(BUILD_DIR) *.log *.dmp *.vpd core
	-rm -rf $(VIVADO_PROJECT_NAME) $(PROJECT_NAME).runs $(PROJECT_NAME).sim $(PROJECT_NAME).cache $(PROJECT_NAME).hw $(PROJECT_NAME).ip_user_files $(PROJECT_NAME).xpr *.jou *.log *.str
	-rm -rf $(LIB_PATH)/result/
	-rm -rf ./reports ./prj ./user
	-rm *.vcd *.view
	-rm *.dot

.PHONY: init syn fix-fanout sta clean

