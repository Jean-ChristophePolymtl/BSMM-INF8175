###########################################################################
# vivado_syn.tcl
#
# Description:
#   Creates a Vivado project from scratch.
#
# Usage:
#   vivado -mode batch -source vivado_syn.tcl
#
# Author:
#   Jean-Christophe Gauthier
#
# Date:
#   2026-6-15
###########################################################################

# ==========================================================================
# User configuration
# ==========================================================================


set PROJECT_NAME [lindex $argv 0]
set ARCHITECTURE $PROJECT_NAME

set PROJECT_PATH $env(VIVADO_BUILDS)
set VITIS_PATH $env(VITIS_HOME)

set BD_NAME "design_1"

set SCRIPT_DIR [file dirname [file normalize [info script]]]



set BD_TCL [file normalize [file join \
    $SCRIPT_DIR \
    ".." \
    ".." \
    "block_designs" \
    "design_qdma.tcl"]]

puts "Block design Tcl: $BD_TCL"

create_project -force $PROJECT_NAME $PROJECT_PATH/$PROJECT_NAME -part xcu280-fsvh2892-2L-e

set_property board_part xilinx.com:au280:part0:1.2 [current_project]
set_property platform.extensible true [current_project]

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}
file mkdir $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sources_1/ip
file mkdir $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sources_1/new
file mkdir $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sources_1/bd
# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}
file mkdir $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/constrs_1/new
# Create 'sim_1' fileset (if not found)
if {[string equal [get_filesets -quiet sim_1] ""]} {
  create_fileset -simset sim_1
}
file mkdir $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sim_1/new


#*********************************************************************************************************
# Adding the custom IP
#*********************************************************************************************************

set HLS_IP_PATH "/export/tmp/allaire/bismo/ip_repo/w4a4/ip"

if {![file isdirectory $HLS_IP_PATH]} {
    error "HLS IP repository not found: $HLS_IP_PATH"
}

set_property ip_repo_paths $HLS_IP_PATH [current_project]
update_ip_catalog

puts "Transformer IP: [get_ipdefs -all -quiet *transformer_top*]"

#*********************************************************************************************************
#create_bd_design "$BD_NAME"
#*********************************************************************************************************

#create_bd_design $BD_NAME
#update_compile_order -fileset sources_1
#open_bd_design $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sources_1/bd/$BD_NAME/$BD_NAME.bd
#create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze:11.0 microblaze_0
#apply_bd_automation -rule xilinx.com:bd_rule:microblaze -config { axi_intc {1} axi_periph {Enabled} cache {None} clk {New Clocking Wizard} cores {1} debug_module {Debug Only} ecc {None} local_mem {128KB} preset {None}}  [get_bd_cells microblaze_0]

#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {sysclk0 ( 100 MHz System differential clock0 ) } Manual_Source {Auto}}  [get_bd_intf_pins clk_wiz_1/CLK_IN1_D]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {resetn ( FPGA Resetn ) } Manual_Source {Auto}}  [get_bd_pins clk_wiz_1/reset]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {resetn ( FPGA Resetn ) } Manual_Source {Auto}}  [get_bd_pins rst_clk_wiz_1_100M/ext_reset_in]

# UART
#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uart16550:2.0 axi_uart16550_0
#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/clk_wiz_1/clk_out1 (100 MHz)} Clk_slave {Auto} Clk_xbar {/clk_wiz_1/clk_out1 (100 MHz)} Master {/microblaze_0 (Periph)} Slave {/axi_uart16550_0/S_AXI} ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_uart16550_0/S_AXI]
#apply_bd_automation -rule xilinx.com:bd_rule:board -config { Board_Interface {rs232_uart ( UART ) } Manual_Source {Auto}}  [get_bd_intf_pins axi_uart16550_0/UART]

#connect_bd_net [get_bd_pins axi_uart16550_0/ip2intc_irpt] [get_bd_pins microblaze_0_xlconcat/In0]

#delete_bd_objs [get_bd_nets resetn_0_1]
#delete_bd_objs [get_bd_ports resetn_0]
#connect_bd_net [get_bd_ports resetn] [get_bd_pins rst_clk_wiz_1_100M/ext_reset_in]

#*********************************************************************************************************
# Adding the Custom HLS IP
#*********************************************************************************************************

#create_bd_cell -type ip -vlnv xilinx.com:hls:transformer_top:1.0 transformer_top_0
#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.1 axi_bram_ctrl_0
#create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_0

