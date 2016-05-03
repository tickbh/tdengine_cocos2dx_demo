#ifndef _TD_MACRO_H_
#define _TD_MACRO_H_

#include <stdio.h>

#ifdef WIN32
typedef __int64            i64;
typedef __int32            i32;
typedef __int16            i16;
typedef __int8             i8;
typedef unsigned __int64   u64;
typedef unsigned __int32   u32;
typedef unsigned __int16   u16;
typedef unsigned __int8    u8;
#else
typedef __int64_t   i64;
typedef __int32_t   i32;
typedef __int16_t   i16;
typedef __int8_t    i8;
typedef __uint64_t  u64;
typedef __uint32_t  u32;
typedef __uint16_t  u16;
typedef __uint8_t   u8;
#endif

#endif