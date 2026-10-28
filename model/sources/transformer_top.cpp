// Top PARTAGÉ v5 : 1 attention + 1 ffn + 2 glues, boucle L, ping-pong
// Fix v4 : aw/fw partition W_GLUE (glues lisent 8/cy) ; xa/xb/nw factor D (MACs)
#include "transformer_top.h"
#include "ap_int.h"
#include "ap_fixed.h"

void transformer_top(input_t input_flat[128], float output_flat[128])
{
#pragma HLS INTERFACE m_axi     port=input_flat  offset=slave bundle=gmem depth=128
#pragma HLS INTERFACE m_axi     port=output_flat offset=slave bundle=gmem depth=128
#pragma HLS INTERFACE s_axilite port=input_flat  bundle=control
#pragma HLS INTERFACE s_axilite port=output_flat bundle=control
#pragma HLS INTERFACE s_axilite port=return      bundle=control

    act_t xa[SEQ_LEN][EMB_DIM], xb[SEQ_LEN][EMB_DIM];
#pragma HLS ARRAY_PARTITION variable=xa cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=xb cyclic factor=32 dim=2
    input_t in2d[SEQ_LEN][TOKEN_DIM];
#pragma HLS ARRAY_PARTITION variable=in2d cyclic factor=8 dim=2

    UP: for (int s = 0; s < SEQ_LEN; s++)
      for (int j = 0; j < TOKEN_DIM; j += 8) {
#pragma HLS PIPELINE II=1
        for (int w = 0; w < 8; w++)
#pragma HLS UNROLL
          in2d[s][j+w] = input_flat[s*TOKEN_DIM+j+w];
      }
    embedding_layer(in2d, xa);

    LOOP_L: for (int L = 0; L < NUM_LAYERS; L++) {
        act_t aw[SEQ_LEN][EMB_DIM], nw[SEQ_LEN][EMB_DIM];
        act_t fw[SEQ_LEN][EMB_DIM];
#pragma HLS ARRAY_PARTITION variable=aw cyclic factor=8 dim=2
#pragma HLS ARRAY_PARTITION variable=nw cyclic factor=32 dim=2
#pragma HLS ARRAY_PARTITION variable=fw cyclic factor=8 dim=2
        if (L % 2 == 0) {
            attention_shared(L, xa, aw);
            rmsglue_shared_1(L, xa, aw, nw);
            ffn_shared(L, nw, fw);
            rmsglue_shared_2(L, nw, fw, xb);
        } else {
            attention_shared(L, xb, aw);
            rmsglue_shared_1(L, xb, aw, nw);
            ffn_shared(L, nw, fw);
            rmsglue_shared_2(L, nw, fw, xa);
        }
    }

    float o2d[SEQ_LEN][OUTPUT_DIM];
#pragma HLS ARRAY_PARTITION variable=o2d cyclic factor=8 dim=2
    output_layer(xa, o2d);
    PK: for (int s = 0; s < SEQ_LEN; s++)
      for (int j = 0; j < OUTPUT_DIM; j += 8) {
#pragma HLS PIPELINE II=1
        for (int w = 0; w < 8; w++)
#pragma HLS UNROLL
          output_flat[s*OUTPUT_DIM+j+w] = o2d[s][j+w];
      }
}
