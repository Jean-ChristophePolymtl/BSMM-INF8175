#include "bitSerialMatMult.h"

#include <ap_int.h>

namespace BSMM{

void bitSerialMatrixMult_top(   ap_int<16> matrixL [32][128], 
                                ap_int<16> matrixR [128][32], 
                                ap_int<32> matrixP[32][32]){

    #pragma HLS INTERFACE m_axi port=matrixL offset=slave bundle=gmem0 depth=32*128
    #pragma HLS INTERFACE m_axi port=matrixR offset=slave bundle=gmem1 depth=128*32
    #pragma HLS INTERFACE m_axi port=matrixP offset=slave bundle=gmem2 depth=32*32

    #pragma HLS INTERFACE s_axilite port=matrixL bundle=control
    #pragma HLS INTERFACE s_axilite port=matrixR bundle=control
    #pragma HLS INTERFACE s_axilite port=matrixP bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    bitSerialMatrixMult<32,128,32,16,16>(matrixL, matrixR, matrixP);

}

}