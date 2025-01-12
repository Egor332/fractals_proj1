#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "config.h"
#include <stddef.h>

typedef struct DsufNode_ {
    size_t rank_size;
    struct DsufNode_* parent;
} DsufNode;

DsufNode* ds_find(DsufNode* p);
DsufNode* ds_lazy_find(DsufNode* p);
void ds_union(DsufNode* restrict a_in, DsufNode* restrict b_in);

#endif
