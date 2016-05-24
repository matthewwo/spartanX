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

#endif /* types_h */
