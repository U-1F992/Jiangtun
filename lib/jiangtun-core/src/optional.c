#include <jiangtun/optional.h>

#include <assert.h>

void jiangtun_optional_uint8_set(jiangtun_optional_uint8_t *optional,
                                 jiangtun_uint8_t value) {
    assert(optional != NULL);

    optional->value = value;
    optional->pointer = &(optional->value);
}

jiangtun_bool_t jiangtun_optional_uint8_get(jiangtun_optional_uint8_t *optional,
                                            jiangtun_uint8_t *value) {
    assert(optional != NULL);
    assert(value != NULL);

    if (optional->pointer == NULL) {
        return JIANGTUN_FALSE;
    }
    *value = optional->value;
    return JIANGTUN_TRUE;
}

void jiangtun_optional_uint8_clear(jiangtun_optional_uint8_t *optional) {
    assert(optional != NULL);

    optional->pointer = NULL;
}

void jiangtun_optional_uint32_set(jiangtun_optional_uint32_t *optional,
                                  jiangtun_uint32_t value) {
    assert(optional != NULL);

    optional->value = value;
    optional->pointer = &(optional->value);
}

jiangtun_bool_t
jiangtun_optional_uint32_get(jiangtun_optional_uint32_t *optional,
                             jiangtun_uint32_t *value) {
    assert(optional != NULL);
    assert(value != NULL);

    if (optional->pointer == NULL) {
        return JIANGTUN_FALSE;
    }
    *value = optional->value;
    return JIANGTUN_TRUE;
}

void jiangtun_optional_uint32_clear(jiangtun_optional_uint32_t *optional) {
    assert(optional != NULL);

    optional->pointer = NULL;
}