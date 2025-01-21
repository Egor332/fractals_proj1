#include "update_callback.h"
#include <union_find_aos.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define BLOCKED_WIDTH (WORLD_WIDTH / 5)

void assign_3_4(uint32_t* x, uint32_t y) {
    *x = y;
}

#ifdef PP_UNIX
void* thread_func(void* void_args) {
    ThreadArgs* args = void_args;
    size_t first = args->first;
    size_t last_exc = first + args->n;
    uint32_t* restrict color = args->cx->color;
    uint32_t* restrict pixels = args->pixels;
    DsufNode* uf = args->cx->uf;
    while (1) {
        if (sem_wait(&args->semaphore_in) == -1) {
            return NULL;
        }
        if (args->finish) {
            return NULL;
        }
        uint32_t* px = &pixels[first];
        DsufNode* ufp = &uf[first];
        for (; px < pixels+last_exc; ++px, ++ufp) {
            DsufNode* node = ds_lazy_find(ufp); 
            uint32_t* restrict pixel = px;
            uint32_t* restrict col = &color[node - uf];
            assign_3_4(pixel, *col);
        }
        (void)sem_post(&args->semaphore_out);
    }
    return NULL;
}
#endif

int init_callback(uint32_t* pixels, struct Context* cx) {
    const size_t width = WORLD_WIDTH;
    const size_t height = WORLD_HEIGHT;
    cx->horiz_connection_prob = malloc(sizeof(float)*width*height);
    cx->vert_connection_prob = malloc(sizeof(float)*width*height);
    cx->color = malloc(width*height*4);
    cx->uf = calloc(width*height, sizeof(DsufNode));
    cx->level = 0;

#ifdef PP_UNIX
    memset(cx->thread, 0, THREAD_COUNT * sizeof(pthread_t));
#endif

    float* horiz_connection_prob = cx->horiz_connection_prob;
    float* vert_connection_prob = cx->vert_connection_prob;
    void* color = cx->color;
    DsufNode* uf = cx->uf;

    if (!horiz_connection_prob || !vert_connection_prob || !uf || !color) {
        goto FAILED;
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            horiz_connection_prob[x+y*width] = (float)rand()/RAND_MAX;
            vert_connection_prob[x+y*width] = (float)rand()/RAND_MAX;
            uf[x+y*width].parent = &uf[x+y*width];
            ((uint8_t*)color)[(x+y*width)*4+0] = 255;
            ((uint8_t*)color)[(x+y*width)*4+1] = 255;
            ((uint8_t*)color)[(x+y*width)*4+2] = 255;
            ((uint8_t*)color)[(x+y*width)*4+3] = 255;
        }
    }

    /* cover top */
    for (size_t x = 0; x < WORLD_WIDTH; ++x) {
        ((uint8_t*)color)[(x+0*width)*4+0] = 0;
        ((uint8_t*)color)[(x+0*width)*4+1] = 0;
        ((uint8_t*)color)[(x+0*width)*4+2] = 0;
    }

    /* close all odd bonds */
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < BLOCKED_WIDTH; ++x) {
            horiz_connection_prob[x+y*width] = 1;
            vert_connection_prob[x+y*width] = 1;
        }

        for (size_t x = WORLD_WIDTH - BLOCKED_WIDTH; x < WORLD_WIDTH; ++x) {
            horiz_connection_prob[x+y*width] = 1;
            vert_connection_prob[x+y*width] = 1;
        }
    }

#ifdef PP_UNIX
    size_t n = width*height;
    const size_t chunk_size = n / (THREAD_COUNT+1);

    ThreadArgs* thread_args = cx->thread_args;
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        thread_args[i].first = chunk_size * i;
        thread_args[i].n = chunk_size;
        thread_args[i].pixels = pixels;
        thread_args[i].cx = cx;

        thread_args[i].finish = 0;
        (void)sem_init(&thread_args[i].semaphore_in, 0, 0);
        (void)sem_init(&thread_args[i].semaphore_out, 0, 0);
        if (pthread_create(&cx->thread[i], NULL, thread_func, &thread_args[i])) {
            goto FAILED;
        }
    }
