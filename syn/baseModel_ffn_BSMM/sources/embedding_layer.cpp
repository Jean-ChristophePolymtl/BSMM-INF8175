// Embedding (non partagé) | S=4 T=32 D=32 | U=4
#include "transformer_top.h"
#include "emb.h"
#include "ap_int.h"
#include "ap_fixed.h"

void embedding_layer(input_t input[4][TOKEN_DIM], act_t output[4][EMB_DIM]) {
#pragma HLS ARRAY_PARTITION variable=emb_W complete dim=3
#pragma HLS ARRAY_PARTITION variable=emb_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=emb_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=emb_SH type=rom_1p impl=lutram
    ap_int<7> x_q[4][32];
#pragma HLS ARRAY_PARTITION variable=x_q cyclic factor=32 dim=2
    QI: for (int s = 0; s < 4; s++)
      for (int j = 0; j < 32; j += 8) {
#pragma HLS PIPELINE II=1
        for (int w = 0; w < 8; w++) {
#pragma HLS UNROLL
          ap_fixed<26,26,AP_RND> r = (float)input[s][j+w] * 289.68524470f;
          ap_int<26> v = r;
          x_q[s][j+w] = v > 63 ? (ap_int<7>)63
                     : (v < -63 ? (ap_int<7>)(-63) : (ap_int<7>)v);
        }
      }

    M_emb: for (int s = 0; s < 4; s++)
      for (int ii = 0; ii < 32; ii += 4) {
#pragma HLS PIPELINE II=1
        for (int u = 0; u < 4; u++) {
#pragma HLS UNROLL
          ap_int<20> acc = 0;
          for (int j0 = 0; j0 < 1; j0++)
            for (int jp = 0; jp < 32; jp++) {
#pragma HLS UNROLL
              int j = j0*32 + jp;
              if (j < 32) {
              ap_int<7> xv = x_q[s][j];
              acc += (ap_int<20>)emb_W[0][ii+u][j] * xv;
              }
            }
          ap_int<36> tmp = (ap_int<36>)acc * (ap_uint<17>)emb_M0[0][ii+u];
          int shv = emb_SH[0][ii+u];
          ap_int<20> y = (shv > 0)
              ? (ap_int<20>)((tmp + ((ap_int<36>)1 << (shv-1))) >> shv)
              : (ap_int<20>)tmp;
          y += emb_B[0][ii+u];
          output[s][ii+u] = y;
        }
      }
}
