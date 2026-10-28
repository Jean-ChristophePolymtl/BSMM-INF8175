// Attention PARTAGÉE | S=4 H=2 HD=16 | U=4 | index couche L
#include "attn_in.h"
#include "attn_out.h"
#include "layer_tables.h"
#include "transformer_top.h"
#include "int_math.h"
#include "ap_int.h"
#include "ap_fixed.h"

void attention_shared(int L, act_t input[4][32], act_t output[4][32]) {
#pragma HLS ARRAY_RESHAPE variable=attn_in_W cyclic factor=32 dim=3
#pragma HLS ARRAY_PARTITION variable=attn_in_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=attn_in_W type=rom_2p impl=bram
#pragma HLS BIND_STORAGE variable=attn_in_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=attn_in_SH type=rom_1p impl=lutram
#pragma HLS ARRAY_PARTITION variable=attn_out_W complete dim=3
#pragma HLS ARRAY_PARTITION variable=attn_out_W cyclic factor=4 dim=2
#pragma HLS BIND_STORAGE variable=attn_out_M0 type=rom_1p impl=lutram
#pragma HLS BIND_STORAGE variable=attn_out_SH type=rom_1p impl=lutram
    act_t qkv[4][96];
    ap_int<7> ctx_q[4][32];
#pragma HLS ARRAY_PARTITION variable=qkv   cyclic factor=16 dim=2
#pragma HLS ARRAY_PARTITION variable=ctx_q cyclic factor=32 dim=2

    M_attn_in: for (int s = 0; s < 4; s++)
      for (int ii = 0; ii < 96; ii += 4) {
#pragma HLS PIPELINE II=1
        for (int u = 0; u < 4; u++) {
#pragma HLS UNROLL
          ap_int<20> acc = 0;
          for (int j0 = 0; j0 < 1; j0++)
            for (int jp = 0; jp < 32; jp++) {
#pragma HLS UNROLL
              int j = j0*32 + jp;
              if (j < 32) {
              ap_int<20> xw_ = input[s][j];
              ap_int<20> xm_ = (xw_ + 4) >> 3;
              int th_ = A_THDIN[L];
              ap_int<7> xv = xm_ >  th_ ? (ap_int<7>)th_
                        : (xm_ < -th_ ? (ap_int<7>)(-th_) : (ap_int<7>)xm_);
              acc += (ap_int<20>)attn_in_W[L][ii+u][j] * xv;
              }
            }
          ap_int<36> tmp = (ap_int<36>)acc * (ap_uint<17>)attn_in_M0[L][ii+u];
          int shv = attn_in_SH[L][ii+u];
          ap_int<20> y = (shv > 0)
              ? (ap_int<20>)((tmp + ((ap_int<36>)1 << (shv-1))) >> shv)
              : (ap_int<20>)tmp;
          y += attn_in_B[L][ii+u];
          qkv[s][ii+u] = y;
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

    M_attn_out: for (int s = 0; s < 4; s++)
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
              ap_int<7> xv = ctx_q[s][j];
              acc += (ap_int<20>)attn_out_W[L][ii+u][j] * xv;
              }
            }
          ap_int<36> tmp = (ap_int<36>)acc * (ap_uint<17>)attn_out_M0[L][ii+u];
          int shv = attn_out_SH[L][ii+u];
          ap_int<20> y = (shv > 0)
              ? (ap_int<20>)((tmp + ((ap_int<36>)1 << (shv-1))) >> shv)
              : (ap_int<20>)tmp;
          y += attn_out_B[L][ii+u];
          output[s][ii+u] = y;
        }
      }
}
