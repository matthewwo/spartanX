//
//  SXString.h
//  cspartanX
//
//  Created by Yuji on 5/29/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#ifndef SXString_h
#define SXString_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "SXError.h"
#include "sx_global.h"
#include "SXVector.h"

typedef struct sx_string * SXStringRef;
typedef struct sx_vector * SXVectorRef;

#define getSubstringInSubStrings(vec, i) *(SXStringRef *)SXVectorGetObjectAtIndex(vec, i)

typedef struct sx_range {
    size_t loca, len;
} sx_range_t;

typedef struct sx_string {
    size_t length;
    int ref_count;
    char * chars;
} sx_string_t;

sx_string_t SXCreateString(char * cstr, size_t length);

SXStringRef SXCreateStringWithCString(const char * cstr, size_t length);

bool SXStringContainsCString(SXStringRef string, const char * cstr);

SXStringRef SXStringGetSubstringInRange(SXStringRef o_str, sx_range_t range);

SXVectorRef SXStringSubStringLocations(SXStringRef o_str, SXStringRef trim);

SXVectorRef SXStringSubCStringLocations(SXStringRef o_str, const char *trim, size_t len);

size_t SXStringSubStringLocation(SXStringRef o_str, const char * trim, size_t len, size_t n);

SXVectorRef SXStringSubStringsTrimmedByString(SXStringRef target, SXStringRef trimstr);
SXVectorRef SXStringSubStringsTrimmedByCString(SXStringRef target, const char * trimcstr, size_t trimstr_len);

void SXStringReplacingOccurrencesOfString(SXStringRef string, SXStringRef target, SXStringRef replacement);
void SXStringReplacingOccurrencesOfCString(SXStringRef string, const char * ctarget, const char * creplacement);

SXStringRef SXStringByJoiningSubStrings(SXVectorRef substring);
void SXStringJoinCString(SXStringRef string, const char * cstring);

SXStringRef SXStringByReplacingOccurrencesOfString(SXStringRef string, SXStringRef target, SXStringRef replacement);
SXStringRef SXStringByReplacingOccurrencesOfCString(SXStringRef string, const char * ctarget, const char * creplacement);

void SXStringRemoveOcurrenceOfCString(SXStringRef string, const char * remove, size_t len);



void SXStringRemovingRange(SXStringRef string, sx_range_t range);

SXError SXRetainString(SXStringRef);
SXError SXReleaseString(SXStringRef);
SXError SXFreeString(SXStringRef);

#endif /* SXString_h */
