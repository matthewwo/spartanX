//
//  types.h
//  c
//
//  Created by Yuji on 5/21/16.
//  Copyright © 2016 YS. All rights reserved.
//

#ifndef types_h
#define types_h

#ifndef __OBJC__
#define __unsafe_unretained
#define __bridge
#endif


#include <dispatch/dispatch.h>

#ifndef __APPLE__
typedef long dispatch_queue_priority_t;
#endif


#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
    
    
#if defined(__i386__) || defined(__amd64__)

#include <gperftools/tcmalloc.h>
    
#define sx_calloc(nmem, size)   tc_calloc(nmem, size)
#define sx_malloc(size)         tc_malloc(size)
#define sx_free(obj)            tc_free(obj)
#define sx_realloc(ptr, size)   tc_realloc(ptr, size)
    
#else 

#define sx_calloc(nmem, size)   calloc(nmem, size)
#define sx_malloc(size)         malloc(size)
#define sx_free(obj)            free(obj)
#define sx_realloc(ptr, size)   realloc(ptr, size)


#endif

    
#ifdef __cplusplus
}
#endif


#define GCD_DEFAULT     DISPATCH_QUEUE_PRIORITY_DEFAULT
#define GCD_LOW         DISPATCH_QUEUE_PRIORITY_LOW
#define GCD_HIGH        DISPATCH_QUEUE_PRIORITY_HIGH
#define GCD_BACKGROUND  DISPATCH_QUEUE_PRIORITY_BACKGROUND

#define contains_flag(options, option) ((options & option) == option)

#define sx_weak_object      410

typedef unsigned int sx_status_t;

typedef unsigned    short   sx_uint16;
typedef             short   sx_int16;
typedef unsigned    int     sx_uint32;
typedef             int     sx_int32;
typedef unsigned    long    sx_uint64;
typedef             long    sx_int64;
typedef             char    sx_byte;

enum sx_status {
    sx_status_idle      = 0,
    sx_status_running   = 1,
    sx_status_resuming  = 2,
    sx_status_suspend   = 3,
    sx_status_should_terminate = 4,
    sx_status_ignored   = 5,
    sx_status_should_kill = 6
};

#endif /* types_h */
