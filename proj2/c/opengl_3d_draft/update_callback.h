#ifndef PERC_UPDATE_CALLBACK
#define PERC_UPDATE_CALLBACK

#include <cglm/cglm.h>
#include <stddef.h>
#define PERC_N 100

typedef struct PercContext_ {
    int tmp;
} PercContext;

int init_callback(ivec4* data, size_t* new_n, PercContext* cx); 

int update_callback(ivec4* data, size_t* new_n, size_t* first_to_update, size_t* n_to_update, PercContext* cx); 

int finish_callback(ivec4* data, size_t* new_n, PercContext* cx);

#endif