#endif

    return 1;
FAILED:
    finish_callback(pixels, cx);
    return 0;
}

int finish_callback(uint32_t* pixels, struct Context* cx) {
#ifdef PP_UNIX
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        cx->thread_args[i].finish = 1;
        (void)sem_post(&cx->thread_args[i].semaphore_in);
        (void)pthread_join(cx->thread[i], NULL);
        (void)sem_destroy(&cx->thread_args[i].semaphore_in);
        (void)sem_destroy(&cx->thread_args[i].semaphore_out);
    }
#endif

    free(cx->horiz_connection_prob);
    free(cx->vert_connection_prob);
    free(cx->uf);
    free(cx->color);
    return 1;
}

float update_level(size_t lvl) {
    const float a = 0.49f;
    const float b = 0.1f;
    float x = (float) lvl / PERC_MAX_LVL;
    if (x < b) {
        return x*a/b;
    } else if (x >= b && x < 1-b) {
        return (1-2*a)/(1-2*b)*(x-b)+a;
    } else {
        return a/b*(x-1+b)+1-a;
    }
}

int update_callback(int pause, uint32_t* pixels_in, struct Context* cx) {
    const size_t width = WORLD_WIDTH;
    const size_t height = WORLD_HEIGHT;
    float* restrict horiz_connection_prob = cx->horiz_connection_prob;
    float* restrict vert_connection_prob = cx->vert_connection_prob;
    uint32_t* restrict color = cx->color;
    uint32_t* restrict pixels = pixels_in;
    DsufNode* uf = cx->uf;

    float lvl = update_level(cx->level);
    
    for (size_t y = 0; y < height-1; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (vert_connection_prob[x+y*width] < lvl) {
                DsufNode* restrict a = &uf[x+y*width];
                DsufNode* restrict b = &uf[x+(y+1)*width];
                uint32_t newcolor = (color[ds_find(a)-uf] & color[ds_find(b)-uf]);
                DsufNode* newnode = ds_union(a, b);
                color[newnode-uf] = newcolor;
            }
        }
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width-1; ++x) {
            if (horiz_connection_prob[x+y*width] < lvl) {
                DsufNode* restrict a = &uf[x+y*width];
                DsufNode* restrict b = &uf[(x+1)+y*width];
                uint32_t newcolor = (color[ds_find(a)-uf] & color[ds_find(b)-uf]);
                DsufNode* newnode = ds_union(a, b);
                color[newnode-uf] = newcolor;
            }
        }
    }
    
#ifdef PP_UNIX
    size_t n = width*height;

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        (void)sem_post(&cx->thread_args[i].semaphore_in);
    }

    const size_t chunk_size = n / (THREAD_COUNT+1);

    uint32_t* px = &pixels[chunk_size*THREAD_COUNT];
    DsufNode* ufp = &uf[chunk_size*THREAD_COUNT];
    for (; px < pixels+n; ++px, ++ufp) {
        DsufNode* node = ds_lazy_find(ufp); 
        uint32_t* restrict pixel = px;
        uint32_t* restrict col = &color[node - uf];
        assign_3_4(pixel, *col);
    }
 
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        CALL_RETRY(sem_wait(&cx->thread_args[i].semaphore_out));
    }
     
#else

    uint32_t* px = pixels;
    DsufNode* ufp = uf;
    for (; px < pixels+width*height; ++px, ++ufp) {
        DsufNode* node = ds_lazy_find(ufp); 
        uint32_t* restrict pixel = px;
        uint32_t* restrict col = &color[node - uf];
        assign_3_4(pixel, *col);
    }

#endif

    if (cx->level % 50 == 0) {
        printf("%f\n", lvl);
        fflush(stdout);
    }

    if (!pause)
        ++cx->level;
    return 1;
}
