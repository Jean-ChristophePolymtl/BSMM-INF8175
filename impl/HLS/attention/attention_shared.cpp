// Attention PARTAGÉE | S=4 H=2 HD=16 | U=4 | index couche L
#include "bitSerialMatMult.h"
#include "attn_in.h"
#include "attn_out.h"
#include "layer_tables.h"
#include "transformer_top.h"
#include "int_math.h"
#include "ap_int.h"

namespace {

constexpr int kNumSamples = 4;
constexpr int kEmbeddingSize = 32;
constexpr int kQkvSize = 96;
constexpr int kOutputSize = 32;

ap_int<7> quantize_attention_input(const act_t value, const int threshold) {
    const ap_int<20> shiftedValue = (value + 4) >> 3;

    if (shiftedValue > threshold) {
        return static_cast<ap_int<7>>(threshold);
    }
    if (shiftedValue < -threshold) {
        return static_cast<ap_int<7>>(-threshold);
    }

    return static_cast<ap_int<7>>(shiftedValue);
}

ap_int<20> requantize_projection_output(
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

void attention_shared(int L, act_t input[kNumSamples][kEmbeddingSize],
                      act_t output[kNumSamples][kEmbeddingSize]) {
#pragma HLS ARRAY_RESHAPE variable=attn_in_W cyclic factor=32 dim=3
#pragma HLS ARRAY_PARTITION variable=attn_in_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=attn_in_W type=rom_2p impl=bram
#pragma HLS BIND_STORAGE variable=attn_in_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=attn_in_SH type=rom_1p impl=lutram
#pragma HLS ARRAY_PARTITION variable=attn_out_W complete dim=3
#pragma HLS ARRAY_PARTITION variable=attn_out_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=attn_out_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=attn_out_SH type=rom_1p impl=lutram
    act_t qkv[kNumSamples][kQkvSize];
    ap_int<7> ctx_q[kNumSamples][kEmbeddingSize];
#pragma HLS ARRAY_PARTITION variable=qkv   cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=ctx_q cyclic factor=32 dim=2

    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        ap_int<7> quantizedInput[kEmbeddingSize];
        ap_int<20> qkvAccumulator[kQkvSize];

        for (int featureIndex = 0; featureIndex < kEmbeddingSize; ++featureIndex) {
#pragma HLS UNROLL
            quantizedInput[featureIndex] =
                quantize_attention_input(input[sampleIndex][featureIndex], A_THDIN[L]);
        }

        BSMM::bitSerialVectorMatrixMult<kQkvSize, kEmbeddingSize, 7, 7, 20>(
            quantizedInput, attn_in_W[L], qkvAccumulator);

        for (int qkvIndex = 0; qkvIndex < kQkvSize; ++qkvIndex) {
#pragma HLS UNROLL
            qkv[sampleIndex][qkvIndex] = requantize_projection_output(
                qkvAccumulator[qkvIndex],
                attn_in_M0[L][qkvIndex],
                attn_in_SH[L][qkvIndex],
                attn_in_B[L][qkvIndex]);
        }
    }

    // ═══ SDP entier partagé : M0s/SHs et thd_ctx indexés par L ═══
    ap_int<38> sc[2][4][4];
    ap_int<18> eq[2][4][4];
    ap_int<34> inv_y[2][4];
    ap_uint<6> inv_e[2][4];
#pragma HLS ARRAY_PARTITION variable=sc complete dim=3
#pragma HLS ARRAY_PARTITION variable=eq complete dim=3

    SC_A: for (int h = 0; h < 2; h++)
      for (int qi = 0; qi < 4; qi++)
        for (int kj = 0; kj < 4; kj++) {
#pragma HLS PIPELINE II=1
          int qo = h*16, ko = 32+h*16;
          ap_int<38> a = 0;
          for (int d = 0; d < 16; d++)
#pragma HLS UNROLL
            a += (ap_int<38>)qkv[qi][qo+d]*(ap_int<38>)qkv[kj][ko+d];
          sc[h][qi][kj] = a;
        }

    SM_B: for (int h = 0; h < 2; h++)
      for (int qi = 0; qi < 4; qi++) {
#pragma HLS PIPELINE II=4
        ap_int<38> mx = sc[h][qi][0];
        for (int kj = 1; kj < 4; kj++)
#pragma HLS UNROLL
          if (sc[h][qi][kj] > mx) mx = sc[h][qi][kj];
        ap_uint<40> sum = 1;
        for (int kj = 0; kj < 4; kj++) {
#pragma HLS UNROLL
          ap_int<18> e2v = exp2_q((ap_int<40>)(sc[h][qi][kj]-mx),
                                  SDP_M0S[L], SDP_SHS[L]);
          eq[h][qi][kj] = e2v;
          sum += (ap_uint<40>)e2v;
        }
        recip_q(sum, inv_y[h][qi], inv_e[h][qi]);
      }

    CTX_C: for (int h = 0; h < 2; h++)
      for (int qi = 0; qi < 4; qi++)
        for (int d0 = 0; d0 < 16; d0 += 4) {
#pragma HLS PIPELINE II=1
          int vo = 64+h*16;
          int tc_ = A_THDCTX[L];
          for (int w = 0; w < 4; w++) {
#pragma HLS UNROLL
            int d = d0 + w;
            ap_int<40> acc = 0;
            for (int kj = 0; kj < 4; kj++)
#pragma HLS UNROLL
              acc += (ap_int<40>)eq[h][qi][kj] * qkv[kj][vo+d];
            ap_int<74> pr = (ap_int<74>)acc * inv_y[h][qi];
            int sh = 30 + (int)inv_e[h][qi];
            ap_int<40> v = (ap_int<40>)((pr + ((ap_int<74>)1 << (sh-1))) >> sh);
            ctx_q[qi][h*16+d] = v > tc_ ? (ap_int<7>)tc_
                : (v < -tc_ ? (ap_int<7>)(-tc_)
                : (ap_int<7>)v);
          }
        }

    for (int sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
        ap_int<20> outputAccumulator[kOutputSize];
        BSMM::bitSerialVectorMatrixMult<kOutputSize, kEmbeddingSize, 7, 7, 20>(
            ctx_q[sampleIndex], attn_out_W[L], outputAccumulator);

        for (int outputIndex = 0; outputIndex < kOutputSize; ++outputIndex) {
#pragma HLS UNROLL
            output[sampleIndex][outputIndex] = requantize_projection_output(
                outputAccumulator[outputIndex],
                attn_out_M0[L][outputIndex],
                attn_out_SH[L][outputIndex],
                attn_out_B[L][outputIndex]);
        }
    }
}
