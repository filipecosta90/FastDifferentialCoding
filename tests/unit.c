#include <stdio.h>
#include <stdlib.h>

#include "fastdelta.h"

int main() {
    int N = 5;
    uint64_t * datain = malloc(N * sizeof(uint64_t));
    uint64_t * buffer = malloc(N * sizeof(uint64_t));
    uint64_t * recovdata = malloc(N * sizeof(uint64_t));
    datain[0] = 10;
    datain[1] = 15;
    datain[2] = 25;
    datain[3] = 35;
    datain[4] = 45;
    for (size_t i = 0; i < N; i++)
    {
        printf("datain[%d]=%d\n",i,datain[i]);
    }
    _ddelta_enc64(datain,N,buffer);
    printf("calculate deltas of deltas:\n");
    for (size_t i = 0; i < N; i++)
    {
        printf("buffer[%d]=%d\n",i,buffer[i]);
    }
    printf("recovered data:\n");
    _ddelta_dec64(buffer,N,recovdata);
     for (size_t i = 0; i < N; i++)
    {
        printf("recovdata[%d]=%d\n",i,recovdata[i]);
    }
    

    // for (int length = 0; length <= N;) {
    //     printf("length = %d \n", length);
    //     for (uint32_t gap = 1; gap <= 387420489; gap *= 3) {
    //         for (int k = 0; k < length; ++k)
    //             datain[k] = gap * (k+1);
    //         compute_deltas(datain, length, buffer, 0);
    //         for (int k = 0; k < length; ++k) {
    //             if(buffer[k] != gap) {
    //                 printf("bug. buffer[%d]=%d expected %d \n",k,buffer[k],gap);
    //                 return -1;
    //             }
    //         }

    //         compute_prefix_sum(buffer, length, recovdata, 0);
    //         for(int k = 0; k < length; ++k) {
    //             if(datain[k] != recovdata[k]) {
    //                 printf("bug.\n");
    //                 return -1;
    //             }
    //         }
    //         compute_deltas_inplace(datain, length, 0);
    //         for (int k = 0; k < length; ++k) {
    //             if(datain[k] != gap) {
    //                 printf("bug.");
    //                 return -1;
    //             }
    //         }
    //         compute_prefix_sum_inplace(datain, length, 0);
    //         for(int k = 0; k < length; ++k) {
    //             if(datain[k] != recovdata[k]) {
    //                 printf("bug.\n");
    //                 return -1;
    //             }
    //         }
    //     }

    //     if (length < 128)
    //         ++length;
    //     else {
    //         length *= 2;
    //     }
    // }
    free(datain);
    free(buffer);
    free(recovdata);
    printf("Code looks good.\n");
    return 0;
}
