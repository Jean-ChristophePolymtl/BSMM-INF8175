// Functional C++ model of the M_ff1 RTL blackbox
// This function is used during C simulation
// During HLS synthesis, it is replaced by the RTL implementation

#include <ap_int.h>
#include <hls_stream.h>
#include "ff1.h"

void ff1_bitserial_blackbox(
    int layer,
    hls::stream<ap_uint<32>>& activation_stream,
    hls::stream<ap_uint<128>>& result_stream);