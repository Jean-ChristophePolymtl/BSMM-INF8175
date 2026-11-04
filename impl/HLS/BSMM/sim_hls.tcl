open_project -reset bitSerialMatMult

set_top bitSerialMatMult

foreach f [lsort [glob *.cpp]] { if {![string match *_tb.cpp $f]} { add_files $f } }

foreach f [lsort [glob *.h]] { add_files $f }

add_files -tb bitSerialMatMult_tb.cpp

open_solution -reset solution1 -flow_target vivado

set_part xcu280-fsvh2892-2L-e

create_clock -period 10 -name default

config_schedule -effort medium

csim_design -O

csynth_design

export_design -flow syn -rtl verilog -format ip_catalog \
    -vendor polymtl.ca -library hls -version 1.0 \
    -display_name "Bit Serial Matrix Mult Function" \
    -description ""

exit
