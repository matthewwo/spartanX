
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

#include "SXSocket.h"


// From Stackoverflow
unsigned getScopeForIp(const char *ip)
{
    struct ifaddrs *addrs;
    char ipAddress[NI_MAXHOST];
    unsigned scope=0;
    // walk over the list of all interface addresses
    getifaddrs(&addrs);
    
    for (struct ifaddrs *addr = addrs ; addr; addr = addr->ifa_next)
    {
        if (addr->ifa_addr && addr->ifa_addr->sa_family==AF_INET6){ // only interested in ipv6 ones
            
            getnameinfo(addr->ifa_addr,sizeof(struct sockaddr_in6),ipAddress,sizeof(ipAddress),NULL,0,NI_NUMERICHOST);
            // result actually contains the interface name, so strip it
            for(int i=0;ipAddress[i];i++){
                if(ipAddress[i]=='%'){
                    ipAddress[i]='\0';
                    break;
                }
            }
            // if the ip matches, convert the interface name to a scope index
            if(strcmp(ipAddress,ip)==0){
                scope=if_nametoindex(addr->ifa_name);
                break;
            }
        }
    }
    freeifaddrs(addrs);
    return scope;
}

SXError SXRetainSocket(SXSocketRef socket)
{
    if (socket->ref_count == sx_weak_object)
        return SX_SUCCESS;
    ++socket->ref_count;
    return SX_SUCCESS;
}

SXError SXReleaseSocket(SXSocketRef socket)
{
    if (socket->ref_count == sx_weak_object)
        return SX_SUCCESS;
    --socket->ref_count;
    if (socket->ref_count == 0)
        return SXFreeSocket(socket);
    return SX_SUCCESS;
}

SXError SXFreeSocket(SXSocketRef socket)
{
    close(socket->sockfd);
    sx_free(socket);
    socket = NULL;
    return SX_SUCCESS;
}

SXSocketRef SXCreateServerSocket(short port,
                                 int domain,
                                 int type,
                                 int protocol,
                                 SXError * err_ret)
{
    SXSocketRef sockPtr = (SXSocketRef)sx_calloc(1, sizeof(sx_socket_t));
    
    if (sockPtr == NULL)
        ERR_RET(SX_ERROR_MEM_ALLOC);
    
    sockPtr->domain = domain;
    sockPtr->type = type;
    sockPtr->protocol = protocol;
    sockPtr->ref_count = 1;
    sockPtr->port = port;
    socklen_t addrlen;
    
    memset(&sockPtr->addr, 0, sizeof(struct sockaddr_storage));
    
    if ((sockPtr->sockfd = socket(domain, type, protocol)) == -1) {
        perror("socket");
        ERR_RET(SX_ERROR_SYS_SOCKET);
    }
    
    switch (domain) {
        case AF_INET:
            sockaddr_in(sockPtr->addr).sin_addr.s_addr = INADDR_ANY;
            sockaddr_in(sockPtr->addr).sin_port = htons(port);
            sockaddr_in(sockPtr->addr).sin_len = sizeof(struct sockaddr_in);
            addrlen = sizeof(struct sockaddr_in);
            break;
        case AF_INET6:
            sockaddr_in6(sockPtr->addr).sin6_addr = in6addr_any;
            sockaddr_in6(sockPtr->addr).sin6_port = htons(port);
            sockaddr_in6(sockPtr->addr).sin6_len = sizeof(struct sockaddr_in6);
            addrlen = sizeof(struct sockaddr_in6);
            break;
        default:
            ERR_RET(SX_ERROR_INVALID_IPADDR);
            return NULL;
    }
    
    sockPtr->addr.ss_family = domain;
    
    int yes=1;
    
    if (setsockopt(sockPtr->sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        ERR_RET(SX_ERROR_SYS_SETSOCKOPT);
    }
    
    if (bind(sockPtr->sockfd, (struct sockaddr *)&sockPtr->addr, addrlen) == -1) {
        perror("bind");
        ERR_RET(SX_ERROR_SYS_BIND);
    }
    
    ERR_RET(SX_SUCCESS);
    
    return sockPtr;
}

SXSocketRef SXCreateClientSocket(const char * ipaddr,
                                 unsigned short port,
                                 int domain,
                                 int type,
                                 int protocol,
                                 SXError * err_ret)
{
    SXSocketRef sockPtr = (SXSocketRef)sx_calloc(1, sizeof(sx_socket_t));

    if (sockPtr == NULL)
        ERR_RET(SX_ERROR_MEM_ALLOC);
    
    sockPtr->protocol = protocol;
    sockPtr->type = type;
    sockPtr->domain = domain;
    sockPtr->ref_count = 1;
    socklen_t addrlen;

    memset(&sockPtr->addr, 0, sizeof(struct sockaddr_storage));
    
    if ((sockPtr->sockfd = socket(domain, type, protocol)) == -1) {
        perror("socket");
        ERR_RET(SX_ERROR_SYS_SOCKET);
    }
    
    switch (domain) {
        case AF_INET:
            sockaddr_in(sockPtr->addr).sin_port = htons(port);
            sockaddr_in(sockPtr->addr).sin_len = sizeof(struct sockaddr_in);
            addrlen = sizeof(struct sockaddr_in);
            inet_pton(AF_INET, ipaddr, &sockaddr_in(sockPtr->addr).sin_addr);
            break;
        case AF_INET6:
            sockaddr_in6(sockPtr->addr).sin6_port = htons(port);
            sockaddr_in6(sockPtr->addr).sin6_len = sizeof(struct sockaddr_in6);
            addrlen = sizeof(struct sockaddr_in6);
            inet_pton(AF_INET6, ipaddr, &sockaddr_in6(sockPtr->addr).sin6_addr);
            break;
        default:
            ERR_RET(SX_ERROR_INVALID_IPADDR);
            return NULL;
    }
    
    sockPtr->addr.ss_family = domain;
    
    ERR_RET(SX_SUCCESS);
    
    return sockPtr;
}

SXError SXSocketSetBlock(SXSocketRef sock, bool block)
{
    int flags;
    if ((flags = fcntl(sock->sockfd, F_GETFL, 0)) < 0)
        return SX_ERROR_SYS_FCNTL;
    flags = block ? (flags &~ O_NONBLOCK) : (flags | O_NONBLOCK);
    if ((flags = fcntl(sock->sockfd, F_SETFL, flags)) < 0)
        return SX_ERROR_SYS_FCNTL;
    sock->blocking = block;
    return SX_SUCCESS;
}

SXError SXSocketConnect(SXSocketRef socket)
{
    if (connect(socket->sockfd,
                (struct sockaddr *)&(socket->addr),
                socket->domain == AF_INET6 ?
                sizeof(struct sockaddr_in6) :
                sizeof(struct sockaddr_in)) == -1)
        return SX_ERROR_SYS_CONNECT;
    return SX_SUCCESS;
}

SXError SXSocketSend(SXSocketRef target, const void * payload, size_t length)
{
    if (send(target->sockfd, payload, length, 0) == -1)
        return SX_ERROR_SYS_SEND;
    return SX_SUCCESS;
}


SXError SXSocketListen(SXSocketRef socket)
{
    if (listen(socket->sockfd, 10) == -1) {
        perror("listen");
        return SX_ERROR_SYS_LISTEN;
    }
    return SX_SUCCESS;
}








