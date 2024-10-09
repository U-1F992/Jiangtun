#ifndef JIANGTUN_OPTIONAL_H
#define JIANGTUN_OPTIONAL_H

#include "compat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jiangtun_optional_uint8_t {
    jiangtun_uint8_t value;
    jiangtun_uint8_t *pointer;
} jiangtun_optional_uint8_t;

void jiangtun_optional_uint8_set(jiangtun_optional_uint8_t *, jiangtun_uint8_t);
jiangtun_bool_t jiangtun_optional_uint8_get(jiangtun_optional_uint8_t *,
                                            jiangtun_uint8_t *);
void jiangtun_optional_uint8_clear(jiangtun_optional_uint8_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_OPTIONAL_H */