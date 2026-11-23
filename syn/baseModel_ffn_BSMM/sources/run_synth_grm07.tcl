# demo2d shared_axi: csim + C synthesis + Vivado IP export (Vitis HLS 2022.2).
# Run on the synthesis server, from this directory:  vitis_hls -f run_synth_grm07.tcl
# Paths are relative, so the folder can be copied anywhere.
open_project -reset demo2d_shared_axi
set_top transformer_top
foreach f [lsort [glob *.cpp]] { if {![string match *_tb.cpp $f]} { add_files $f } }
foreach f [lsort [glob *.h]] { add_files $f }
add_files -tb transformer_top_tb.cpp
open_solution -reset solution1 -flow_target vivado
set_part xcu280-fsvh2892-2L-e
create_clock -period 10 -name default
config_schedule -effort medium
csim_design -O
csynth_design
export_design -flow syn -rtl verilog -format ip_catalog \
    -vendor polymtl.ca -library hls -version 1.0 \
    -display_name "demo2d transformer precoder W6A6" \
    -description "4x4 UPA, K=4 MU-MIMO transformer precoder, s_axilite control + m_axi gmem"
exit
