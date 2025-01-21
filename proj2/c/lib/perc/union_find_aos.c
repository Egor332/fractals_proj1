#include "union_find_aos.h"

DsufNode* ds_find(DsufNode* p) {
#ifdef DSUF_HALVE
    while (p != p->parent) {
        p->parent = p->parent->parent;
        p = p->parent;
    }
#else
    while (p != p->parent) {
        void* tmp = p->parent;
        p->parent = p->parent->parent;
        p = tmp;
    }
#endif
    return p;
}

DsufNode* ds_lazy_find(DsufNode* p) {
    while (p != p->parent) {
        p = p->parent;
    }
    return p;
}

DsufNode* ds_union(DsufNode* restrict a_in, DsufNode* restrict b_in) {
    DsufNode* a_tmp = ds_find(a_in);
    DsufNode* b_tmp = ds_find(b_in);

    if (a_tmp == b_tmp)
        return a_tmp;

    if (a_tmp->rank_size < b_tmp->rank_size) {
        DsufNode* tmp = a_tmp;
        a_tmp = b_tmp;
        b_tmp = tmp;
    }

    {
    
    DsufNode* restrict a = a_tmp;
    DsufNode* restrict b = b_tmp;
    b->parent = a;

#ifdef DSUF_RANK
    if (a->rank_size == b->rank_size) {
        a->rank_size += 1;
    }
#else
    a->rank_size += b->rank_size;
#endif
    
    return a;

    }
}
