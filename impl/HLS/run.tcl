# demo2d_fpga_s4: csim only (synthesis: run_synth_grm07.tcl).
# Run from this directory: vitis_hls -f run.tcl
open_project -reset bitSerialMatMult

set_top bitSerialMatMult

foreach f [lsort [glob *.cpp]] { if {![string match *_tb.cpp $f]} { add_files $f } }

foreach f [lsort [glob *.h]] { add_files $f }

add_files -tb bitSerialMatMult_tb.cpp

open_solution -reset solution1 -flow_target vivado

config_schedule -effort medium

csim_design -O

exit
