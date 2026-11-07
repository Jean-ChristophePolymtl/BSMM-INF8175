// ff0 EMPILÉ | [2][128][32] | W<4> acc 14b | bram
#pragma once
#include "ap_int.h"

typedef ap_int<4> wi_ff0_t;
extern const wi_ff0_t ff0_W[2][128][32];
extern const int ff0_M0[2][128];
extern const int ff0_SH[2][128];
extern const int ff0_B[2][128];
