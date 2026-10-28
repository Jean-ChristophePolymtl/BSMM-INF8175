// rmsglue PARTAGÉ 2 : résiduel + RMSNorm INT + requant | Gq indexé [L]
#include "GQ2_all.h"
#include "layer_tables.h"
#include "transformer_top.h"
#include "int_math.h"

void rmsglue_shared_2(int L, act_t a[4][32], act_t b[4][32],
                            act_t out[4][32]) {
#pragma HLS ARRAY_PARTITION variable=GQ2_all cyclic factor=8 dim=2
    RG_S: for (int s = 0; s < 4; s++) {
        ap_int<17> r[32];
#pragma HLS ARRAY_PARTITION variable=r cyclic factor=8 dim=1
        ap_uint<48> ssq = 0;
        P1_2: for (int j = 0; j < 32; j += 8) {
#pragma HLS PIPELINE II=1
            ap_uint<48> part = 0;
            for (int w = 0; w < 8; w++) {
#pragma HLS UNROLL
                ap_int<17> rv = a[s][j+w] + b[s][j+w];
                r[j+w] = rv;
                part += (ap_uint<48>)((ap_int<40>)rv * rv);
            }
            ssq += part;
        }
        ap_int<34> y1; ap_uint<6> e2;
        rsqrt_q(ssq, y1, e2);
        int sh = 30 + (int)e2;
        P2_2: for (int j = 0; j < 32; j += 8) {
#pragma HLS PIPELINE II=1
            for (int w = 0; w < 8; w++) {
#pragma HLS UNROLL
                ap_int<68> t_mul = (ap_int<68>)r[j+w] * (ap_int<68>)y1;
                ap_int<54> t  = (ap_int<54>)(t_mul >> 16);
                ap_int<88> t2 = (ap_int<88>)t * (ap_int<88>)GQ2_all[L][j+w];
                ap_int<40> v  = (ap_int<40>)((t2 + ((ap_int<88>)1 << (sh-1))) >> sh);
                int cl_ = CLIP2[L];
                if (cl_ > 0) {
                    if (v >  cl_) v =  cl_;
                    if (v < -cl_) v = -cl_;
                }
                out[s][j+w] = (act_t)v;
            }
        }
    }
}
