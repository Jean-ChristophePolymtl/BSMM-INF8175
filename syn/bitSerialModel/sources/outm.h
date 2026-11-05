// outm EMPILÉ | [1][32][32] | W<7> acc 20b | lutram
#pragma once
#include "ap_int.h"

typedef ap_int<7> wi_outm_t;
extern const wi_outm_t outm_W[1][32][32];
extern const int outm_M0[1][32];
extern const int outm_SH[1][32];
extern const int outm_B[1][32];
