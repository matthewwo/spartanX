//
//  SXVector.h
//  cspartanX
//
//  Created by yuuji on 5/31/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#ifndef SXVector_h
#define SXVector_h

#include "sx_global.h"
#include "SXError.h"

typedef struct sx_vector * SXVectorRef;

typedef struct sx_vector {
    size_t count;
    size_t osize;
    void * mem_loca;
    int ref_count;
    void (*dealloc)(void *);
} sx_vector_t;

SXVectorRef SXCreateVector(size_t obj_size, size_t reserve_capacity, void (*dealloc)(void *));
SXVectorRef SXCreateVectorWithArray(const void * array, size_t obj_size, size_t n_items, void (*dealloc)(void *));

void SXVectorResize(SXVectorRef vector, size_t count);
void SXVectorPopback(SXVectorRef vector);
void *SXVectorObjectPtrAtIndex(SXVectorRef vector, size_t index);

SXError SXRetainVector(SXVectorRef);
SXError SXReleaseVector(SXVectorRef);
SXError SXFreeVector(SXVectorRef);

#endif /* SXVector_h */
