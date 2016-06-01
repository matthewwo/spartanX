//
//  SXString.c
//  cspartanX
//
//  Created by Yuji on 5/29/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

#include "SXString.h"

SXStringRef SXCreateStringWithCString(const char * cstr, size_t length) {
    
    size_t len      = strnlen(cstr, length);
    
    SXStringRef str = (SXStringRef)sx_calloc(1, sizeof(sx_string_t));
    
    str->chars      = (char *)sx_calloc(len + 1, sizeof(char));
    memcpy(str->chars, cstr, len + 1);
    
    str->length     = strnlen(str->chars, len);
    str->ref_count  = 1;
    return str;
}

SXStringRef SXStringGetSubstringInRange(SXStringRef o_str, sx_range_t range)
{
    char * buf = (char *)sx_calloc(range.len + 1, sizeof(char));
    memset(buf, 0, sizeof(char) * range.len + 1);
    memcpy(buf, o_str->chars + range.loca, range.len);
    
    SXStringRef s =  SXCreateStringWithCString(buf, range.len);
    free(buf);
    return s;
}

void SXStringDealloc(void * s) {
    SXReleaseString(*(SXStringRef *)s);
}

size_t SXStringSubStringLocation(SXStringRef o_str, const char * trim, size_t len, size_t n) {
    int locas[o_str->length]; // max possible substrings trimed
    int j = 0;
    
    memset(locas, 0, sizeof(int) * o_str->length);
    
    for (int i = 0; i < o_str->length; ++i)
        if (!strncmp(o_str->chars + i, trim, len)) {
            if (j == n) return i;
            else ++j;
        }
    return -1;
}

SXVectorRef SXStringSubCStringLocations(SXStringRef o_str, const char *trim, size_t len) {
    int locas[o_str->length]; // max possible substrings trimed
    int n = 0;
    
    memset(locas, 0, sizeof(int) * o_str->length);
    
    for (int i = 0; i < o_str->length; ++i)
        if (!strncmp(o_str->chars + i, trim, len))
            locas[n++] = i;
    
    if (n == 0) return NULL;
    
    SXVectorRef v_locas = SXCreateVectorWithArray(locas, sizeof(int), n, NULL);
    
    return v_locas;
}

void SXStringJoinCString(SXStringRef string, const char * cstring) {
    string->chars = sx_realloc(string->chars, string->length + strlen(cstring) + 1);
    memcpy(string->chars + string->length, cstring, strlen(cstring));
    
}

SXStringRef SXStringByJoiningSubStrings(SXVectorRef substring) {
    SXStringRef first = *(SXStringRef *)SXVectorObjectPtrAtIndex(substring, 0);
    SXRetainString(first);
    for (int i = 1; i < substring->count; ++i) {
        SXStringRef o = *(SXStringRef *)SXVectorObjectPtrAtIndex(substring, i);
        SXStringJoinCString(first, o->chars);
    }
    return first;
}

SXVectorRef SXStringSubStringLocations(SXStringRef o_str, SXStringRef trim)
{
    return SXStringSubCStringLocations(o_str, trim->chars, trim->length);
}

SXVectorRef SXStringSubStringsTrimmedByString(SXStringRef target, SXStringRef trimstr)
{
    SXVectorRef loca = SXStringSubStringLocations(target, trimstr);
    
    SXVectorRef ret = SXCreateVector(sizeof(SXStringRef), loca->count + 1, SXStringDealloc);
    
    sx_range_t range;

    for (int i = 0; i <= loca->count; ++i) {
        if (i == 0) {
            range.loca = 0;
            range.len = *(int *)SXVectorObjectPtrAtIndex(loca, i);
        } else if ( i == loca->count) {
            range.len = (int)target->length - (*(int *)SXVectorObjectPtrAtIndex(loca, i - 1) + (int)trimstr->length);
            range.loca = (int)target->length - range.len;
        } else {
            range.loca = *(int *)SXVectorObjectPtrAtIndex(loca, i - 1) + (int)trimstr->length;
            range.len = *(int *)SXVectorObjectPtrAtIndex(loca, i) - *(int *)SXVectorObjectPtrAtIndex(loca, i - 1) - (int)trimstr->length;
        }
        
        SXStringRef sstr = SXStringGetSubstringInRange(target, range);
        memcpy((char *)ret->mem_loca + ret->osize * i, &sstr, sizeof(SXStringRef));
        ++ret->count;
    }
    
    SXReleaseVector(loca);
    return ret;
}


SXVectorRef SXStringSubStringsTrimmedByCString(SXStringRef target, const char * trimcstr, size_t trimstr_len)
{
    SXStringRef s = SXCreateStringWithCString(trimcstr, trimstr_len);
    SXVectorRef v = SXStringSubStringsTrimmedByString(target, s);
    SXReleaseString(s);
    return v;
}

