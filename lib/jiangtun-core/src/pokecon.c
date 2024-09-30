#include <jiangtun/parse.h>

#include <string.h>

jiangtun_error_t
jiangtun_pokecon_previous_state_init(jiangtun_pokecon_previous_state_t *prev) {
    if (prev == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    }

    prev->left.x = JIANGTUN_REPORT_STICK_NEUTRAL;
    prev->left.y = JIANGTUN_REPORT_STICK_NEUTRAL;
    prev->right.x = JIANGTUN_REPORT_STICK_NEUTRAL;
    prev->right.y = JIANGTUN_REPORT_STICK_NEUTRAL;

    return JIANGTUN_SUCCESS;
}

static void remove_0x(char *str) {
    char *p = str;
    while ((p = strstr(p, "0x")) != NULL) {
        /* Remove "0x" by moving the rest of the string over it */
        memmove(p, p + 2, strlen(p + 2) + 1);
    }
}

/*
 * static const size_t MIN_VALID_LENGTH = sizeof("0 0\n");
 * static const size_t MAX_VALID_LENGTH = sizeof("0x0000 0 0x00 0x00 0x00
 * 0x00\r\n");
 */
#define MIN_VALID_LENGTH 5
#define MAX_VALID_LENGTH 31

jiangtun_error_t
jiangtun_pokecon_parse(const jiangtun_uint8_t *buffer, size_t buffer_length,
                       jiangtun_report_t *out_report,
                       jiangtun_bool_t *out_reset,
                       jiangtun_pokecon_previous_state_t *prev) {
    size_t i = 0;

    char str[MAX_VALID_LENGTH];
    size_t str_length = 0;
    int whitespace_count = 0;

    jiangtun_uint16_t btns = 0x0000;
    jiangtun_uint16_t hat = 8;
    jiangtun_uint16_t x_0 = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t y_0 = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t x_1 = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t y_1 = JIANGTUN_REPORT_STICK_NEUTRAL;

    jiangtun_bool_t update_rs = JIANGTUN_FALSE;
    jiangtun_bool_t update_ls = JIANGTUN_FALSE;

    if (buffer == NULL || prev == NULL) {
        return JIANGTUN_ERROR_INVALID_ARGUMENT;
    } else if (buffer_length < MIN_VALID_LENGTH - 1 ||
               MAX_VALID_LENGTH - 1 < buffer_length) {
        return JIANGTUN_ERROR_PARSE_FAILURE;
    }

    if ((buffer_length == 4 || buffer_length == 5) &&
        (buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd' &&
         (buffer[3] == '\n' || (buffer[3] == '\r' && buffer[4] == '\n')))) {

        if (out_report != NULL) {
            if (jiangtun_report_init(out_report) != JIANGTUN_SUCCESS) {
                return JIANGTUN_ERROR_INTERNAL;
            }
        }

        prev->left.x = JIANGTUN_REPORT_STICK_NEUTRAL;
        prev->left.y = JIANGTUN_REPORT_STICK_NEUTRAL;
        prev->right.x = JIANGTUN_REPORT_STICK_NEUTRAL;
        prev->right.y = JIANGTUN_REPORT_STICK_NEUTRAL;

        return JIANGTUN_SUCCESS;
    }

    /* Copy buffer into a new char[] */
    for (i = 0; i < buffer_length; i++) {
        switch (buffer[i]) {
        case '\r':
        case '\n':
            break;

        case ' ':
            whitespace_count++;
            /* fallthrough */
        default:
            str_length++;
            str[i] = buffer[i];
            break;
        }
    }
    str[str_length++] = '\0';
    /* 0x" must be removed. Maybe this is another Newlib-nano restriction. */
    remove_0x(str);

    /*
     * Use uint16_t and "%hx" instead of uint8_t and "%hhx",
     * as Newlib-nano do not support C99 notation.
     * https://ja-support.renesas.com/knowledgeBase/20441082
     */
    switch (whitespace_count) {
    case 1:
        sscanf(str, "%hx %hx", &btns, &hat);
        break;
    case 3:
        sscanf(str, "%hx %hx %hx %hx", &btns, &hat, &x_0, &y_0);
        break;
    case 5:
        sscanf(str, "%hx %hx %hx %hx %hx %hx", &btns, &hat, &x_0, &y_0, &x_1,
               &y_1);
        break;
    default:
        return JIANGTUN_ERROR_PARSE_FAILURE;
    }
    hat = hat % 9;
    x_0 = x_0 & 0xFF;
    y_0 = 0xFF - (y_0 & 0xFF);
    x_1 = x_1 & 0xFF;
    y_1 = 0xFF - (y_1 & 0xFF);

    update_rs = (jiangtun_bool_t)((btns >> 0) & 1);
    update_ls = (jiangtun_bool_t)((btns >> 1) & 1);

    if (out_report != NULL) {
        if (jiangtun_report_init(out_report) != JIANGTUN_SUCCESS) {
            return JIANGTUN_ERROR_INTERNAL;
        }

        out_report->mode3.y = (btns >> 2) & 1;
        out_report->mode3.b = (btns >> 3) & 1;
        out_report->mode3.a = (btns >> 4) & 1;
        out_report->mode3.x = (btns >> 5) & 1;
        out_report->mode3.l = (btns >> 6) & 1;
        out_report->mode3.r = (btns >> 7) & 1;
        /* zl = (btns >> 8) & 1; */
        out_report->mode3.z = (btns >> 9) & 1;
        /* minus = (btns >> 10) & 1; */
        out_report->mode3.start = (btns >> 11) & 1;
        /* l_click = (btns >> 12) & 1; */
        /* r_click = (btns >> 13) & 1; */
        /* home = (btns >> 14) & 1; --> reset */
        /* capture = (btns >> 15) & 1; */

        switch (hat) {
        case 0:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 1:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 2:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 3:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 0;
            break;
        case 4:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 0;
            break;
        case 5:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 1;
            break;
        case 6:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 1;
            break;
        case 7:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 1;
            break;
        case 8:
        default:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        }
    }

    if (update_ls) {
        prev->left.x = (jiangtun_uint8_t)x_0;
        prev->left.y = (jiangtun_uint8_t)y_0;
    }
    if (update_rs) {
        prev->right.x = (jiangtun_uint8_t)x_1;
        prev->right.y = (jiangtun_uint8_t)y_1;
    }
    if (out_report != NULL) {
        out_report->mode3.xAxis = prev->left.x;
        out_report->mode3.yAxis = prev->left.y;
        out_report->mode3.cxAxis = prev->right.x;
        out_report->mode3.cyAxis = prev->right.y;
    }

    if (out_reset != NULL) {
        *out_reset = (jiangtun_bool_t)((btns >> 14) & 1);
    }

    return JIANGTUN_SUCCESS;
}