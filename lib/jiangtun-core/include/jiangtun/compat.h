#ifndef JIANGTUN_COMPAT_H
#define JIANGTUN_COMPAT_H

#include <limits.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char jiangtun_uint8_t;
#define JIANGTUN_UINT8_MAX UCHAR_MAX

#if USHRT_MAX == 0xFFFF
typedef unsigned short jiangtun_uint16_t;
#define JIANGTUN_UINT16_MAX USHRT_MAX
#elif UINT_MAX == 0xFFFF
typedef unsigned int jiangtun_uint16_t;
#define JIANGTUN_UINT16_MAX UINT_MAX
#elif ULONG_MAX == 0xFFFF
typedef unsigned long jiangtun_uint16_t;
#define JIANGTUN_UINT16_MAX ULONG_MAX
#else
#error "No suitable type for jiangtun_uint16_t"
#endif

#if UINT_MAX == 0xFFFFFFFF
typedef unsigned int jiangtun_uint32_t;
#define JIANGTUN_UINT32_MAX UINT_MAX
#elif ULONG_MAX == 0xFFFFFFFF
typedef unsigned long jiangtun_uint32_t;
#define JIANGTUN_UINT32_MAX ULONG_MAX
#elif ULLONG_MAX == 0xFFFFFFFF
typedef unsigned long long jiangtun_uint32_t;
#define JIANGTUN_UINT32_MAX ULLONG_MAX
#else
#error "No suitable type for jiangtun_uint32_t"
#endif

typedef jiangtun_uint8_t jiangtun_bool_t;
#define JIANGTUN_FALSE ((jiangtun_bool_t)0)
#define JIANGTUN_TRUE ((jiangtun_bool_t)1)

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_COMPAT_H */