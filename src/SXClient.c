
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

#include "SXClient.h"

SXError SXFreeClient(SXClientRef client) {
    SXRelease(client->sock);
    sx_free(client->buf);
    sx_free(client);
    client = NULL;
    return SX_SUCCESS;
}

void killClient(SXClientRef client) {
    SXSocketSend(client, NULL, 0);
}

SXClientRef SXCreateClientWithHostname(const char * hostname,
                                const char * service,
                                struct addrinfo * hint,
                                int buf_size,
                                dispatch_queue_priority_t priority,
                                SXError * err_ret)
{
    SXClientRef client = (SXClientRef)sx_calloc(1, sizeof(sx_client_t));
    sx_runtime_obj_init(client, &SXFreeClient);
    client->type = sx_client_use_hostname;
    client->priority = priority;
    client->sock = SXCreateClientSocketByHostname(hostname, service, hint, err_ret);
    client->dataSize = buf_size;
    client->buf = (sx_byte *)sx_calloc(client->dataSize, sizeof(sx_byte));
    return client;
}

SXClientRef SXCreateClientWithIp(const char * ip,
                                 unsigned short port,
                                 int sock_domain,
                                 int sock_type,
                                 int sock_protocol,
                                 int buf_size,
                                 dispatch_queue_priority_t priority,
                                 SXError * err_ret)
{
    SXClientRef client = (SXClientRef)sx_calloc(1, sizeof(sx_client_t));
    sx_runtime_obj_init(client, &SXFreeClient);
    client->type = sx_client_use_hostname;
    client->priority = priority;
    client->sock = SXCreateClientSocket(ip, port, sock_domain, sock_type, sock_protocol, err_ret);
    client->dataSize = buf_size;
    client->buf = (sx_byte *)sx_calloc(client->dataSize, sizeof(sx_byte));
    return client;
}

SXError SXStartClient(SXClientRef client, void * initialPayload, size_t len) {
    SXError err;
    
    if ((err = SXSocketConnect(client->sock)) != SX_SUCCESS)
        return err;
    
    if (initialPayload != NULL)
        SXSocketSend(client->sock, initialPayload, len);
    
    __block size_t s = 0;
    client->status = sx_status_running;
//    printf("%s\n", inet_ntoa(((struct sockaddr_in *)(&client->sock->addr))->sin_addr));
    dispatch_async(dispatch_get_global_queue(client->priority, 0), ^{

        bool suspended = false;
        
        do {
            memset(client->buf, 0, sizeof(sx_byte) * client->dataSize);
            switch (client->status) {
                case sx_status_running:
                    
                    s = recv(client->sock->sockfd,client->buf, client->dataSize, 0);
                    
                    if (s == -1 || 0 || client == NULL) {
                        perror("recv");
                        goto exit;
                    }
                    if (client->dataHandler_block != NULL)
                        s = client->dataHandler_block(client, client->buf, s);
                    break;
                
                case sx_status_resuming:
                    client->status = sx_status_running;
                    if (client->didResume_block != NULL)
                        client->didResume_block(client);
                    break;
                    
                case sx_status_suspend:
                    if (!suspended)
                        if (client->willSuspend_block != NULL)
                            client->willSuspend_block(client);
                    suspended = true;
                    
                    size_t len = recv(client->sock->sockfd, client->buf, client->dataSize, client->recvFlags);
                    if (len == 0 || len == -1 || client == NULL) goto exit;
                    
                    switch (client->status) {
                        case sx_status_should_terminate:
                        case sx_status_idle:
                            goto exit;
                            
                        case sx_status_resuming:
                        case sx_status_running:
                            if (client->dataHandler_block != NULL)
                                s = client->dataHandler_block(client, client->buf, s);
                            break;
                            
                        default:
                            break;
                    }
                    break;
                    
                default:
                    break;
            }
            
        } while (s > 0);
        
    exit:

        SXRelease(client->sock);
        if (client->didDisconnect_block != NULL)
            client->didDisconnect_block(client);
        SXRelease(client);
    });

    return err;
}

#undef SERVER_HAS_STATUS
#undef use_block
#undef use_fn
#undef eval
#undef eval_if_exist
#undef has_handler
