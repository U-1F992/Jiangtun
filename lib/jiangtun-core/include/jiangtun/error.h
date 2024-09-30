#ifndef JIANGTUN_ERROR_H
#define JIANGTUN_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum jiangtun_error_t {
    JIANGTUN_SUCCESS,
    JIANGTUN_ERROR_NULL_POINTER,
    JIANGTUN_ERROR_INVALID_ARGUMENT,
    JIANGTUN_ERROR_PARSE_FAILURE,
    JIANGTUN_ERROR_INTERNAL
} jiangtun_error_t;

const char *jiangtun_error_stringify(jiangtun_error_t);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_ERROR_H */