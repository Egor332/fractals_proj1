#ifndef UPDATE_CALLBACK_H
#define UPDATE_CALLBACK_H
#include <stddef.h>
#include <stdint.h>
#include "union_find_aos.h"
#include "config.h"

#ifdef PP_UNIX
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef PP_UNIX
typedef struct ThreadArgs_ {
    sem_t semaphore_in;
    sem_t semaphore_out;
    size_t first;
    size_t n;
    uint32_t* pixels;
    struct Context* cx;
    int finish;
    uint64_t padding[80];
} ThreadArgs;
#endif

struct Context {
#ifdef PP_UNIX
    pthread_t thread[THREAD_COUNT];
    ThreadArgs thread_args[THREAD_COUNT];
#endif
    float* horiz_connection_prob;
    float* vert_connection_prob;
    uint32_t* color;
    DsufNode* uf;
    size_t level;
};

int init_callback(uint32_t* pixels, struct Context* cx);
int update_callback(int pause, uint32_t* pixels, struct Context* cx);
int finish_callback(uint32_t* pixels, struct Context* cx);

#endif