void SXStringReplacingOccurrencesOfString(SXStringRef string, SXStringRef target, SXStringRef replacement)
{
    size_t len = string->length + replacement->length * string->length + 1;
//    char buf[len]; // maximum possible characters
    char *buf = sx_calloc(len, sizeof(char));
    memset(buf, 0, len);
    SXVectorRef positions = SXStringSubStringLocations(string, target);
    
    int delta = (int)replacement->length - (int)target->length;

    if (positions->count == 0) return;
    
    for (int i = 0, str_pos = 0, n = 0; i < string->length + delta * positions->count; ++i) {
        
        if (str_pos == *(int *)(SXVectorObjectPtrAtIndex(positions, n))) {
            ++n;
            memcpy(buf + i, replacement->chars, replacement->length);
            i += replacement->length - 1;
            str_pos += target->length;
            
        } else {
            sprintf((buf + i), "%c", *(string->chars + str_pos));
            ++str_pos;
        }
        
    }
    
    SXReleaseVector(positions);
    
    string->chars = (char *)sx_realloc(string->chars, len);
    
    memset(string->chars, 0, len);
    memcpy(string->chars, buf, len);
    sx_free(buf);
    
    string->length = strlen(string->chars);
}

void SXStringReplacingOccurrencesOfCString(SXStringRef string, const char * ctarget, const char * creplacement)
{
    SXStringRef target = SXCreateStringWithCString(ctarget, strlen(ctarget));
    SXStringRef replacement = SXCreateStringWithCString(creplacement, strlen(creplacement));
    SXStringByReplacingOccurrencesOfString(string, target, replacement);
    SXReleaseString(target);
    SXReleaseString(replacement);
}

bool SXStringContainsCString(SXStringRef string, const char * cstr) {
    if (string->length > strlen(cstr)) return false;
    size_t t = string->length - strlen(cstr) + 1;
    for (size_t i = 0 ; i < t ; ++i)
    if (!strncmp(string->chars, cstr, strlen(cstr)))
        return true;
    return false;
}

SXStringRef SXStringByReplacingOccurrencesOfString(SXStringRef string, SXStringRef target, SXStringRef replacement)
{
    size_t len = string->length + replacement->length * string->length + 1;
    char buf[len]; // maximum possible characters
    memset(buf, 0, len);
    SXVectorRef positions = SXStringSubStringLocations(string, target);
    
    int delta = (int)replacement->length - (int)target->length;
    
    if (positions->count == 0) return SXCreateStringWithCString(string->chars, string->length);
    
    for (int i = 0, str_pos = 0, n = 0; i < string->length + delta * positions->count; ++i) {
        
        if (str_pos == *(int *)(SXVectorObjectPtrAtIndex(positions, n))) {
            ++n;
            memcpy(buf + i, replacement->chars, replacement->length);
            i += replacement->length - 1;
            str_pos += target->length;
            
        } else {
            sprintf((buf + i), "%c", *(string->chars + str_pos));
            ++str_pos;
        }
    }
    
    SXReleaseVector(positions);
    return SXCreateStringWithCString(buf, len);
}


SXStringRef SXStringByReplacingOccurrencesOfCString(SXStringRef string, const char * ctarget, const char * creplacement)
{
    SXStringRef target = SXCreateStringWithCString(ctarget, strlen(ctarget));
    SXStringRef replacement = SXCreateStringWithCString(creplacement, strlen(creplacement));
    
    SXStringRef str = SXStringByReplacingOccurrencesOfString(string, target, replacement);

    SXReleaseString(target);
    SXReleaseString(replacement);
    return str;
}

void SXStringRemovingRange(SXStringRef string, sx_range_t range) {
    memset(string->chars + range.loca, 0, range.len);
    memcpy(string->chars + range.loca, string->chars + range.loca + range.len, string->length - range.loca - range.len);
    
    memset((char *)string->chars + string->length - range.len, 0, range.len);
    
    string->length = strnlen(string->chars, string->length);
    string->chars = (char *)sx_realloc(string->chars, string->length + 1);
}

void SXStringRemoveOcurrenceOfCString(SXStringRef string, const char * remove, size_t len) {
    
    for (int i = 0; i < string->length; ++i)
        if (!strncmp(string->chars + i, remove, len)) {
            sx_range_t range = {i, strnlen(remove, len)};
            SXStringRemovingRange(string, range);
        }
}

void SXStringRemoveOcurrencesOfCStrings(SXStringRef string, const char * remove, size_t len) {
    size_t cl = string->length;
    do {
        SXStringRemoveOcurrenceOfCString(string, remove, len);
        cl = string->length;
    } while (string->length != cl);
}

SXError SXRetainString(SXStringRef string) {
    if (string->ref_count == sx_weak_object)
        return SX_SUCCESS;
    ++string->ref_count;
    return SX_SUCCESS;
}
SXError SXReleaseString(SXStringRef string) {
    if (string->ref_count == sx_weak_object)
        return SX_SUCCESS;
    --string->ref_count;
    if (string->ref_count == 0)
        return SXFreeString(string);
    return SX_SUCCESS;
}

SXError SXFreeString(SXStringRef string) {
    sx_free(string);
    sx_free(string->chars);
    return SX_SUCCESS;
}