#set_property CONFIG.SINGLE_PORT_BRAM {1} [get_bd_cells axi_bram_ctrl_0]
#set_property CONFIG.PROTOCOL {AXI4LITE} [get_bd_cells axi_bram_ctrl_0]

#apply_bd_automation -rule xilinx.com:bd_rule:bram_cntlr -config {BRAM "Auto" }  [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA]

#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/clk_wiz_1/clk_out1 (100 MHz)} Clk_slave {Auto} Clk_xbar {/clk_wiz_1/clk_out1 (100 MHz)} Master {/microblaze_0 (Periph)} Slave {/axi_bram_ctrl_0/S_AXI} ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_bram_ctrl_0/S_AXI]

#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {Auto} Clk_slave {/clk_wiz_1/clk_out1 (100 MHz)} Clk_xbar {/clk_wiz_1/clk_out1 (100 MHz)} Master {/transformer_top_0/m_axi_gmem} Slave {/microblaze_0_axi_intc/s_axi} ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}}  [get_bd_intf_pins transformer_top_0/m_axi_gmem]

#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/clk_wiz_1/clk_out1 (100 MHz)} Clk_slave {Auto} Clk_xbar {/clk_wiz_1/clk_out1 (100 MHz)} Master {/microblaze_0 (Periph)} Slave {/transformer_top_0/s_axi_control} ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}}  [get_bd_intf_pins transformer_top_0/s_axi_control]

#connect_bd_net [get_bd_pins transformer_top_0/interrupt] [get_bd_pins microblaze_0_xlconcat/In1]

#create_bd_cell -type ip -vlnv xilinx.com:ip:axi_perf_mon:5.0 axi_perf_mon_0

#apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/clk_wiz_1/clk_out1 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_perf_mon_0/core_aclk]

#apply_bd_automation -rule xilinx.com:bd_rule:axi4 -config { Clk_master {/clk_wiz_1/clk_out1 (100 MHz)} Clk_slave {Auto} Clk_xbar {/clk_wiz_1/clk_out1 (100 MHz)} Master {/transformer_top_0/m_axi_gmem} Slave {/axi_perf_mon_0/S_AXI} ddr_seg {Auto} intc_ip {/microblaze_0_axi_periph} master_apm {0}}  [get_bd_intf_pins axi_perf_mon_0/S_AXI]

#connect_bd_intf_net [get_bd_intf_pins axi_perf_mon_0/SLOT_0_AXI] -boundary_type upper [get_bd_intf_pins microblaze_0_axi_periph/S01_AXI]
#apply_bd_automation -rule xilinx.com:bd_rule:clkrst -config { Clk {/clk_wiz_1/clk_out1 (100 MHz)} Freq {100} Ref_Clk0 {} Ref_Clk1 {} Ref_Clk2 {}}  [get_bd_pins axi_perf_mon_0/slot_0_axi_aclk]

# ==========================================================================
# Create the complete block design, including QDMA
# ==========================================================================

if {![file exists $BD_TCL]} {
    error "Block design Tcl file not found: $BD_TCL"
}

source $BD_TCL

validate_bd_design
save_bd_design

set BD_FILE [get_files -quiet "${BD_NAME}.bd"]

if {[llength $BD_FILE] == 0} {
    error "Block design file ${BD_NAME}.bd was not created"
}

generate_target all $BD_FILE

#*********************************************************************************************************
# Making the wrapper
#*********************************************************************************************************

make_wrapper -files [get_files $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.srcs/sources_1/bd/$BD_NAME/design_1.bd] -top

add_files -norecurse $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.gen/sources_1/bd/$BD_NAME/hdl/design_1_wrapper.v

puts $BD_NAME
append bdWrapperName $BD_NAME "_wrapper"
puts $bdWrapperName
set_property top $bdWrapperName [current_fileset]

launch_runs impl_1 -to_step write_bitstream -jobs 5
wait_on_run impl_1

set IMPL_STATUS [get_property STATUS [get_runs impl_1]]
puts "Implementation status: $IMPL_STATUS"

if {![string match "*Complete*" $IMPL_STATUS]} {
    error "Implementation failed: $IMPL_STATUS"
}

write_hw_platform -fixed -include_bit -force -file $PROJECT_PATH/$PROJECT_NAME/$bdWrapperName.xsa
add_files -fileset sources_1  -copy_to $PROJECT_PATH/$PROJECT_NAME -force -quiet [glob -nocomplain $PROJECT_PATH/$PROJECT_NAME/$PROJECT_NAME.runs/impl_1/*.bit]

