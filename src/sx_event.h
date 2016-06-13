//
//  SXEventHandler.h
//  http-proxy
//
//  Created by Yuji on 6/1/16.
//  Copyright © 2016 YS. All rights reserved.
//

#ifndef SXEventHandler_h
#define SXEventHandler_h

#include <stdlib.h>
#include <stdbool.h>
#include <dispatch/dispatch.h>
#include "sx_object.h"
#include "SXError.h"

typedef struct sx_runtime_object sx_runtime_object_t;
typedef struct sx_server * SXServerRef;
typedef struct sx_socket * SXSocketRef;

#define CAT_I(a,b) a##b
#define CAT(a,b) CAT_I(a, b)

typedef void    (^didStart)     (SXServerRef server                                             );
typedef bool    (^shouldConnect)(SXServerRef server, SXSocketRef socket);
typedef void    (^didKill)      (SXServerRef server);


typedef void    (^didConnect)   (sx_runtime_object_t * object);
typedef void    (^didDisconnect)(sx_runtime_object_t * object);
typedef void    (^willKill)     (sx_runtime_object_t * object);

typedef size_t  (^didReceive)   (sx_runtime_object_t * object, void * data, size_t length);
typedef void    (^didRecvError) (sx_runtime_object_t * object, SXError err);

typedef void    (^didResume)    (sx_runtime_object_t * object);
typedef void    (^willSuspend)  (sx_runtime_object_t * object);

typedef dispatch_queue_t(^block_queue_generate_policy)();

#define transfer_fn_p(obj1, obj2, fname) (obj1->CAT(fname, _fptr) = obj2->CAT(fname, _fptr))
#define transfer_fn_b(obj1, obj2, fname) (obj1->CAT(fname, _block) = obj2->CAT(fname, _block))

#define transfer_fn(obj1, obj2, fname) transfer_fn_b(obj1, obj2, fname);

#define sx_runtime_status_handler \
__unsafe_unretained willSuspend   willSuspend_block;\
__unsafe_unretained didResume     didResume_block;\

#define sx_runtime_object_handler \
__unsafe_unretained didConnect    didConnect_block;\
__unsafe_unretained didDisconnect didDisconnect_block;\
__unsafe_unretained willKill      willKill_block;\

#define sx_runtime_data_handler \
__unsafe_unretained didReceive    dataHandler_block;\
__unsafe_unretained didRecvError  errHandler_block;

#define SXServerStatusHandlers \
__unsafe_unretained didStart didStart_block;\
__unsafe_unretained shouldConnect shouldConnect_block;\
__unsafe_unretained didKill didKill_block;


void SXServerSetBlockDidStart       (SXServerRef server, didStart               );
void SXServerSetBlockShouldConnect  (SXServerRef server, shouldConnect          );
void SXServerSetBlockDidKill        (SXServerRef server, didKill                );

void SXRuntimeObjectSetBlockDidConnect     (sx_runtime_object_t * object, didConnect     );
void SXRuntimeObjectSetBlockDidDisconnect  (sx_runtime_object_t * object, didDisconnect  );
void SXRuntimeObjectSetBlockWillKill       (sx_runtime_object_t * object, willKill       );

void SXRuntimeObjectSetBlockDidReceive     (sx_runtime_object_t * object, didReceive     );
void SXRuntimeObjectSetBlockDidRecvError   (sx_runtime_object_t * object, didRecvError   );

void SXRuntimeObjectSetBlockWillSuspend    (sx_runtime_object_t * object, willSuspend    );
void SXRuntimeObjectSetBlockDidResume      (sx_runtime_object_t * object, didResume      );

#endif /* SXEventHandler_h */
