
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
#include "sx_global.h"
#include "sx_object.h"
#include "SXQueue.h"
#include "SXError.h"
#include "SXSocket.h"
#ifdef __cplusplus
}
#endif

#if defined(__FreeBSD__) || defined(__BSD__) || defined(__APPLE__)
    #include <sys/event.h>
#endif

#ifdef 	__gnu_linux__
    #include <sys/epoll.h>
#endif

#include <pthread.h>
#include "sx_event.h"

typedef struct sx_server * SXServerRef;

typedef struct sx_server
{
    sx_runtime_items;
    SXServerStatusHandlers;
    
//    SXSocketRef socket;

//    __unsafe_unretained block_SXServerDidStart      didStart_block;
//    __unsafe_unretained block_SXServerShouldConnect shouldConnect_block;
//    SXServerHandlers_block;
//    __unsafe_unretained block_SXServerDidKill       didKill_block;

    
    size_t backlog;
    size_t max_guest;
    bool failable;
    
    void * udata, * udata1;
    SXQueueRef * queues;

    unsigned int ref_count;
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

SXServerRef SXCreateServer          (sx_server_setup setup, SXError * err_ret, didReceive message_handler);
//
//void SXServerSetBlockDidStart       (SXServerRef server, block_SXServerDidStart         );
//void SXServerSetBlockShouldConnect  (SXServerRef server, block_SXServerShouldConnect    );
//void SXServerSetBlockDidConnect     (SXServerRef server, block_SXServerDidConnect       );
//void SXServerSetBlockDidReceive     (SXServerRef server, block_SXServerDidReceive       );
//void SXServerSetBlockDidDisconnect  (SXServerRef server, block_SXServerDidDisconnect    );
//void SXServerSetBlockWillSuspend    (SXServerRef server, block_SXServerWillSuspend      );
//void SXServerSetBlockDidResume      (SXServerRef server, block_SXServerDidResume        );
//void SXServerSetBlockWillKill       (SXServerRef server, block_SXServerWillKill         );
//void SXServerSetBlockDidKill        (SXServerRef server, block_SXServerDidKill          );

#ifdef __DISPATCH_PUBLIC__

SXError SXServerStart(SXServerRef server,
                      block_queue_generate_policy listen_queue_generate_policy,
                      block_queue_generate_policy operate_queue_generate_policy);

SXError SXServerStart_kqueue(SXServerRef server, dispatch_queue_t gcd_queue);

SXError SXServerStart1(SXServerRef server,
                       dispatch_queue_priority_t listen_queue_priority,
                       dispatch_queue_priority_t operate_queue_priority);

SXError SXServerStart2(SXServerRef server,
                       dispatch_queue_priority_t priority);
#endif

SXError SXFreeServer    (SXServerRef);
SXError SXSuspendServer (SXServerRef);
SXError SXResumeServer  (SXServerRef);
SXError SXKillServer    (SXServerRef);

#endif /* SXServer_h */
