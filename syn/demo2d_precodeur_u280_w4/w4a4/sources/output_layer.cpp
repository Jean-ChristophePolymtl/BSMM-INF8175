// Output (non partagé) | dequant ×SOUT=0.0142436 | U=4
#include "transformer_top.h"
#include "outm.h"
#include "ap_int.h"

void output_layer(act_t input[4][32], float output[4][32]) {
#pragma HLS ARRAY_PARTITION variable=outm_W complete dim=3
#pragma HLS ARRAY_PARTITION variable=outm_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=outm_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=outm_SH type=rom_1p impl=lutram
    ap_int<14> oarr[4][32];
#pragma HLS ARRAY_PARTITION variable=oarr cyclic factor=8 dim=2
    M_outm: for (int s = 0; s < 4; s++)
      for (int ii = 0; ii < 32; ii += 4) {
#pragma HLS PIPELINE II=1
        for (int u = 0; u < 4; u++) {
#pragma HLS UNROLL
          ap_int<14> acc = 0;
          for (int j0 = 0; j0 < 1; j0++)
            for (int jp = 0; jp < 32; jp++) {
#pragma HLS UNROLL
              int j = j0*32 + jp;
              if (j < 32) {
              ap_int<4> xv = input[s][j];
              acc += (ap_int<14>)outm_W[0][ii+u][j] * xv;
              }
            }
          ap_int<30> tmp = (ap_int<30>)acc * (ap_uint<17>)outm_M0[0][ii+u];
          int shv = outm_SH[0][ii+u];
          ap_int<14> y = (shv > 0)
              ? (ap_int<14>)((tmp + ((ap_int<30>)1 << (shv-1))) >> shv)
              : (ap_int<14>)tmp;
          y += outm_B[0][ii+u];
          oarr[s][ii+u] = y;
        }
      }

    DQ: for (int s = 0; s < 4; s++)
      for (int j = 0; j < 32; j += 8) {
#pragma HLS PIPELINE II=1
        for (int w = 0; w < 8; w++)
#pragma HLS UNROLL
          output[s][j+w] = (float)oarr[s][j+w] * 0.0142435589f;
      }
}
