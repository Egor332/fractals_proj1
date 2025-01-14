#include "update_callback.h"

int init_callback(ivec4* data, size_t* new_n, PercContext* cx) {
    
    return 1;
}

int update_callback(ivec4* data, size_t* new_n, size_t* first_to_update, size_t* n_to_update, PercContext* cx) {
    *new_n = PERC_N;
    *first_to_update = 30;
    *n_to_update = 30;


    return 1;
}

int finish_callback(ivec4* data, size_t* new_n, PercContext* cx) {

    return 1;
}
