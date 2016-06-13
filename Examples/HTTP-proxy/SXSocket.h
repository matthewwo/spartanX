
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

#ifndef SXSocket_h
#define SXSocket_h

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <errno.h>

#include "sx_global.h"
#include "SXError.h"
#include "sx_object.h"

#define sockaddr_in(addr) (*((struct sockaddr_in*)&addr))
#define sockaddr_in6(addr) (*((struct sockaddr_in6*)&addr))

typedef struct sx_socket {
    sx_object_t obj; /* `super class` */
    
    int sockfd;
    int protocol;
    int domain;
    int type;
    
    int port;
    struct sockaddr_storage addr;
    
    
} sx_socket_t;

typedef sx_socket_t * SXSocketRef;

SXSocketRef SXCreateServerSocket(unsigned short port,
                                 int domain,
                                 int type,
                                 int protocol,
                                 SXError * err_ret);

#define SXCreateServerSocketTCPIPv4(port, err_ret) SXCreateServerSocket(port, AF_INET, SOCK_STREAM, 0, &err_ret)
#define SXCreateServerSocketTCPIPv6(port, err_ret) SXCreateServerSocket(port, AF_INET6, SOCK_STREAM, 0, &err_ret)

SXSocketRef SXCreateClientSocketByHostname(const char * hostname,
                                           const char * service,
                                           struct addrinfo * hint,
                                           SXError * err_ret);

SXSocketRef SXCreateClientSocket(const char * ipaddr,
                                 unsigned short port,
                                 int domain,
                                 int type,
                                 int protocol,
                                 SXError * err_ret);

#define SXCreateClientSocketTCPIPv4(ipaddr, err_ret) SXCreateClientSocket(#ipaddr, 0, AF_INET, SOCK_STREAM, 0, &err_ret)
#define SXCreateClientSocketTCPIPv6(ipaddr, err_ret) SXCreateClientSocket(#ipaddr, 0, AF_INET6, SOCK_STREAM, 0 &err_ret)

#define SXCreateSocketUDPIPv4(ipaddr, err_ret) SXCreateClientSocket(#ipaddr, 0, AF_INET, SOCK_DGRAM, 0, &err_ret)
#define SXCreateSocketUDPIPv6(ipaddr, err_ret) SXCreateClientSocket(#ipaddr, 0, AF_INET6, SOCK_DGARM, 0 &err_ret)

SXError SXSocketSetBlock(SXSocketRef sock, bool block);
SXError SXSocketSend    (SXSocketRef target, const void * payload, size_t length);

SXError SXFreeSocket    (SXSocketRef socket);
SXError SXSocketConnect (SXSocketRef socket);
SXError SXSocketListen  (SXSocketRef socket);
#endif /* SXSocket_h */
