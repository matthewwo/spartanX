
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


SXStringRef parseRequest(SXStringRef string) {
    
    SXVectorRef lines = SXStringSubStringsTrimmedByCString(string, "\r\n", 2);
    SXStringRef hostname = NULL;
    
    if (lines->count >= 2) {
        
        {
            SXStringRef secline = *(SXStringRef *)SXVectorObjectPtrAtIndex(lines, 1);
            SXVectorRef a = SXStringSubStringsTrimmedByCString(secline , " ", 1);
            if (a->count >= 2) {
                hostname = *(SXStringRef *)SXVectorObjectPtrAtIndex(a, 1);
                SXRetainString(hostname);
            }
            SXReleaseVector(a);
        }
    }
    
    SXReleaseVector(lines);
    
    return hostname;
}



int main(int argc, const char * argv[]) {
    sx_server_setup setup = {AF_INET, 8080, 50, 819200, 500, false};
    SXServerRef server = SXCreateServer(setup, NULL, ^size_t(sx_runtime_object_t *queue, void *data, size_t length) {

        
        if (queue->udata == NULL) {
            
            SXStringRef raw = SXCreateStringWithCString((char *)data, length);
            SXStringRef host = parseRequest(raw);
            SXReleaseString(raw);
            
            if (host) {
                queue->udata = SXCreateClientWithHostname(host->chars, "http", NULL, 5000, GCD_HIGH, NULL);
                SXReleaseString(host);
        
                SXRuntimeObjectSetBlockDidReceive(sx_runtime_obj_cast(queue->udata), ^size_t(sx_runtime_object_t *object, void *data, size_t length) {
                    
                    SXSocketSend(queue->sock, data, length);
                    return length;
                });
                
                SXStartClient(sx_client_ref(queue->udata), data, length);
            }
            return length;
            
        } else {
            if (length == 0) {
                SXRelease(queue->udata);
            } else {
                SXSocketSend(sx_runtime_obj_get_sock(queue->udata), data, length);
            }
        }
        
        return length;
    });
    
    SXServerStart2(server, GCD_HIGH);
    
    while (1) {
//        sleep(10000);
    }
}
