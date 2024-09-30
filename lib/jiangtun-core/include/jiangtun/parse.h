#ifndef JIANGTUN_PARSE_H
#define JIANGTUN_PARSE_H

#include "compat.h"
#include "error.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif

jiangtun_error_t jiangtun_nxmc2_parse(const jiangtun_uint8_t *, size_t,
                                      jiangtun_report_t *, jiangtun_bool_t *);

typedef struct jiangtun_pokecon_previous_state_t {
    struct {
        jiangtun_uint8_t x;
        jiangtun_uint8_t y;
    } left;
    struct {
        jiangtun_uint8_t x;
        jiangtun_uint8_t y;
    } right;
} jiangtun_pokecon_previous_state_t;

jiangtun_error_t
jiangtun_pokecon_previous_state_init(jiangtun_pokecon_previous_state_t *);
jiangtun_error_t jiangtun_pokecon_parse(const jiangtun_uint8_t *, size_t,
                                        jiangtun_report_t *, jiangtun_bool_t *,
                                        jiangtun_pokecon_previous_state_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_PARSE_H */