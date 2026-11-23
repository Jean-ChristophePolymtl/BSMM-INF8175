// TB PARTAGÉ v5 | 0 LSB attendu partout
#include <stdio.h>
#include <math.h>
#include "transformer_top.h"
#include "hls_test_vectors.h"
#include "ap_int.h"

static void CI(const char* n, act_t h[SEQ_LEN][EMB_DIM], const int* ref, int){
    int fail=0, mx=0; double av=0;
    for(int s=0;s<SEQ_LEN;s++)for(int j=0;j<EMB_DIM;j++){
        int d = (int)h[s][j]-ref[s*EMB_DIM+j]; if(d<0)d=-d;
        av+=d; if(d>mx)mx=d; if(d>0)fail++;
    }
    av/=(SEQ_LEN*EMB_DIM);
    printf("%-16s avg=%.3f max=%d fail=%d/%d %s\n", n, av, mx, fail,
           SEQ_LEN*EMB_DIM, fail==0?"ok":"WARN");
}
static float cosim(const float*a,const float*b,int N){
    float d=0,na=0,nb=0;
    for(int j=0;j<N;j++){d+=a[j]*b[j];na+=a[j]*a[j];nb+=b[j]*b[j];}
    return d/(sqrtf(na)*sqrtf(nb)+1e-9f);}

int main() {
    input_t inf[128]; float outf[128];
    for(int i=0;i<128;i++) inf[i]=(input_t)test_input_flat[i];

    printf("\n=== T1 blocs partagés (0 LSB attendu) ===\n");
    {
        act_t xi[SEQ_LEN][EMB_DIM], o1[SEQ_LEN][EMB_DIM];
        act_t nw[SEQ_LEN][EMB_DIM], fw[SEQ_LEN][EMB_DIM], x2[SEQ_LEN][EMB_DIM];
        for(int s=0;s<SEQ_LEN;s++)for(int j=0;j<EMB_DIM;j++)
            xi[s][j]=(act_t)ref_emb_int[s*EMB_DIM+j];
        attention_shared(0, xi, o1);   CI("L0_attn",  o1, ref_attn_int_0, 1);
        rmsglue_shared_1(0, xi, o1, nw); CI("L0_glue1", nw, ref_nw_0, 1);
        ffn_shared(0, nw, fw);          CI("L0_ffn",   fw, ref_ffn_int_0, 1);
        rmsglue_shared_2(0, nw, fw, x2); CI("L0_glue2", x2, ref_x_int_0, 1);
    }
    {
        act_t xi[SEQ_LEN][EMB_DIM], o1[SEQ_LEN][EMB_DIM];
        act_t nw[SEQ_LEN][EMB_DIM], fw[SEQ_LEN][EMB_DIM], x2[SEQ_LEN][EMB_DIM];
        for(int s=0;s<SEQ_LEN;s++)for(int j=0;j<EMB_DIM;j++)
            xi[s][j]=(act_t)ref_x_int_0[s*EMB_DIM+j];
        attention_shared(1, xi, o1);   CI("L1_attn",  o1, ref_attn_int_1, 1);
        rmsglue_shared_1(1, xi, o1, nw); CI("L1_glue1", nw, ref_nw_1, 1);
        ffn_shared(1, nw, fw);          CI("L1_ffn",   fw, ref_ffn_int_1, 1);
        rmsglue_shared_2(1, nw, fw, x2); CI("L1_glue2", x2, ref_x_int_1, 1);
    }

    printf("\n=== T2 chaine ===\n");
    {
        act_t xc[SEQ_LEN][EMB_DIM], xc2[SEQ_LEN][EMB_DIM];
        act_t aw[SEQ_LEN][EMB_DIM], nw[SEQ_LEN][EMB_DIM], fw[SEQ_LEN][EMB_DIM];
        input_t i2[SEQ_LEN][TOKEN_DIM];
        for(int s=0;s<SEQ_LEN;s++)for(int j=0;j<TOKEN_DIM;j++)i2[s][j]=inf[s*TOKEN_DIM+j];
        embedding_layer(i2, xc);
        attention_shared(0, xc, aw);
        rmsglue_shared_1(0, xc, aw, nw);
        ffn_shared(0, nw, fw);
        rmsglue_shared_2(0, nw, fw, xc2);
        CI("chain_L0", xc2, ref_x_int_0, 1);
        attention_shared(1, xc2, aw);
        rmsglue_shared_1(1, xc2, aw, nw);
        ffn_shared(1, nw, fw);
        rmsglue_shared_2(1, nw, fw, xc);
        CI("chain_L1", xc, ref_x_int_1, 1);
    }

    printf("\n=== T3 e2e ===\n");
    transformer_top(inf, outf);
    int ok=1; float av=0;
    for(int s=0;s<4;s++){
        float c=cosim(outf+s*32, ref_output_flat+s*32, 32);
        av+=c; if(c<=0.999f)ok=0;
        printf("  user[%d]: cos=%+.5f %s\n",s,c,c>0.999f?"ok":"WARN");}
    printf("  Cosine moyen: %.5f %s\n",av/4,ok?"PASS":"FAIL");
    return ok?0:1;
}
