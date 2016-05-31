
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
//  Created by yuuji on 5/29/16.
//  Copyright © 2016 yuuji. All rights reserved.

#include <stdio.h>
#include "spartanX.h"


/* provide basic data structure
 * for parsing
 */
#include "SXVector.h"
#include "SXString.h"

/* a very minimal way to check if the uri is hostname or ip.
 * notice that this example does not parse ipv6 and port num,
 * however, it is pretty easy to parse and implement by yourself
 */
bool requestIsHostname(SXStringRef string) {
    struct sockaddr_storage a;
    return inet_pton(AF_INET, string->chars, &a);
}

/*  Trim protocol-like scheme
 *  for example: [http://]
 */
void trimURLProtocol(SXStringRef string, size_t offset) {
    size_t loca = SXStringSubStringLocation(string, "://", 3, 0);
    SXStringRemovingRange(string, (sx_range_t){loca - offset, loca - offset + 3});
}

/*   This function rewrite the http request (string)
 *   as shown as below, and return the hostname for
 *   for dns loopup.
 *
 *   GET http://www.example.org/index.html HTTPx/x
 *   Host: www.example.org
 *
 *   to the form
 *
 *   GET /index.html
 *   Host: www.example.org
 */
SXStringRef parseRequest(SXStringRef string) {
    
    SXVectorRef lines = SXStringSubStringsTrimmedByCString(string, "\r\n", 2);
    SXStringRef firstline = *(SXStringRef *)SXVectorObjectPtrAtIndex(lines, 0);
    SXStringRef originalFirstline = SXCreateStringWithCString(firstline->chars, firstline->length);
    SXStringRef domain;
    
    {
        SXVectorRef is = SXStringSubCStringLocations(firstline, " ", 1);
        size_t loca = *(int *)SXVectorObjectPtrAtIndex(is, 0);
        trimURLProtocol(firstline, loca + 1);
        
        
        SXStringRef secline = *(SXStringRef *)SXVectorObjectPtrAtIndex(lines, 1);
        SXVectorRef a = SXStringSubStringsTrimmedByCString(secline , " ", 1);
        
        SXStringRef hostname = *(SXStringRef *)SXVectorObjectPtrAtIndex(a, 1);
        domain = SXCreateStringWithCString(hostname->chars, hostname->length);
        
        // rewrite the first occur is good enough for us.
        SXStringRemoveOcurrenceOfCString(firstline, hostname->chars, hostname->length);
        
        SXReleaseVector(a);
        SXReleaseVector(is);
    }
    
    SXStringReplacingOccurrencesOfString(string, originalFirstline, firstline);
    SXReleaseString(originalFirstline);
    SXReleaseVector(lines);
    
    return domain;
}

block_SXClientDidReceive cl = ^size_t(SXClientRef client, void * data, size_t length) {
    SXRetainSocket(client->socket);
    if (((SXQueueRef)client->udata)->sock == NULL) {
        // just for safe, should never call
        printf("SOcket is NULL\n");
    }else {
        SXSocketSend(((SXQueueRef)client->udata)->sock, data, length);
    }
    SXReleaseSocket(client->socket);
    return length;
};


int main(int argc, const char * argv[]) {
    
    /* The first arugment of `SXCreateServer` takes a struct
     * defined as:
     *
     *      typedef struct sx_server_setup {
     *          unsigned domain;
     *          unsigned port;
     *          size_t   backlog;
     *          size_t   dataSize;
     *          size_t   max_guest;
     *          bool     failable;  //reserved, not yet implemented
     *      } sx_server_setup;
     *
     * the `NULL` part will return errors, but it is
     * not yet implemented.
     */
    
//    SXServerRef proxyServer = SXCreateServer({AF_INET, 8080, 600000, 20480, 5000, false}, NULL, ^size_t(SXServerRef server, SXQueueRef queue, void *data, size_t length) {
    
    sx_server_setup setup = {AF_INET, 8080, 600000, 20480, 5000, false};
    
    SXServerRef proxyServer = SXCreateServer(setup, NULL, ^size_t(SXServerRef server, SXQueueRef queue, void *data, size_t length) {
        // yes, the data handler will be call even when recv() returns 0 (disconnect)
        // the framework is designed to offer as much info as possible
        if (length == 0) return 0;
        
        SXStringRef string = SXCreateStringWithCString((char *)data, length + 1);
        SXStringRef domain = parseRequest(string);
        
        if (queue->udata == NULL) {
            
            SXClientRef client;
            
            
            /* if you're interested to build a more functional proxy,
             * feel free to parse and pass the port number, or the
             * service
             */
            if (requestIsHostname(domain))
                client = SXCreateClientWithIp(domain->chars,
                                              80,
                                              AF_INET,
                                              SOCK_STREAM,
                                              0,
                                              50000,
                                              DISPATCH_QUEUE_PRIORITY_HIGH,
                                              NULL);
            else
                client = SXCreateClientWithHostname(domain->chars,
                                                    "http", SOCK_STREAM,
                                                    AF_INET,
                                                    50000,
                                                    DISPATCH_QUEUE_PRIORITY_HIGH,
                                                    NULL);
            
            
            client->dataHandler_block = cl;
            
            // use the udata of the queue and client to bridge them
            queue->udata = client;
            client->udata = queue;
            
            client->disconnect_block = ^(SXClientRef client) {
                // put whatever you want to do when client finished its section
            };
            
            SXStartClient(client, true);
            
        }
        
        if (((SXClientRef)(queue->udata))->socket != NULL)
            SXSocketSend(((SXClientRef)(queue->udata))->socket, string->chars, string->length);
        
        SXReleaseString(string);
        SXReleaseString(domain);
        
        return length;
    });
    
    // uncomment to use kqueue, which should perform better for
    // discrete connections.
    //SXServerStart_kqueue(proxyServer, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), true);
    
    // One of the options to start pure-async service,
    // give you more real-time control over the connection.
    // however performance will decrease for massive but
    // short connections due to the overhead to create queue.
    SXServerStart2(proxyServer, DISPATCH_QUEUE_PRIORITY_HIGH, true);
    
    while (1) {
        // keep the process alive, yet put the main
        // thread to sleep to save energy and cpu usage
        sleep(100000);
    }
    
    return 0;
}
