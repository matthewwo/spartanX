# SpartanX - asynchronous socket framework

SpartanX is an extremely easy to use asynchronous socket programming framework designed for writing applications that require
 - to program in native programming languages (C, C++, Obj-C, swift)
 - no-middle-man. With SpartanX, what you see is what the socket sees.
 - ability to handle raw TCP payload.
 - ability to assign different threading policy dynamically in runtime.
 - ability to suspend/ignore single connection and resume.
 - ability to manage and host multiple different services
 - ability to use GPU (OpenCL) directly to process connections. (OS X only)
 - low latency and a lot of control over the connection cycle, for example discrete computing over local network.
 - on-demand server capability, for example a temporary web-server/key server/verification server 

SpartanX is build directly on top of UNIX socket, and use libdispatch as its threading library. Therefore, clang (with -fblocks enabled) and libdispatch is required in order to build SpartanX. 
# Structure
In spartanX, server are a group of handlers which will call at different phase of the connection cycle for connections coming from a single port. Different server can share their handler by simply assign their handler to each other. Handlers can be blocks or [function pointer(when in C) | std::function(when C++)].

Each connection formed will be represent as an 'sx_queue_t` object, which is defined as
```C
typedef struct sx_queue
{
// which gcd_queue is operating on
    __unsafe_unretained dispatch_queue_t gcd_queue;
// which socket
    SXSocketRef sock;
// status
    sx_status_t status;
    unsigned int ref_count;
} sx_queue_t;
```
connection will keep alive as long as the syscall `recv` returns a value greater than 0 and if `didReceiveDataHandler` returns a value larger than 0. Each `sx_queue_t` can be suspended and resumed by using the appropriate function. In the suspend mode, even though the socket will still recv from the opposite side, but the data will never pass to any of the handler except `didDisconnect`. Each connection is running on top of a `dispatch_queue_t`, which the policy of how the `dispatch_queue_t` should generate are user-defined. Since the `dispatch_queue_t` will not create until an actually connection is accepted, it is possible to optimize the application in runtime by assigning different `dispatch_queue_t` according to different situation. This mode is good for services that require complicated server-side computing and also connections that require server work seamlessly with client. 

spartanX also provides a kqueue version of the server runloop, which is close to the traditional `poll` and `select` way to accomplish async-io. This mode is very useful for applications that does not require a lot of workload per connection but many connections, such as HTTP server.
# Usage
To build a simple server with SpartanX, even in C, you only really need two lines.
```C
#include "spartanX.h"
int main(int argc, const char * argv[]) {
// #1
    SXServerRef server = SXCreateServer({SX_SERVER_DOMAIN_USE_IPV4, 5000, 5, 1024, 10, false}, NULL,
    ^size_t(SXServerRef server, SXQueueRef queue, void *data, size_t length) {
            printf("%s\n", data);
            return length;
    });
    
// #2    
    SXServerStart2(server, DISPATCH_QUEUE_PRIORITY_LOW, true);
    
    while (1) {}
    return 0;
}
```

SpartanX can also take function pointer (std::function if you compile it as c++ or use it in c++ project) as handler.
The following is the equivalent code:
```C
// In C
#include "spartanX.h"
size_t handle_message(SXServerRef server, SXQueueRef queue, void * data, size_t length) {
    printf("%s\n", data);
    return length;
}

int main(int argc, const char * argv[]) {
    SXServerRef server = SXCreateServer_f({SX_SERVER_DOMAIN_USE_IPV4, 5000, 5, 1024, 10, false},
                                        NULL, 
                                        &handle_message);
    
    SXServerStart2(server, DISPATCH_QUEUE_PRIORITY_LOW, true);
    
    while (1) {
        
    }
    return 0;
}
```

Or use lamda expression when you compile it in C++ 1x
```C++
#include "spartanX.h"

int main(int argc, const char * argv[]) {
    SXCreateServer_f({SX_SERVER_DOMAIN_USE_IPV4, 5000, 5, 1024, 10, false}, NULL,
                     [](SXServerRef server, SXQueueRef queue, void * data, size_t length){
                         printf("%s\n", data);
                         return length;
                     });
    SXServerStart2(server, DISPATCH_QUEUE_PRIORITY_LOW, true);
    while (1) {}
    return 0;
}
```

There are several different handlers available, which will occurs at different stage of the connection, which are:
 - didStart  
 - shoudlConnect
 - didConnect
 - didReceiveData
 - didDisconnect
 - willSuspend
 - didResume
 - willKill
 - didKill
 
### Development

SpartanX is still under development. Please feel free to try out and report bugs.

The followings are features in progress
 - error handling of queue runtime
 - fail-able server 
 - IPC socket
 - Enhance UDP support
 - UDP "Server"
 - memory warning

