#include "update_callback.h"
#include "union_find_aos.h"
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct Parameters {
    struct Vertex* vertices;
    size_t width;
    size_t vsz;
};

void draw(size_t x, size_t y, float r, float g, float b, struct Parameters* p) {
    set_vertex_color(p->vertices + (x + y * p->width) * p->vsz, p->vsz, r, g, b);
}

#ifdef PP_UNIX

void* thread_func(void* void_args) {
    ThreadArgs* args = void_args;
    while (1) {
        if (sem_wait(&args->semaphore_in) == -1) {
            return NULL;
        }
        if (args->finish) {
            return NULL;
        }

        /* --- */

        size_t first = args->first;
        size_t last_exc = first + args->n;
        size_t width = args->width;
        float* color = args->cx->color;
        DsufNode* uf = args->cx->uf;
        struct Parameters p = { args->vertices, width, args->vsz };
        for (size_t i = first; i < last_exc; ++i) {
            size_t x = i % width;
            size_t y = i / width;
            DsufNode* node = ds_lazy_find(&uf[x+y*width]); 
            draw(x, y,
                 color[(node - uf)*3+0],
                 color[(node - uf)*3+1],
                 color[(node - uf)*3+2],
                 &p);
        }

        (void)sem_post(&args->semaphore_out);
    }
    return NULL;
}

#endif

int init_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx) {
    cx->hor_conn = malloc(sizeof(float)*width*height);
    cx->ver_conn = malloc(sizeof(float)*width*height);
    cx->color = malloc(width*height*3*sizeof(float));
    cx->uf = calloc(width*height, sizeof(DsufNode));
    cx->level = 0;

#ifdef PP_UNIX
    memset(cx->thread, 0, THREAD_COUNT * sizeof(pthread_t));
#endif

    float* hor_conn = cx->hor_conn;
    float* ver_conn = cx->ver_conn;
    float* color = cx->color;
    DsufNode* uf = cx->uf;

    if (!hor_conn || !ver_conn || !uf || !color) {
        goto FAILED;
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            hor_conn[x+y*width] = (float)rand()/RAND_MAX;
            ver_conn[x+y*width] = (float)rand()/RAND_MAX;
            uf[x+y*width].parent = &uf[x+y*width];
            color[(x+y*width)*3+0] = (float)rand()/RAND_MAX;
            color[(x+y*width)*3+1] = (float)rand()/RAND_MAX;
            color[(x+y*width)*3+2] = (float)rand()/RAND_MAX;
        }
    }

    size_t n = width*height;
    const size_t chunk_size = n / (THREAD_COUNT+1);

    ThreadArgs* thread_args = cx->thread_args;
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        thread_args[i].first = chunk_size * i;
        thread_args[i].n = chunk_size;
        thread_args[i].width = width;
        thread_args[i].vertices = vertices;
        thread_args[i].vsz = vsz;
        thread_args[i].cx = cx;

        thread_args[i].finish = 0;
        (void)sem_init(&thread_args[i].semaphore_in, 0, 0);
        (void)sem_init(&thread_args[i].semaphore_out, 0, 0);
        if (pthread_create(&cx->thread[i], NULL, thread_func, &thread_args[i])) {
            goto FAILED;
        }
    }

    return 1;
FAILED:
    finish_callback(vertices, width, height, vsz, cx);
    return 0;
}

int finish_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx) {

#ifdef PP_UNIX
    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        cx->thread_args[i].finish = 1;
        (void)sem_post(&cx->thread_args[i].semaphore_in);
        (void)pthread_join(cx->thread[i], NULL);
        (void)sem_destroy(&cx->thread_args[i].semaphore_in);
        (void)sem_destroy(&cx->thread_args[i].semaphore_out);
    }
#endif

    free(cx->hor_conn);
    free(cx->ver_conn);
    free(cx->uf);
    free(cx->color);
    return 1;
}


int update_callback(struct Vertex* vertices, size_t width, size_t height, size_t vsz, struct Context* cx) {
    struct Parameters p = { vertices, width, vsz };

    float* hor_conn = cx->hor_conn;
    float* ver_conn = cx->ver_conn;
    float* color = cx->color;
    DsufNode* uf = cx->uf;

    const size_t max_lvl = PERC_MAX_LVL;
    size_t n = width*height;

    for (size_t y = 0; y < height-1; ++y) {
        for (size_t x = 0; x < width; ++x) {
            if (ver_conn[x+y*width] < (float)cx->level/max_lvl) {
                ds_union(&uf[x+y*width], &uf[x+(y+1)*width]);
            }
        }
    }

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width-1; ++x) {
            if (hor_conn[x+y*width] < (float)cx->level/max_lvl) {
                ds_union(&uf[x+y*width], &uf[(x+1)+y*width]);
            }
        }
    }

#ifdef PP_UNIX

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        (void)sem_post(&cx->thread_args[i].semaphore_in);
    }

    const size_t chunk_size = n / (THREAD_COUNT+1);

    for (size_t i = chunk_size*THREAD_COUNT; i < n; ++i) {
        size_t x = i % width;
        size_t y = i / width;
        DsufNode* node = ds_lazy_find(&uf[x+y*width]); 
        draw(x, y,
                 color[(node - uf)*3+0],
                 color[(node - uf)*3+1],
                 color[(node - uf)*3+2],
                 &p);
    }

    for (size_t i = 0; i < THREAD_COUNT; ++i) {
        CALL_RETRY(sem_wait(&cx->thread_args[i].semaphore_out));
    }
     
#else

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            draw(x, y,
                 color[(ds_find(&uf[x+y*width]) - uf)*3+0],
                 color[(ds_find(&uf[x+y*width]) - uf)*3+1],
                 color[(ds_find(&uf[x+y*width]) - uf)*3+2],
                 &p);
        }
    }

#endif
 
    ++cx->level;
    return 1;
}



void set_vertex_color(struct Vertex* first, size_t n, float r, float g, float b) {
    for (size_t i = 0; i < n; ++i) {
        first[i].r = r;
        first[i].g = g;
        first[i].b = b;
    }
}
