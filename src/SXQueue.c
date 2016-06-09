
//  Copyright (c) 2016, Yuji
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  The views and conclusions contained in the software and documentation are those
//  of the authors and should not be interpreted as representing official policies,
//  either expressed or implied, of the FreeBSD Project.
//
//  Created by yuuji on 5/9/16.
//  Copyright © 2016 yuuji. All rights reserved.

#include "SXQueue.h"

#define SXCHECK_COMPATIBLES(arr, c) SX_RETURN_ERR(SXCheckCompatibleStatus(queue->status,arr, c))
#define SXCHECK_INCOMPATIBLES(arr, c) SX_RETURN_ERR(SXCheckIncompatibleStatus(queue->status,arr, c))

SXQueueRef SXCreateQueue(SXSocketRef socket, dispatch_queue_t queue, SXError * err_ret)
{
    SXQueueRef _queue = (SXQueueRef)sx_calloc(1, sizeof(sx_queue_t));
    sx_obj_init(&socket->obj, &SXFreeQueue);
    _queue->gcd_queue = queue;
    
    if (socket == NULL) {
        *err_ret = SX_ERROR_INVALID_SOCKET;
        SXRelease(_queue);
        return NULL;
    }
        
    _queue->sock = socket;
    
    return _queue;
}

SXError SXSuspendQueue(SXQueueRef queue)
{
    if (queue== NULL)
        return SX_ERROR_INVALID_SERVER;
    
    sx_status_t a[2] = {sx_status_idle, sx_status_running};
    SXCHECK_COMPATIBLES(a, 2);
    
    queue->status = sx_status_suspend;
    return SX_SUCCESS;
}

SXError SXResumeQueue(SXQueueRef queue)
{
    if (queue == NULL)
        return SX_ERROR_INVALID_SERVER;
    
    SX_RETURN_ERR(SXCheckStatus(queue->status, sx_status_suspend));
    
    queue->status = sx_status_resuming;
    return SX_SUCCESS;
}

SXError SXFreeQueue(SXQueueRef queue)
{
    SX_RETURN_ERR(SXRelease(queue->sock));
    memset(queue, 0, sizeof(queue->sock));
    sx_free(queue);
    queue=NULL;
    return SX_SUCCESS;
}

SXError SXCheckStatus(sx_status_t current, sx_status_t required)
{
    if (current == required)
        return  SX_SUCCESS;
    return current * -1;
}


SXError SXCheckCompatibleStatus(sx_status_t current, sx_status_t * array, size_t count)
{
    for (int i = 0; i < count ; ++i)
        if (current == array[i])
            return SX_SUCCESS;
    return -1 * current;
}

SXError SXCheckIncompatibleStatus(sx_status_t current, sx_status_t * array, size_t count)
{
    for (int i = 0; i < count; ++i)
        if (current == array[i])
            return -1 * current;
    return SX_SUCCESS;
}

#undef SXCHECK_COMPATIBLES
#undef SXCHECK_INCOMPATIBLES