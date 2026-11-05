// attn_out EMPILÉ | [2][32][32] | W<7> acc 20b | lutram
#pragma once
#include "ap_int.h"

typedef ap_int<7> wi_attn_out_t;
extern const wi_attn_out_t attn_out_W[2][32][32];
extern const int attn_out_M0[2][32];
extern const int attn_out_SH[2][32];
extern const int attn_out_B[2][32];
