#include "bitSerialMatMult.h"

#include <iostream>
#include <string>
#include <ap_int.h>

constexpr auto M{2};
constexpr auto K{2};
constexpr auto N{2};
constexpr auto L_BITS{4};
constexpr auto R_BITS{4};

ap_int<L_BITS> matrixL [M][K] = {{{1},{2}},{{3},{4}}};
ap_int<R_BITS> matrixR [K][N] = {{{1},{2}},{{3},{4}}};
ap_int<L_BITS+R_BITS> matrixP[M][N];

int main(){

    std::cout << "Input matrices " << std::endl << std::endl;

    for(auto i{0}; i < M; i++){
        for(auto j{0}; j < K; j++){
                std::cout << matrixL[i][j] << " ";
        }
        std::cout << std::endl;
    }

        std::cout << std::endl;
        std::cout << std::endl;

    for(auto i{0}; i < K; i++){
        for(auto j{0}; j < N; j++){
                std::cout << matrixR[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;

    BSMM::bitSerialMatrixMult<M, K, N, L_BITS, R_BITS>(matrixL, matrixR, matrixP);

    std::cout << "Output Matrix is: " << std::endl << std::endl;

    for(auto i{0}; i < M; i++){
        for(auto j{0}; j < N; j++){
                std::cout << matrixP[i][j] << " ";
            
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}