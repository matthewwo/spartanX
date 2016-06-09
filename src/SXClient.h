//
////  Copyright (c) 2016, Yuji
////  All rights reserved.
////
////  Redistribution and use in source and binary forms, with or without
////  modification, are permitted provided that the following conditions are met:
////
////  1. Redistributions of source code must retain the above copyright notice, this
////  list of conditions and the following disclaimer.
////  2. Redistributions in binary form must reproduce the above copyright notice,
////  this list of conditions and the following disclaimer in the documentation
////  and/or other materials provided with the distribution.
////
////  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
////  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
////  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
////  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
////  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
////  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
////  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
////  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
////  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
////  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////
////  The views and conclusions contained in the software and documentation are those
////  of the authors and should not be interpreted as representing official policies,
////  either expressed or implied, of the FreeBSD Project.
////
////  Created by yuuji on 5/9/16.
////  Copyright © 2016 yuuji. All rights reserved.
//
//#ifndef SXClient_h
//#define SXClient_h
//
//#include "sx_global.h"
//#include "SXSocket.h"
//#include "SXError.h"
//
//#define FRTR_N(name) fptr_##name
//
//#define CAT_I(a,b) a##b
//#define CAT(a,b) CAT_I(a, b)
//
//#ifdef __cplusplus
//
//#define FPTR(ret, name, args) typedef std::function<ret args> fptr_##name;
//#else
//
//#define FPTR(ret, name, args) typedef ret (fptr_##name args);
//#endif
//
//#ifdef __BLOCKS__
//#define BLOCK_N(name) block_##name
//#define BLOCK(ret, name, args) typedef ret(^block_##name) args;
//
//#ifdef __Swift
//#define SWIFT_N(name) typedef BLOCK_N(name) ##name;
//#else
//#define SWIFT_N(name)
//#endif
//#else
//#define BLOCK_N(name)
//#define BLOCK(ret, name, args)
//#endif
//
//#define hDeclare(ret, name, args)\
//BLOCK(ret, name, args);\
//FPTR(ret, name, args);\
//SWIFT_N(name);
//
//typedef struct sx_client * SXClientRef;
//
//hDeclare(size_t,
//         SXClientDidReceive,
//         (SXClientRef client, void * data, size_t length));
//
//hDeclare(bool,
//         SXClientDidReceiveError,
//         (SXClientRef client, SXError error));
//
//hDeclare(void,
//         SXClientDidDisconnect,
//         (SXClientRef client))
//
//#undef FRTR_N
//#undef FPTR
//#undef BLOCK_N
//#undef BLOCK
//
//enum sx_client_type {
//    sx_client_use_hostname,
//    sx_client_use_ipaddr,
//};
//
//typedef struct sx_client {
//    SXSocketRef socket;
//    char target[255];
//    char service[55];
//    int preferred_af_inet;
//    int sock_type;
//    int protocol;
//    int buf_size;
//    unsigned short port;
//    enum sx_client_type type;
//    
//    void * udata;
//    
//    dispatch_queue_priority_t priority;
//    
//#ifdef __BLOCKS__
//    __unsafe_unretained block_SXClientDidReceive dataHandler_block;
//    __unsafe_unretained block_SXClientDidReceiveError errHandler_block;
//    __unsafe_unretained block_SXClientDidDisconnect disconnect_block;
//#endif
//    
//#ifdef __cplusplus
//    fptr_SXClientDidReceive     dataHandler_fptr;
//    fptr_SXClientDidReceiveError errHandler_fptr;
//    fptr_SXClientDidDisconnect disconnect_fptr;
//#else
//    fptr_SXClientDidReceive*    dataHandler_fptr;
//    fptr_SXClientDidReceiveError* errHandler_fptr;
//    fptr_SXClientDidDisconnect* disconnect_fptr;
//#endif
//    
//    int ref_count;
//    
//} sx_client_t ;
//
//SXClientRef SXCreateClientWithHostname(const char * hostname,
//                                const char * service,
//                                int sock_type,
//                                int preferred_af_inet,
//                                int buf_size,
//                                dispatch_queue_priority_t priority,
//                                SXError * err_ret);
//
//SXClientRef SXCreateClientWithIp(const char * ip,
//                                 unsigned short port,
//                                 int sock_type,
//                                 int sock_protocol,
//                                 int sock_domain,
//                                 int buf_size,
//                                 dispatch_queue_priority_t priority,
//                                 SXError * err_ret);
//
//void SXStartClient(SXClientRef client, bool use_block_handler);
//
//SXError SXRetainClient(SXClientRef);
//SXError SXReleaseClient(SXClientRef);
//SXError SXFreeClient(SXClientRef);
//
//#endif /* SXClient_h */
