// attn_in EMPILÉ | [2][96][32] | W<7> acc 20b | bram
#pragma once
#include "ap_int.h"

typedef ap_int<7> wi_attn_in_t;
extern const wi_attn_in_t attn_in_W[2][96][32];
extern const int attn_in_M0[2][96];
extern const int attn_in_SH[2][96];
extern const int attn_in_B[2][96];
