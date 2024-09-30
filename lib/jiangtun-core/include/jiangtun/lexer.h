#ifndef JIANGTUN_LEXER_H
#define JIANGTUN_LEXER_H

#include "compat.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum jiangtun_yy_match_t {
    JIANGTUN_YY_NO_MATCH,
    JIANGTUN_YY_MATCH_NXMC2,
    JIANGTUN_YY_MATCH_POKECON,
    JIANGTUN_YY_MATCH_ORCA
} jiangtun_yy_match_t;

typedef struct jiangtun_yy_extra_t {
    jiangtun_bool_t partial;
    jiangtun_yy_match_t return_;
} jiangtun_yy_extra_t;

typedef struct jiangtun_lexer_t {
    void *scanner;
    jiangtun_yy_extra_t extra;
} jiangtun_lexer_t;

typedef enum jiangtun_lexer_result_t {
    JIANGTUN_LEXER_NO_MATCH,
    JIANGTUN_LEXER_MATCH_PARTIAL,
    JIANGTUN_LEXER_MATCH_NXMC2,
    JIANGTUN_LEXER_MATCH_POKECON,
    JIANGTUN_LEXER_MATCH_ORCA
} jiangtun_lexer_result_t;

const char *jiangtun_lexer_result_stringify(jiangtun_lexer_result_t);

jiangtun_error_t jiangtun_lexer_init(jiangtun_lexer_t *);
jiangtun_error_t jiangtun_lexer_deinit(jiangtun_lexer_t *);
jiangtun_error_t jiangtun_lex(jiangtun_lexer_t *, const jiangtun_uint8_t *,
                              size_t, jiangtun_lexer_result_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_LEXER_H */