// emb EMPILÉ | [1][32][32] | W<4> acc 15b | lutram
#pragma once
#include "ap_int.h"

typedef ap_int<4> wi_emb_t;
extern const wi_emb_t emb_W[1][32][32];
extern const int emb_M0[1][32];
extern const int emb_SH[1][32];
extern const int emb_B[1][32];
