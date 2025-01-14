#ifndef UPDATE_CALLBACK_H
#define UPDATE_CALLBACK_H
#include <stddef.h>
#include "union_find_aos.h"
#include "local_config.h"

#ifdef PP_UNIX
#include <pthread.h>
#include <semaphore.h>
#endif

struct Vertex {
    float x,y,r,g,b;
};

#ifdef PP_UNIX
typedef struct ThreadArgs_ {
    size_t first;
    size_t n;
    size_t width;
    struct Vertex* vertices;
    size_t vsz;
    struct Context* cx;
    
    int finish;
    sem_t semaphore_in;
    sem_t semaphore_out;
} ThreadArgs;
#endif

struct Context {
    float* hor_conn;
    float* ver_conn;
    float* color;
    DsufNode* uf;
    size_t level;

#ifdef PP_UNIX
    pthread_t thread[THREAD_COUNT];
    ThreadArgs thread_args[THREAD_COUNT];
#endif

};

void set_vertex_color(struct Vertex* first, size_t n, float r, float g, float b);
int init_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx);
int update_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx);
int finish_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx);

#endif
