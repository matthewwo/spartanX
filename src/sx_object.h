//
//  sx_object.h
//  cspartanX
//
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#ifndef sx_object_h
#define sx_object_h

#include "sx_event.h"
#include "sx_global.h"
#include "SXError.h"

typedef struct sx_socket * SXSocketRef;
typedef struct sx_queue * SXQueueRef;
typedef struct sx_object * SXObjectRef;
typedef SXError (*sx_deallocator)(SXObjectRef obj);

typedef struct sx_object {
    sx_int64 ref_count;
    sx_deallocator dealloc;
} sx_object_t;

typedef struct sx_runtime_object {
    sx_object_t obj;
    SXSocketRef sock;
    size_t dataSize;
    int recvFlags;
    int sendFlags;
    sx_status_t status;
    sx_runtime_status_handler;
    sx_runtime_object_handler;
    sx_runtime_data_handler;
} sx_runtime_object_t;

#define sx_runtime_items \
sx_object_t obj;\
SXSocketRef sock;\
size_t dataSize;\
int recvFlags;\
int sendFlags;\
sx_status_t status;\
sx_runtime_status_handler;\
sx_runtime_object_handler;\
sx_runtime_data_handler;

SXError SXRetain(SXObjectRef);
SXError SXRelease(SXObjectRef);
SXError SXFree(SXObjectRef);

void sx_obj_init(SXObjectRef obj, sx_deallocator dealloc);

#endif /* sx_object_h */
