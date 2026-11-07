// ff1 EMPILÉ | [2][32][128] | W<4> acc 15b | bram
#pragma once
#include "ap_int.h"

typedef ap_int<4> wi_ff1_t;
extern const wi_ff1_t ff1_W[2][32][128];
extern const int ff1_M0[2][32];
extern const int ff1_SH[2][32];
extern const int ff1_B[2][32];
