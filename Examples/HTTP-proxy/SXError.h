
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

#ifndef SXError_h
#define SXError_h

#include <stdio.h>
#include <limits.h>

typedef enum SXError
{
    SX_SUCCESS                          = 0,
    SX_ERROR_INVALID_SOCKET             = -1,
    SX_ERROR_INVALID_SERVER             = -2,
    SX_ERROR_INVALID_CLIENT             = -3,
    SX_ERROR_INVALID_ARGUMENTS          = -4,
    SX_ERROR_INVALID_DOMAIN             = -5,
    SX_ERROR_MEM_ALLOC                  = -6,
    SX_ERROR_INVALID_IPADDR             = -7,
    SX_ERROR_SERVER_CANNOT_BE_DGRAM     = -8,
    SX_ERROR_SYS_SOCKET                 = -10,
    SX_ERROR_SYS_BIND                   = -11,
    SX_ERROR_SYS_LISTEN                 = -12,
    SX_ERROR_SYS_CONNECT                = -13,
    SX_ERROR_SYS_SEND                   = -14,
    SX_ERROR_SYS_RECV                   = -15,
    SX_ERROR_SYS_ACCEPT                 = -16,
    
    SX_ERROR_SYS_SETSOCKOPT             = -17,
    SX_ERROR_SYS_GETSYSOPT              = -18,
    SX_ERROR_SYS_SEND_MSG               = -19,
    SX_ERROR_SYS_RECV_MSG               = -20,
    SX_ERROR_SYS_FCNTL                  = -21,
    
    SX_ERROR_SYS_KQUEUE                 = -22,
    SX_ERROR_SYS_KEVENT                 = -23,
    SX_ERROR_SYS_GET_ADDR_INFO          = -24,
    
    SX_ERROR_CREATE_THREAD              = -40,
    SX_ERROR_CREATE_QUEUE               = -41
} SXError;

#define ERR_RET(ret) if (err_ret != NULL) *err_ret = ret
#define SX_RETURN_ERR(operation)\
SXError ________err_ = operation;\
if (________err_ != SX_SUCCESS) return ________err_;


#endif /* SXError_h */
