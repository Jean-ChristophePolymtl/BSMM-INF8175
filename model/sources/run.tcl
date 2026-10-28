# demo2d_fpga_s4: csim only (synthesis: run_synth_grm07.tcl).
# Run from this directory: vitis_hls -f run.tcl
open_project -reset demo2d_fpga_s4
set_top transformer_top
foreach f [lsort [glob *.cpp]] { if {![string match *_tb.cpp $f]} { add_files $f } }
foreach f [lsort [glob *.h]] { add_files $f }
add_files -tb transformer_top_tb.cpp
open_solution -reset solution1 -flow_target vivado
set_part xcu280-fsvh2892-2L-e
create_clock -period 10 -name default
config_schedule -effort medium
csim_design -O
exit
