// Top PARTAGÉ v5 | S=4 D=32 NL=2 | RES_J=3 | iface=axi
#pragma once
#include "ap_fixed.h"
#include "ap_int.h"

#define NUM_LAYERS 2
#define TOKEN_DIM  32
#define EMB_DIM    32
#define OUTPUT_DIM 32
#define SEQ_LEN    4
#define RES_J      3

typedef ap_int<16> act_t;
typedef ap_fixed<24,2> input_t;

void embedding_layer(input_t i_[SEQ_LEN][TOKEN_DIM], act_t o_[SEQ_LEN][EMB_DIM]);
void attention_shared(int L, act_t i_[SEQ_LEN][EMB_DIM], act_t o_[SEQ_LEN][EMB_DIM]);
void ffn_shared(int L, act_t i_[SEQ_LEN][EMB_DIM], act_t o_[SEQ_LEN][EMB_DIM]);
void rmsglue_shared_1(int L, act_t a[SEQ_LEN][EMB_DIM], act_t b[SEQ_LEN][EMB_DIM], act_t o_[SEQ_LEN][EMB_DIM]);
void rmsglue_shared_2(int L, act_t a[SEQ_LEN][EMB_DIM], act_t b[SEQ_LEN][EMB_DIM], act_t o_[SEQ_LEN][EMB_DIM]);
void output_layer(act_t i_[SEQ_LEN][EMB_DIM], float o_[SEQ_LEN][OUTPUT_DIM]);
void transformer_top(input_t input_flat[128], float output_flat[128]);
