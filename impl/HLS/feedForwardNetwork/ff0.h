// ff0 EMPILÉ | [2][128][32] | W<7> acc 20b | bram
#pragma once
#include "ap_int.h"

typedef ap_int<7> wi_ff0_t;
extern const wi_ff0_t ff0_W[2][128][32];
extern const int ff0_M0[2][128];
extern const int ff0_SH[2][128];
extern const int ff0_B[2][128];
