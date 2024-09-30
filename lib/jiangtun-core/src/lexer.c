#include <jiangtun/lexer.h>

/* clang-format off */
#include "lex.yy.extern.h"
#include "lex.yy.h"
/* clang-format on */

jiangtun_error_t jiangtun_lexer_init(jiangtun_lexer_t *lexer) {
    if (lexer == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    }

    lexer->extra.partial = JIANGTUN_FALSE;
    if (yylex_init_extra(&(lexer->extra), (yyscan_t *)&(lexer->scanner)) != 0) {
        return JIANGTUN_ERROR_INTERNAL;
    }

    return JIANGTUN_SUCCESS;
}

jiangtun_error_t jiangtun_lexer_deinit(jiangtun_lexer_t *lexer) {
    if (lexer == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    }

    yylex_destroy((yyscan_t)(lexer->scanner));

    return JIANGTUN_SUCCESS;
}

jiangtun_error_t jiangtun_lex(jiangtun_lexer_t *lexer,
                              const jiangtun_uint8_t *buffer,
                              size_t buffer_length,
                              jiangtun_lexer_result_t *out_result) {
    jiangtun_error_t err = JIANGTUN_SUCCESS;
    jiangtun_lexer_result_t result = JIANGTUN_LEXER_NO_MATCH;
    YY_BUFFER_STATE state = NULL;

    if (lexer == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    } else if (buffer == NULL || buffer_length == 0) {
        if (out_result != NULL) {
            *out_result = JIANGTUN_LEXER_NO_MATCH;
        }
        return JIANGTUN_SUCCESS;
    }

    state = yy_scan_bytes((char *)buffer, buffer_length,
                          (yyscan_t)(lexer->scanner));
    if (state == NULL) {
        return JIANGTUN_ERROR_INTERNAL;
    }
    yy_switch_to_buffer(state, (yyscan_t)(lexer->scanner));

    lexer->extra.partial = JIANGTUN_FALSE;
    lexer->extra.return_ = JIANGTUN_YY_NO_MATCH;
    yylex((yyscan_t)(lexer->scanner));
    switch (lexer->extra.return_) {
    case JIANGTUN_YY_NO_MATCH:
        result = lexer->extra.partial ? JIANGTUN_LEXER_MATCH_PARTIAL
                                      : JIANGTUN_LEXER_NO_MATCH;
        break;

    case JIANGTUN_YY_MATCH_NXMC2:
        result = JIANGTUN_LEXER_MATCH_NXMC2;
        break;

    case JIANGTUN_YY_MATCH_POKECON:
        result = JIANGTUN_LEXER_MATCH_POKECON;
        break;

    case JIANGTUN_YY_MATCH_ORCA:
        result = JIANGTUN_LEXER_MATCH_ORCA;
        break;

    default:
        err = JIANGTUN_ERROR_INTERNAL;
        break;
    }

    yy_delete_buffer(state, (yyscan_t)(lexer->scanner));

    if (out_result != NULL) {
        *out_result = result;
    }

    return err;
}

const char *jiangtun_lexer_result_stringify(jiangtun_lexer_result_t result) {
    switch (result) {
    case JIANGTUN_LEXER_NO_MATCH:
        return "JIANGTUN_LEXER_NO_MATCH";

    case JIANGTUN_LEXER_MATCH_PARTIAL:
        return "JIANGTUN_LEXER_MATCH_PARTIAL";

    case JIANGTUN_LEXER_MATCH_NXMC2:
        return "JIANGTUN_LEXER_MATCH_NXMC2";

    case JIANGTUN_LEXER_MATCH_POKECON:
        return "JIANGTUN_LEXER_MATCH_POKECON";

    case JIANGTUN_LEXER_MATCH_ORCA:
        return "JIANGTUN_LEXER_MATCH_ORCA";

    default:
        return "unknown";
    }
}