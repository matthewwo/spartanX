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

#define sx_runtime_obj_cast(v) (sx_runtime_object_t *)(v)
#define sx_runtime_obj_get_sock(v) (sx_runtime_obj_cast(v))->sock
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
    void * udata;
    void * udata1;
    sx_status_t status;
    sx_runtime_object_t * owner;
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
void * udata;\
void * udata1;\
sx_status_t status;\
sx_runtime_object_t * owner;\
sx_runtime_status_handler;\
sx_runtime_object_handler;\
sx_runtime_data_handler;
/* takes sx_runtime_object_t only */
SXError SXSuspendObject(void * sx_runtime_obj);
SXError SXResumeObject(void * sx_runtime_obj);

SXError SXRetain(void * sx_object);
SXError SXRelease(void * sx_object);
SXError SXFree(void * sx_object);

void sx_obj_init(SXObjectRef obj, sx_deallocator dealloc);
void sx_runtime_obj_init(sx_runtime_object_t * obj, sx_deallocator dealloc);

#endif /* sx_object_h */
