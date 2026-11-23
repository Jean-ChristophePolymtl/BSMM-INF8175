// Functional C++ model of the M_ff1 RTL blackbox
// This function is used during C simulation
// During HLS synthesis, it is replaced by the RTL implementation

#include "ff1_bitserial_blackbox.h"

void ff1_bitserial_blackbox(
    int layer,
    hls::stream<ap_uint<32>>& activation_stream,
    hls::stream<ap_uint<128>>& result_stream) {

    ap_int<4> activations[4][128];

    // The RTL activation interface transfers eight 4-bit values per 32-bit word.
    for (int sample = 0; sample < 4; ++sample) {
        for (int input_index = 0; input_index < 128; input_index += 8) {
#pragma HLS PIPELINE II=1
            ap_uint<32> packed = activation_stream.read();
            for (int lane = 0; lane < 8; ++lane) {
#pragma HLS UNROLL
                activations[sample][input_index + lane] =
                    static_cast<ap_int<4>>(packed.range(lane * 4 + 3, lane * 4));
            }
        }
    }

    ap_int<16> results[4][32];

    for (int sample = 0; sample < 4; ++sample) {
        for (int output_index = 0; output_index < 32; ++output_index) {
#pragma HLS PIPELINE II=1
            ap_int<16> accumulator = 0;

            for (int input_index = 0; input_index < 128; ++input_index) {
#pragma HLS UNROLL
                accumulator += ap_int<16>(activations[sample][input_index])
                             * ap_int<16>(ff1_W[layer][output_index][input_index]);
            }

            results[sample][output_index] = accumulator;
        }
    }

    // The RTL result interface transfers eight 16-bit values per 128-bit word.
    for (int sample = 0; sample < 4; ++sample) {
        for (int output_index = 0; output_index < 32; output_index += 8) {
#pragma HLS PIPELINE II=1
            ap_uint<128> packed = 0;
            for (int lane = 0; lane < 8; ++lane) {
#pragma HLS UNROLL
                packed.range(lane * 16 + 15, lane * 16) =
                    results[sample][output_index + lane].range(15, 0);
            }
            result_stream.write(packed);
        }
    }
}