// emb EMPILÉ | [1][32][32] | W<7> acc 20b | lutram
#pragma once
#include "ap_int.h"

typedef ap_int<7> wi_emb_t;
extern const wi_emb_t emb_W[1][32][32];
extern const int emb_M0[1][32];
extern const int emb_SH[1][32];
extern const int emb_B[1][32];
