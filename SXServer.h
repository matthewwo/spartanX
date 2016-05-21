
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

#ifndef SXServer_h
#define SXServer_h


#ifdef __cplusplus
#include <functional>
extern "C" {
#endif
#include <unistd.h>
#include "SXQueue.h"
#include "SXError.h"
#include "SXSocket.h"
#ifdef __cplusplus
}
#endif

#if defined(__APPLE__) || defined(__SX___)
    #include <dispatch/dispatch.h>
#endif

#if defined(__FreeBSD__) || defined(__BSD__) || defined(__APPLE__)
    #include <sys/event.h>
#endif

#ifdef 	__gnu_linux__
    #include <sys/epoll.h>
#endif

#include <pthread.h>

typedef struct sx_server * SXServerRef;

#ifndef __OBJC__
#define __unsafe_unretained
#define __bridge
#endif

#define FRTR_N(name) fptr_##name

#define CAT_I(a,b) a##b
#define CAT(a,b) CAT_I(a, b)

#ifdef __cplusplus

#define FPTR(ret, name, args) typedef std::function<ret args> fptr_##name;
#else

#define FPTR(ret, name, args) typedef ret (fptr_##name args);
#endif

#ifdef __BLOCKS__
    #define BLOCK_N(name) block_##name
    #define BLOCK(ret, name, args) typedef ret(^block_##name) args;

    #ifdef __Swift
        #define SWIFT_N(name) typedef BLOCK_N(name) ##name;
    #else
        #define SWIFT_N(name)
    #endif
#else
    #define BLOCK_N(name)
    #define BLOCK(ret, name, args)
#endif

#define hDeclare(ret, name, args)\
BLOCK(ret, name, args);\
FPTR(ret, name, args);\
SWIFT_N(name);

#define hdls(name)\
    BLOCK_N(name) block;\
    FRTR_N(name) * fn;

hDeclare(size_t,
         SXServerDidReceive,
         (SXServerRef server, SXQueueRef queue, void * data, size_t length));

hDeclare(void,
         SXServerDidStart,
         (SXServerRef server));

hDeclare(bool,
         SXServerShouldConnect,
         (SXServerRef server, SXSocketRef socket));

hDeclare(void,
         SXServerDidConnect,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerDidDisconnect,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerWillKill,
         (SXServerRef server,  SXQueueRef const queue));

hDeclare(void,
         SXServerDidKill, (SXServerRef server));

hDeclare(void,
         SXServerDidResume,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerWillSuspend,
         (SXServerRef server, SXQueueRef queue));

hDeclare(dispatch_queue_t,
         queue_generate_policy,
         ());

#undef FRTR_N
#undef FPTR
#undef BLOCK_N
#undef BLOCK

typedef struct sx_server
{
    SXSocketRef socket;

#ifdef __BLOCKS__

    __unsafe_unretained block_SXServerDidStart      didStart_block;
    __unsafe_unretained block_SXServerShouldConnect shouldConnect_block;
    __unsafe_unretained block_SXServerDidConnect    didConnect_block;
    __unsafe_unretained block_SXServerDidReceive    dataHandler_block;
    __unsafe_unretained block_SXServerDidDisconnect didDisconnect_block;
    __unsafe_unretained block_SXServerWillSuspend   willSuspend_block;
    __unsafe_unretained block_SXServerDidResume     didResume_block;
    __unsafe_unretained block_SXServerWillKill      willKill_block;
    __unsafe_unretained block_SXServerDidKill       didKill_block;
    
#endif

#ifdef __cplusplus
    fptr_SXServerDidStart       didStart_fptr;
    fptr_SXServerShouldConnect  shouldConnect_fptr;
    fptr_SXServerDidConnect     didConnect_fptr;
    fptr_SXServerDidReceive     dataHandler_fptr;
    fptr_SXServerDidDisconnect  didDisconnect_fptr;
    fptr_SXServerWillSuspend    willSuspend_fptr;
    fptr_SXServerDidResume      didResume_fptr;
    fptr_SXServerWillKill       willKill_fptr;
    fptr_SXServerDidKill        didKill_fptr;
#else
    fptr_SXServerDidStart*      didStart_fptr;
    fptr_SXServerShouldConnect* shouldConnect_fptr;
    fptr_SXServerDidConnect*    didConnect_fptr;
    fptr_SXServerDidReceive*    dataHandler_fptr;
    fptr_SXServerDidDisconnect* didDisconnect_fptr;
    fptr_SXServerWillSuspend*   willSuspend_fptr;
    fptr_SXServerDidResume*     didResume_fptr;
    fptr_SXServerWillKill*      willKill_fptr;
    fptr_SXServerDidKill*       didKill_fptr;
#endif
    
    size_t dataSize;
    size_t backlog;
    size_t max_guest;
    bool failable;
    
    SXQueueRef * queues;
    
    uint8_t status;
    unsigned int referenceCount;
} sx_server_t ;


