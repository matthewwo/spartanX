//
//  SXVector.c
//  cspartanX
//
//  Created by yuuji on 5/31/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#include "SXVector.h"

SXVectorRef SXCreateVector(size_t obj_size, size_t reserve_capacity, void (*dealloc)(void *)) {
    SXVectorRef vector = (SXVectorRef)sx_calloc(1, sizeof(sx_vector_t));
    vector->ref_count = 1;
    vector->osize = obj_size;
    vector->mem_loca = sx_malloc(reserve_capacity * obj_size);
    vector->dealloc = dealloc;
    return vector;
}

size_t SXVectorPushBack(SXVectorRef vector, void * item)
{
    sx_realloc(vector->mem_loca, (vector->count + 1) * vector->osize);
    memcpy((char *)vector->mem_loca + vector->count * vector->osize, item, vector->osize);
    ++vector->count;
    return vector->count;
}

SXVectorRef SXCreateVectorWithArray(const void * array, size_t obj_size, size_t n_items, void (*dealloc)(void *))
{
    SXVectorRef vector = (SXVectorRef)sx_calloc(1, sizeof(sx_vector_t));
    vector->ref_count = 1;
    vector->osize = obj_size;
    vector->count = n_items;
    vector->mem_loca = sx_malloc(obj_size * n_items);
    
    vector->dealloc = dealloc;
    for (int i = 0; i < n_items; ++i)
        memcpy((char *)vector->mem_loca + obj_size * i, (char *)array + obj_size * i, obj_size);
    return vector;
}

void * SXVectorObjectPtrAtIndex(SXVectorRef vector, size_t index) {
    return (char *)vector->mem_loca + index * vector->osize;
}

void SXVectorResize(SXVectorRef vector, size_t count) {
    
    if (count == vector->count) return;
    
    if (count < vector->count) {
        for (size_t i = vector->count; i < count; ++i)
            if (vector->dealloc)
                vector->dealloc(SXVectorObjectPtrAtIndex(vector, i));
    }
    
    vector->mem_loca = sx_realloc(vector->mem_loca, count * vector->osize);
}

void SXVectorPopback(SXVectorRef vector) {
    if (vector->count <= 0) return;
    
    if (vector->dealloc)
        vector->dealloc(SXVectorObjectPtrAtIndex(vector, vector->count - 1));
    vector->count--;
}

SXError SXRetainVector(SXVectorRef vector) {
    if (vector->ref_count == sx_weak_object)
        return SX_SUCCESS;
    ++vector->ref_count;
    return SX_SUCCESS;
}
SXError SXReleaseVector(SXVectorRef vector) {
    if (vector->ref_count == sx_weak_object)
        return SX_SUCCESS;
    --vector->ref_count;
    if (vector->ref_count == 0)
        return SXFreeVector(vector);
    return SX_SUCCESS;
}

SXError SXFreeVector(SXVectorRef vector) {
    if (vector->dealloc != NULL)
        for (int i = 0; i < vector->count; ++i)
            vector->dealloc(vector->mem_loca + i * vector->osize);
    sx_free(vector->mem_loca);
    sx_free(vector);
    return SX_SUCCESS;
}
