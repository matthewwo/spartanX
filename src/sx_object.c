//
//  sx_object.c
//  cspartanX
//
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#include "sx_object.h"

void sx_obj_init(SXObjectRef obj, sx_deallocator dealloc) {
    obj->ref_count = 1;
    obj->dealloc = dealloc;
}

SXError SXRetain(SXObjectRef obj) {
    if (obj->ref_count == sx_weak_object) {
        return SX_SUCCESS;
    }
    ++obj->ref_count;
    return SX_SUCCESS;
}

SXError SXRelease(SXObjectRef obj) {
    if (obj->ref_count == sx_weak_object)
        return SX_SUCCESS;
    --obj->ref_count;
    if (obj->ref_count == 0)
        return SXFree(obj);
    return SX_SUCCESS;
}

SXError SXFree(SXObjectRef obj) {
    return obj->dealloc(obj);
}


void SXRuntimeObjectSetBlockDidConnect     (sx_runtime_object_t * object, didConnect h) {
    object->didConnect_block = h;
}

void SXRuntimeObjectSetBlockDidDisconnect  (sx_runtime_object_t * object, didDisconnect h) {
    object->didDisconnect_block = h;
}

void SXRuntimeObjectSetBlockWillKill       (sx_runtime_object_t * object, willKill h) {
    object->willKill_block = h;
}

void SXRuntimeObjectSetBlockDidReceive     (sx_runtime_object_t * object, didReceive h) {
    object->dataHandler_block = h;
}

void SXRuntimeObjectSetBlockDidRecvError   (sx_runtime_object_t * object, didRecvError h) {
    object->errHandler_block = h;
}

void SXRuntimeObjectSetBlockWillSuspend    (sx_runtime_object_t * object, willSuspend h) {
    object->willSuspend_block = h;
}

void SXRuntimeObjectSetBlockDidResume      (sx_runtime_object_t * object, didResume h) {
    object->didResume_block = h;
}
