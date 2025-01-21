#include <union_find_aos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mt_random.h>

#define WORLD_WIDTH 1000
#define WORLD_HEIGHT 1000
#define WORLD_SIZE (WORLD_WIDTH * WORLD_HEIGHT)

#define PERC_STEP_SIZE 10000000
#define PERC_NR_SAMPLES 10

typedef struct Context_ {
    mt_state rand_state;
    DsufNode* nodes;
    uint32_t* horiz_prob;
    uint32_t* vert_prob;
    uint_fast8_t* checked;
} Context;

int init(Context* cx) {
    cx->nodes = calloc(WORLD_SIZE, sizeof(DsufNode));
    if (!cx->nodes) goto FAILURE;

    cx->horiz_prob = malloc(WORLD_SIZE * sizeof(uint32_t));
    if (!cx->horiz_prob) goto FAILURE;

    cx->vert_prob = malloc(WORLD_SIZE * sizeof(uint32_t));
    if (!cx->vert_prob) goto FAILURE;

    cx->checked = calloc(WORLD_SIZE, sizeof(uint_fast8_t));
    if (!cx->checked) goto FAILURE;

    mt_initialize_state(&cx->rand_state, 543254);
    return 1;
FAILURE:
    free(cx->nodes);
    free(cx->horiz_prob);
    free(cx->vert_prob);
    free(cx->checked);
    return 0;
}

uint32_t sample(Context* cx) {
    uint32_t* horiz_prob = cx->horiz_prob;
    uint32_t* vert_prob = cx->vert_prob;
    DsufNode* nodes = cx->nodes;
    uint_fast8_t* checked = cx->checked;
    mt_state* rand_state = &cx->rand_state;

    for (size_t i = 0; i < WORLD_SIZE; ++i) {
        horiz_prob[i] = mt_random_uint32(rand_state);
    }
    
    for (size_t i = 0; i < WORLD_SIZE; ++i) {
        vert_prob[i] = mt_random_uint32(rand_state);
    }
 
    for (size_t i = 0; i < WORLD_SIZE; ++i) {
        cx->nodes[i].parent = &cx->nodes[i];
    }

    DsufNode src, sink;
    memset(&src, 0, sizeof(DsufNode));
    memset(&sink, 0, sizeof(DsufNode));
    src.parent = &src;
    sink.parent = &sink;

    /* init connections */
    for (size_t i = 1; i < WORLD_WIDTH-1; ++i) {
        ds_union(&src, &nodes[i+0*WORLD_WIDTH]);
    }
    
    for (size_t i = 1; i < WORLD_WIDTH-1; ++i) {
        ds_union(&sink, &nodes[i+(WORLD_HEIGHT-1)*WORLD_WIDTH]);
    }

    for (uint32_t lvl = 0; lvl < UINT32_MAX - 2*PERC_STEP_SIZE;
            lvl += PERC_STEP_SIZE) {
        
        for (size_t y = 0; y < WORLD_HEIGHT-1; ++y) {
            for (size_t x = 0; x < WORLD_WIDTH-1; ++x) {
                if (!checked[x+y*WORLD_WIDTH] &&
                        horiz_prob[x+y*WORLD_WIDTH] < lvl) {
                    DsufNode* restrict a = &nodes[x+y*WORLD_WIDTH];
                    DsufNode* restrict b = &nodes[x+1+y*WORLD_WIDTH];
                    ds_union(a, b);
                }
            }
        }
        for (size_t y = 0; y < WORLD_HEIGHT-1; ++y) {
            for (size_t x = 0; x < WORLD_WIDTH-1; ++x) {
                if (!checked[x+y*WORLD_WIDTH] &&
                        vert_prob[x+y*WORLD_WIDTH] < lvl) {
                    DsufNode* restrict a = &nodes[x+y*WORLD_WIDTH];
                    DsufNode* restrict b = &nodes[x+(y+1)*WORLD_WIDTH];
                    ds_union(a, b);
                }
            }
        }
 
        if (ds_find(&src) == ds_find(&sink)) {
            return lvl;
        }
    }

    return UINT32_MAX;
}

void finish(Context* cx) {
    free(cx->nodes);
    free(cx->horiz_prob);
    free(cx->vert_prob);
    free(cx->checked);
}

int main() {
    Context cx;
    if (!init(&cx)) {
        return EXIT_FAILURE;
    }
    
    uint32_t samples[PERC_NR_SAMPLES];
    for (size_t i = 0; i < PERC_NR_SAMPLES; ++i) {
        samples[i] = sample(&cx);
    }

    uint64_t sum = 0;
    for (size_t i = 0; i < PERC_NR_SAMPLES; ++i) {
        sum += samples[i];
    }

    printf("%f\n", (float)(sum / PERC_NR_SAMPLES) / UINT32_MAX);

    finish(&cx);
    return EXIT_SUCCESS;
}
