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
//#include "SXClient.h"
//
//#define has_handler(fname) !(use_block_handler ? client->CAT(fname, _block) == NULL : client->CAT(fname, _fptr) == NULL)
//
//#define SERVER_HAS_STATUS(stat) (client->status == stat)
//
//#define use_fn(fname, args) client->CAT(fname, _fptr) args
//
//#ifdef __BLOCKS__
//#define use_block(fname, args) client->CAT(fname, _block) args
//#define eval(fname, args) (use_block_handler ? use_block(fname, args) : use_fn(fname, args))
//#else
//#define eval(fname, args) (use_fn(fname, args))
//#endif
//
//#define eval_if_exist(fname, args) if (has_handler(fname)) eval(fname, args);
//
//
//
//SXError SXRetainClient(SXClientRef client) {
//    if (client->ref_count == sx_weak_object)
//        return SX_SUCCESS;
//    ++client->ref_count;
//    return SX_SUCCESS;
//}
//SXError SXReleaseClient(SXClientRef client) {
//    if (client->ref_count == sx_weak_object)
//        return SX_SUCCESS;
//    --client->ref_count;
//    if (client->ref_count == 0)
//        return SXFreeClient(client);
//    return SX_SUCCESS;
//}
//
//SXError SXFreeClient(SXClientRef client) {
//
//    SXReleaseSocket(client->socket);
//    sx_free(client);
//    client = NULL;
//    return SX_SUCCESS;
//}
//
//void SXStartClient_h(SXClientRef client, bool use_block_handler);
//
//SXClientRef SXCreateClientWithHostname(const char * hostname,
//                                const char * service,
//                                int sock_type,
//                                int preferred_af_inet,
//                                int buf_size,
//                                dispatch_queue_priority_t priority,
//                                SXError * err_ret)
//{
//    SXClientRef client = (SXClientRef)sx_calloc(1, sizeof(sx_client_t));
//    client->type = sx_client_use_hostname;
//    client->priority = priority;
//    client->buf_size = buf_size;
//    client->preferred_af_inet = preferred_af_inet;
//    client->sock_type = sock_type;
//    client->ref_count = 1;
//    strncpy(client->target, hostname, 255);
//
//    strncpy(client->service, service, 55);
//    return client;
//}
//
//SXClientRef SXCreateClientWithIp(const char * ip,
//                                 unsigned short port,
//                                 int sock_domain,
//                                 int sock_type,
//                                 int sock_protocol,
//                                 int buf_size,
//                                 dispatch_queue_priority_t priority,
//                                 SXError * err_ret)
//{
//    SXClientRef client = (SXClientRef)sx_calloc(1, sizeof(sx_client_t));
//    client->priority = priority;
//    client->preferred_af_inet = sock_domain;
//    client->type = sx_client_use_ipaddr;
//    client->sock_type = sock_type;
//    client->port = port;
//    client->protocol = sock_protocol;
//    client->buf_size = buf_size;
//    client->ref_count = 1;
//    strncpy(client->target, ip, 255);
//    return client;
//}
//
//void SXStartClient_ip(SXClientRef client, bool use_block_handler)
//{
//    client->socket = SXCreateClientSocket(client->target, client->port, client->preferred_af_inet, client->sock_type, client->protocol, NULL);
//    
//    if (SXSocketConnect(client->socket) != SX_SUCCESS) {
//        perror("connect");
//        return;
//    }
//    
//    dispatch_async(dispatch_get_global_queue(client->priority, 0), ^{
//        char * buf = (char *)sx_calloc(client->buf_size, sizeof(char));
//        size_t s;
//        memset(buf, 0, sizeof(char) * client->buf_size);
//        
//        while ((s = recv(client->socket->sockfd, buf, client->buf_size, 0)) > 0) {
//            if (s == -1) perror("recv");
//            eval(dataHandler, (client, buf, s));
//            memset(buf, 0, sizeof(char) * client->buf_size);
//        }
//        
//        if (s == -1) perror("recv");
//
//        eval_if_exist(disconnect, (client));
//        
//        SXReleaseClient(client);
//    });
//}
//
//
//void SXStartClient(SXClientRef client, bool use_block_handler) {
//    switch (client->type) {
//        case sx_client_use_hostname:
//            
//            SXStartClient_h(client, use_block_handler);
//            break;
//            
//        case sx_client_use_ipaddr:
//            
//            SXStartClient_ip(client, use_block_handler);
//            break;
//    }
//}
//
//
//void SXStartClient_h(SXClientRef client, bool use_block_handler) {
//    
//    struct addrinfo * info;
//    struct addrinfo * cinfo;
//;
//    int err;
//    
//    if ((err = getaddrinfo( client->target, client->service, NULL, &info) ) != 0)
//        eval_if_exist(errHandler, (client, SX_ERROR_SYS_GET_ADDR_INFO));
//    
//    for (cinfo = info; cinfo; cinfo = cinfo->ai_next) {
//
//        char buf[50];
//        memset(buf, 0, 50);
//        
//        if (cinfo->ai_family == AF_INET)
//            inet_ntop(info->ai_family, &((struct sockaddr_in *)cinfo->ai_addr)->sin_addr, buf, info->ai_addrlen);
//        else if (info->ai_family == AF_INET6)
//            inet_ntop(info->ai_family, &((struct sockaddr_in6 *)cinfo->ai_addr)->sin6_addr, buf, info->ai_addrlen);
//        
//        client->port = htons((unsigned short)getservbyname(client->service, NULL)->s_port);
//        
//        client->socket = SXCreateClientSocket(buf, client->port, info->ai_family, client->sock_type, 0, NULL);
//        
//        if (SXSocketConnect(client->socket) != SX_SUCCESS) {
//            perror("connect");
//            SXReleaseSocket(client->socket);
//            continue;
//        }
//        
//        freeaddrinfo(info);
//        break;
//    }
//    
//    dispatch_async(dispatch_get_global_queue(client->priority, 0), ^{
//        char * buf = (char *)sx_calloc(client->buf_size, sizeof(char));
//        size_t s;
//        memset(buf, 0, sizeof(char) * client->buf_size);
//        
//        while ((s = recv(client->socket->sockfd, buf, client->buf_size, 0)) > 0) {
//            
//            if (s == -1) perror("recv");
//            eval(dataHandler, (client, buf, s));
//        }
//        
//        if (s == -1) perror("recv");
//        
//        eval_if_exist(disconnect, (client));
//        
//        sx_free(buf);
//        SXReleaseClient(client);
//    });
//}
//
//#undef SERVER_HAS_STATUS
//#undef use_block
//#undef use_fn
//#undef eval
//#undef eval_if_exist
//#undef has_handler
