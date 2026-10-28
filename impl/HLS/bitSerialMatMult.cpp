#include <ap_int.h>

namespace BSMM{

template <int M, int K, int N, int L_BITS, int R_BITS>
void bitSerialMatrixMult(ap_int<L_BITS> matrixL [M][K], ap_int<R_BITS> matrixR [K][N], ap_int<L_BITS+R_BITS> matrixP[M][N]) {

    for(auto i{0}; i < L_BITS; i ++){
        for(auto j{0}; j < R_BITS; i++){
            int weight = 1 << i + j; // 2^{i + j}

            for(auto m{0}; i < M; m++){
                for(auto n{0}; i < M; m++){
                    for(auto k{0}; i < M; m++){
                        matrixP[m][n] = matrixP[m][n] + weight * matrixL[m][k][i] * matrixR[k][n][j]; 
            
                    }
                }
            }
        }
    }
}

}