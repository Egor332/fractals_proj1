// Draft tests for percolation in 2D grid

#include "mt_random.h"
#include <stdio.h>
#include <stdlib.h>

// width and height
#define M 100
#define N 100

// nr of test samples 
#define K 50

// nr of attempts per sample
#define L 100

typedef uint_fast8_t uf8;

// values:
// 0 blocked
// 1 free
// 2 filled

// fill the field by random blocked cells
void fill_field(uf8 field[N*M], int k, mt_state* mt) {
    // TODO: perf
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            uint32_t r = mt_random_uint32(mt);
            uint32_t t = UINT32_MAX / K * k;
            field[i*M+j] = (r < t);
        }
    }
}

// Fill from top to bottom.
// 
// Rules:
// 1. Water goes down, left and right
// 2. Water can't bypass blocked cells
//
// Return 1 if the bottom contains water
int do_percolation(uf8 field[N*M]) {
    for (int j = 0; j < M; ++j) {
        if (field[0*M+j])
            field[0*M+j] = 2;
    }

    for (int i = 1; i < N; ++i) { 
        if (field[(i-1)*M+0] == 2) {
            if (field[i*M+0]) field[i*M+0] = 2;
        }

        int succ = 0;

        for (int j = 1; j < M; ++j) {
            if (field[(i-1)*M+j] == 2 || field[i*M+j-1] == 2) {
                if (field[i*M+j]) {
                    succ = 1;
                    field[i*M+j] = 2;
                }
            }
        }

        if (!succ) return 0;

        for (int j = M-2; j >= 0; --j) {
            if (field[i*M+j+1] == 2) {
                if (field[i*M+j]) field[i*M+j] = 2;
            }
        }
    }

    for (int j = 0; j < M; ++j) {
        if (field[(N-1)*M+j] == 2) {
            return 1;
        }
    }

    return 0;
}

int main() {
    uf8 field[N*M];
    size_t nr_succ[K];

    // prepare random
    mt_state mt;
    mt_initialize_state(&mt, 30144);

    // for K samples
    for (int k = 0; k < K; ++k) {
        nr_succ[k] = 0;

        // do L attempts
        for (int l = 0; l < L; ++l) {
            // test percolation
            fill_field(field, k, &mt);
            int succ = do_percolation(field);

            if (succ)
                ++nr_succ[k];
        }
    }

    // results
    printf("Blocked cells ratio -> nr succeeded tests per nr all tests\n");
    for (int k = 0; k < K; ++k) {
        printf("%lf -> %lf\n", (double)k / K, (double)nr_succ[k] / L);
    }

    //for (int i = 0; i < 10; ++i) {
    //    printf("%lu\n", mt_random_uint32(&mt));
    //}

    //size_t N;
    //scanf("%lu", &N);

    /*size_t* ms = malloc(sizeof(size_t)*n);
    size_t* ns = malloc(sizeof(size_t)*n);

    if (!ms || !ns) {
        free(ms);
        free(ns);
        return 1;
    }*/

    return 0;
}
