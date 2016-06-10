//
//  sx_object.c
//  cspartanX
//
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#include "sx_object.h"

#define SXCHECK_COMPATIBLES(arr, c) SX_RETURN_ERR(SXCheckCompatibleStatus(obj->status,arr, c))
#define SXCHECK_INCOMPATIBLES(arr, c) SX_RETURN_ERR(SXCheckIncompatibleStatus(obj->status,arr, c))

void sx_obj_init(SXObjectRef obj, sx_deallocator dealloc) {
    obj->ref_count = 1;
    obj->dealloc = dealloc;
}

void sx_runtime_obj_init(sx_runtime_object_t * obj, sx_deallocator dealloc) {
    sx_obj_init(obj, dealloc);
    obj->sendFlags = 0;
    obj->recvFlags = 0;
    obj->status = sx_status_idle;
}

SXError SXSuspendObject(sx_runtime_object_t * obj)
{
    if (obj== NULL)
        return SX_ERROR_INVALID_SERVER;
    
    sx_status_t a[2] = {sx_status_idle, sx_status_running};
    SXCHECK_COMPATIBLES(a, 2);
    
    obj->status = sx_status_suspend;
    return SX_SUCCESS;
}

SXError SXResumeObject(sx_runtime_object_t * obj)
{
    if (obj == NULL)
        return SX_ERROR_INVALID_SERVER;
    
    SX_RETURN_ERR(SXCheckStatus(obj->status, sx_status_suspend));
    
    obj->status = sx_status_resuming;
    return SX_SUCCESS;
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
