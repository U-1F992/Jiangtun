#include <jiangtun/error.h>

const char *jiangtun_error_stringify(jiangtun_error_t err) {
    switch (err) {
    case JIANGTUN_SUCCESS:
        return "JIANGTUN_SUCCESS";

    case JIANGTUN_ERROR_NULL_POINTER:
        return "JIANGTUN_ERROR_NULL_POINTER";

    case JIANGTUN_ERROR_INVALID_ARGUMENT:
        return "JIANGTUN_ERROR_INVALID_ARGUMENT";

    case JIANGTUN_ERROR_PARSE_FAILURE:
        return "JIANGTUN_ERROR_PARSE_FAILURE";

    case JIANGTUN_ERROR_INTERNAL:
        return "JIANGTUN_ERROR_INTERNAL";

    default:
        return "unknown";
    }
}