#define SX_SERVER_DOMAIN_USE_IPV4   0x00000001
#define SX_SERVER_DOMAIN_USE_IPV6   0x00000010

#define SX_SERVER_PORT_ANY          0x00000000

#define SX_SERVER_DEFAULT_BACKLOG_SIZE 5
#define SX_SERVER_DEFAULT_DATA_SIZE 1024
#define SX_SERVER_DEFAULT_MAX_GUEST 500

typedef struct sx_server_setup {
    unsigned domain;
    unsigned port;
    size_t   backlog;
    size_t   dataSize;
    size_t   max_guest;
    bool     failable; /* reserved, not yet implemented */
} sx_server_setup;

SXServerRef SXCreateServer          (sx_server_setup setup, SXError * err_ret, block_SXServerDidReceive message_handler);
SXServerRef SXCreateServer_f        (sx_server_setup setup, SXError * err_ret, fptr_SXServerDidReceive  message_handler);


void SXServerSetBlockDidStart       (SXServerRef server, block_SXServerDidStart         );
void SXServerSetBlockShouldConnect  (SXServerRef server, block_SXServerShouldConnect    );
void SXServerSetBlockDidConnect     (SXServerRef server, block_SXServerDidConnect       );
void SXServerSetBlockDidReceive     (SXServerRef server, block_SXServerDidReceive       );
void SXServerSetBlockDidDisconnect  (SXServerRef server, block_SXServerDidDisconnect    );
void SXServerSetBlockWillSuspend    (SXServerRef server, block_SXServerWillSuspend      );
void SXServerSetBlockDidResume      (SXServerRef server, block_SXServerDidResume        );
void SXServerSetBlockWillKill       (SXServerRef server, block_SXServerWillKill         );
void SXServerSetBlockDidKill        (SXServerRef server, block_SXServerDidKill          );

void SXServerSetFnPtrDidStart       (SXServerRef server, fptr_SXServerDidStart          );
void SXServerSetFnPtrShouldConnect  (SXServerRef server, fptr_SXServerShouldConnect     );
void SXServerSetFnPtrDidConnect     (SXServerRef server, fptr_SXServerDidConnect        );
void SXServerSetFnPtrDidReceive     (SXServerRef server, fptr_SXServerDidReceive        );
void SXServerSetFnPtrDidDisconnect  (SXServerRef server, fptr_SXServerDidDisconnect     );
void SXServerSetFnPtrWillSuspend    (SXServerRef server, fptr_SXServerWillSuspend       );
void SXServerSetFnPtrDidResume      (SXServerRef server, fptr_SXServerDidResume         );
void SXServerSetFnPtrWillKill       (SXServerRef server, fptr_SXServerWillKill          );
void SXServerSetFnPtrDidKill        (SXServerRef server, fptr_SXServerDidKill           );

#ifdef __DISPATCH_PUBLIC__

SXError SXServerStart(SXServerRef server,
                      block_queue_generate_policy listen_queue_generate_policy,
                      block_queue_generate_policy operate_queue_generate_policy,
                      bool use_blocks_handler);


SXError SXServerStart_f(SXServerRef server,
                        fptr_queue_generate_policy listen_queue_generate_policy,
                        fptr_queue_generate_policy operate_queue_generate_policy,
                        bool use_blocks_handler);

SXError SXServerStart_kqueue(SXServerRef server, dispatch_queue_t gcd_queue, bool use_block_handler);

SXError SXServerStart1(SXServerRef server,
                       dispatch_queue_priority_t listen_queue_priority,
                       dispatch_queue_priority_t operate_queue_priority,
                       bool use_blocks_handler);

SXError SXServerStart2(SXServerRef server,
                       dispatch_queue_priority_t priority,
                       bool use_blocks_handler);
#endif

SXError SXRetainServer  (SXServerRef);
SXError SXReleaseServer (SXServerRef);
SXError SXFreeServer    (SXServerRef);

SXError SXSuspendServer (SXServerRef);
SXError SXResumeServer  (SXServerRef);
SXError SXKillServer    (SXServerRef);


#ifdef __cplusplus

#endif
#endif /* SXServer_h */
