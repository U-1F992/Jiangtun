#ifndef JIANGTUN_COMPAT_H
#define JIANGTUN_COMPAT_H

#include <limits.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum jiangtun_bool_t { JIANGTUN_FALSE, JIANGTUN_TRUE } jiangtun_bool_t;

typedef unsigned char jiangtun_uint8_t;

#if USHRT_MAX == 0xFFFF
typedef unsigned short jiangtun_uint16_t;
#elif UINT_MAX == 0xFFFF
typedef unsigned int jiangtun_uint16_t;
#elif ULONG_MAX == 0xFFFF
typedef unsigned long jiangtun_uint16_t;
#else
#error "No suitable type for jiangtun_uint16_t"
#endif

#if UINT_MAX == 0xFFFFFFFF
typedef unsigned int jiangtun_uint32_t;
#elif ULONG_MAX == 0xFFFFFFFF
typedef unsigned long jiangtun_uint32_t;
#elif ULLONG_MAX == 0xFFFFFFFF
typedef unsigned long long jiangtun_uint32_t;
#else
#error "No suitable type for jiangtun_uint32_t"
#endif

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_COMPAT_H */