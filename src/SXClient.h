
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

#ifndef SXClient_h
#define SXClient_h

#include "sx_global.h"
#include "SXSocket.h"
#include "SXError.h"

#define sx_client_ref(v) ((SXClientRef)(v))

typedef struct sx_client * SXClientRef;

enum sx_client_type {
    sx_client_use_hostname,
    sx_client_use_ipaddr,
};

typedef struct sx_client {
    sx_runtime_items;
    
//    sx_runtime_object_t * owner;
    enum sx_client_type type;
    dispatch_queue_priority_t priority;
    sx_byte * buf;
} sx_client_t ;

SXClientRef SXCreateClientWithHostname(const char * hostname,
                                       const char * service,
                                       struct addrinfo * hint,
                                       int buf_size,
                                       dispatch_queue_priority_t priority,
                                       SXError * err_ret);

SXClientRef SXCreateClientWithIp(const char * ip,
                                 unsigned short port,
                                 int sock_domain,
                                 int sock_type,
                                 int sock_protocol,
                                 int buf_size,
                                 dispatch_queue_priority_t priority,
                                 SXError * err_ret);

#define SXClientTCP_IPv4(ip, port, buf_size) SXCreateClientWithIp(ip, port, AF_INET, SOCK_STREAM, 0, buf_size, GCD_DEFAULT, NULL);

#define SXClientTCP_IPv6(ip, port, buf_size) SXCreateClientWithIp(ip, port, AF_INET6, SOCK_STREAM, 0, buf_size, GCD_DEFAULT, NULL);

SXError SXStartClient(SXClientRef client, void * initialPayload, size_t len);

SXError SXFreeClient(SXClientRef);

#endif /* SXClient_h */
