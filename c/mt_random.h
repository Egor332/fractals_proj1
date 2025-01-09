/* wiki 29.12.2024 */
#ifndef MT_RANDOM_H
#define MT_RANDOM_H
#include <stdint.h>
#define mt_random_n 624

typedef struct
{
    uint32_t state_array[mt_random_n];         // the array for the state vector 
    int state_index;                 // index into state vector array, 0 <= state_index <= n-1   always
} mt_state;

void mt_initialize_state(mt_state* state, uint32_t seed);
uint32_t mt_random_uint32(mt_state* state);

#endif