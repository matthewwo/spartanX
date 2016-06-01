//
//  SXEventHandler.h
//  http-proxy
//
//  Created by Yuji on 6/1/16.
//  Copyright © 2016 YS. All rights reserved.
//

#ifndef SXEventHandler_h
#define SXEventHandler_h

struct sx_server;
struct sx_queue;

typedef struct sx_server * SXServerRef;
typedef struct sx_queue * SXQueueRef;

#define FRTR_N(name) fptr_##name

#define CAT_I(a,b) a##b
#define CAT(a,b) CAT_I(a, b)

#ifdef __cplusplus

#define FPTR(ret, name, args) typedef std::function<ret args> fptr_##name;
#else

#define FPTR(ret, name, args) typedef ret (fptr_##name args);
#endif

#ifdef __BLOCKS__
#define BLOCK_N(name) block_##name
#define BLOCK(ret, name, args) typedef ret(^block_##name) args;

#ifdef __Swift
#define SWIFT_N(name) typedef BLOCK_N(name) ##name;
#else
#define SWIFT_N(name)
#endif
#else
#define BLOCK_N(name)
#define BLOCK(ret, name, args)
#endif

#define hDeclare(ret, name, args)\
BLOCK(ret, name, args);\
FPTR(ret, name, args);\
SWIFT_N(name);

hDeclare(size_t,
         SXServerDidReceive,
         (SXServerRef server, SXQueueRef queue, void * data, size_t length));

hDeclare(void,
         SXServerDidStart,
         (SXServerRef server));

hDeclare(bool,
         SXServerShouldConnect,
         (SXServerRef server, SXSocketRef socket));

hDeclare(void,
         SXServerDidConnect,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerDidDisconnect,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerWillKill,
         (SXServerRef server,  SXQueueRef const queue));

hDeclare(void,
         SXServerDidKill, (SXServerRef server));

hDeclare(void,
         SXServerDidResume,
         (SXServerRef server, SXQueueRef queue));

hDeclare(void,
         SXServerWillSuspend,
         (SXServerRef server, SXQueueRef queue));

hDeclare(dispatch_queue_t,
         queue_generate_policy,
         ());

#undef FRTR_N
#undef FPTR
#undef BLOCK_N
#undef BLOCK

#define transfer_fn_p(obj1, obj2, fname) (obj1->CAT(fname, _fptr) = obj2->CAT(fname, _fptr))
#define transfer_fn_b(obj1, obj2, fname) (obj1->CAT(fname, _block) = obj2->CAT(fname, _block))

#define transfer_fn(obj1, obj2, fname) transfer_fn_p(obj1, obj2, fname); transfer_fn_b(obj1, obj2, fname);

#define SXServerHandlers_block \
__unsafe_unretained block_SXServerDidConnect    didConnect_block;\
__unsafe_unretained block_SXServerDidReceive    dataHandler_block;\
__unsafe_unretained block_SXServerDidDisconnect didDisconnect_block;\
__unsafe_unretained block_SXServerWillSuspend   willSuspend_block;\
__unsafe_unretained block_SXServerDidResume     didResume_block;\
__unsafe_unretained block_SXServerWillKill      willKill_block;\

#define SXServerHandlers_c_fnptr \
fptr_SXServerDidConnect *   didConnect_fptr;\
fptr_SXServerDidReceive*    dataHandler_fptr;\
fptr_SXServerDidDisconnect* didDisconnect_fptr;\
fptr_SXServerWillSuspend*   willSuspend_fptr;\
fptr_SXServerDidResume*     didResume_fptr;\
fptr_SXServerWillKill*      willKill_fptr;

#define SXServerHandlers_cpp_fn \
fptr_SXServerDidConnect    didConnect_fptr;\
fptr_SXServerDidReceive    dataHandler_fptr;\
fptr_SXServerDidDisconnect didDisconnect_fptr;\
fptr_SXServerWillSuspend   willSuspend_fptr;\
fptr_SXServerDidResume     didResume_fptr;\
fptr_SXServerWillKill      willKill_fptr;

void SXServerSetBlockDidStart       (SXServerRef server, block_SXServerDidStart         );
void SXServerSetBlockShouldConnect  (SXServerRef server, block_SXServerShouldConnect    );
void SXServerSetBlockDidConnect     (SXServerRef server, block_SXServerDidConnect       );
void SXServerSetBlockDidReceive     (SXServerRef server, block_SXServerDidReceive       );
void SXServerSetBlockDidDisconnect  (SXServerRef server, block_SXServerDidDisconnect    );
void SXServerSetBlockWillSuspend    (SXServerRef server, block_SXServerWillSuspend      );
void SXServerSetBlockDidResume      (SXServerRef server, block_SXServerDidResume        );
void SXServerSetBlockWillKill       (SXServerRef server, block_SXServerWillKill         );
void SXServerSetBlockDidKill        (SXServerRef server, block_SXServerDidKill          );

void SXServerSetFnPtrDidStart       (SXServerRef server, fptr_SXServerDidStart          );
void SXServerSetFnPtrShouldConnect  (SXServerRef server, fptr_SXServerShouldConnect     );
void SXServerSetFnPtrDidConnect     (SXServerRef server, fptr_SXServerDidConnect        );
void SXServerSetFnPtrDidReceive     (SXServerRef server, fptr_SXServerDidReceive        );
void SXServerSetFnPtrDidDisconnect  (SXServerRef server, fptr_SXServerDidDisconnect     );
void SXServerSetFnPtrWillSuspend    (SXServerRef server, fptr_SXServerWillSuspend       );
void SXServerSetFnPtrDidResume      (SXServerRef server, fptr_SXServerDidResume         );
void SXServerSetFnPtrWillKill       (SXServerRef server, fptr_SXServerWillKill          );
void SXServerSetFnPtrDidKill        (SXServerRef server, fptr_SXServerDidKill           );

#endif /* SXEventHandler_h */
