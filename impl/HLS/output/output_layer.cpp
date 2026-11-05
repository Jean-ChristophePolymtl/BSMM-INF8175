// Output (non partagé) | dequant ×SOUT=0.000244151 | U=4
#include "bitSerialMatMult.h"
#include "transformer_top.h"
#include "outm.h"
#include "ap_int.h"

namespace {

constexpr int kNumSamples = 4;
constexpr int kInputSize = 32;
constexpr int kOutputSize = 32;

ap_int<20> requantize_output(
    const ap_int<20> accumulator,
    const int scaleFactor,
    const int shiftValue,
    const int bias) {
    const ap_int<36> scaledValue =
        static_cast<ap_int<36>>(accumulator) * static_cast<ap_uint<17>>(scaleFactor);
    ap_int<20> result = (shiftValue > 0)
        ? static_cast<ap_int<20>>(
              (scaledValue + (static_cast<ap_int<36>>(1) << (shiftValue - 1)))
              >> shiftValue)
        : static_cast<ap_int<20>>(scaledValue);

    result += bias;
    return result;
}

}  // namespace

void output_layer(act_t input[kNumSamples][kInputSize],
                  float output[kNumSamples][kOutputSize]) {
#pragma HLS ARRAY_PARTITION variable=outm_W complete dim=3
#pragma HLS ARRAY_PARTITION variable=outm_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=outm_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=outm_SH type=rom_1p impl=lutram
    ap_int<20> oarr[kNumSamples][kOutputSize];
#pragma HLS ARRAY_PARTITION variable=oarr cyclic factor=8 dim=2

    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        ap_int<7> quantizedInput[kInputSize];
        ap_int<20> outputAccumulator[kOutputSize];

        for (int featureIndex = 0; featureIndex < kInputSize; ++featureIndex) {
#pragma HLS UNROLL
            quantizedInput[featureIndex] = static_cast<ap_int<7>>(input[sampleIndex][featureIndex]);
        }

        BSMM::bitSerialVectorMatrixMult<kOutputSize, kInputSize, 7, 7, 20>(
            quantizedInput, outm_W[0], outputAccumulator);

        for (int outputIndex = 0; outputIndex < kOutputSize; ++outputIndex) {
#pragma HLS UNROLL
            oarr[sampleIndex][outputIndex] = requantize_output(
                outputAccumulator[outputIndex],
                outm_M0[0][outputIndex],
                outm_SH[0][outputIndex],
                outm_B[0][outputIndex]);
        }
    }

    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        for (int outputIndex = 0; outputIndex < kOutputSize; outputIndex += 8) {
#pragma HLS PIPELINE II=1
            for (int lane = 0; lane < 8; ++lane) {
#pragma HLS UNROLL
                output[sampleIndex][outputIndex + lane] =
                    static_cast<float>(oarr[sampleIndex][outputIndex + lane]) * 0.0002441506f;
            }
        }
    }
}
