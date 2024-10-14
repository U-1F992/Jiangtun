#include <jiangtun/command.h>

#include "internal.h"

#include <assert.h>
#include <stdio.h>

#define POKECON_HAT_UP 0
#define POKECON_HAT_UPRIGHT 1
#define POKECON_HAT_RIGHT 2
#define POKECON_HAT_DOWNRIGHT 3
#define POKECON_HAT_DOWN 4
#define POKECON_HAT_DOWNLEFT 5
#define POKECON_HAT_LEFT 6
#define POKECON_HAT_UPLEFT 7
#define POKECON_HAT_NEUTRAL 8

static void patch_btns(jiangtun_report_mode3_t *report,
                       jiangtun_uint16_t btns) {
    assert(report != NULL);

    report->y = (btns >> 2) & 1;
    report->b = (btns >> 3) & 1;
    report->a = (btns >> 4) & 1;
    report->x = (btns >> 5) & 1;
    report->l = (btns >> 6) & 1;
    report->r = (btns >> 7) & 1;
    /* zl = (btns >> 8) & 1; */
    report->z = (btns >> 9) & 1;
    /* minus = (btns >> 10) & 1; */
    report->start = (btns >> 11) & 1;
    /* l_click = (btns >> 12) & 1; */
    /* r_click = (btns >> 13) & 1; */
    report->reset = (btns >> 14) & 1;
    /* capture = (btns >> 15) & 1; */
}

static void patch_hat(jiangtun_report_mode3_t *report, jiangtun_uint16_t hat) {
    assert(report != NULL);
    assert(hat <= POKECON_HAT_NEUTRAL);

    switch (hat) {
    case POKECON_HAT_UP:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case POKECON_HAT_UPRIGHT:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case POKECON_HAT_RIGHT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case POKECON_HAT_DOWNRIGHT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case POKECON_HAT_DOWN:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case POKECON_HAT_DOWNLEFT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case POKECON_HAT_LEFT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case POKECON_HAT_UPLEFT:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case POKECON_HAT_NEUTRAL:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    default:
        assert(JIANGTUN_FALSE);
        break;
    }
}

static void patch_ls(jiangtun_report_mode3_t *report, jiangtun_uint16_t lx,
                     jiangtun_uint16_t ly) {
    assert(report != NULL);
    assert(lx <= JIANGTUN_UINT8_MAX);
    assert(ly <= JIANGTUN_UINT8_MAX);

    report->xAxis = (jiangtun_uint8_t)lx;
    report->yAxis = (jiangtun_uint8_t)(JIANGTUN_UINT8_MAX - ly);
}

static void patch_rs(jiangtun_report_mode3_t *report, jiangtun_uint16_t rx,
                     jiangtun_uint16_t ry) {
    assert(report != NULL);
    assert(rx <= JIANGTUN_UINT8_MAX);
    assert(ry <= JIANGTUN_UINT8_MAX);

    report->cxAxis = (jiangtun_uint8_t)rx;
    report->cyAxis = (jiangtun_uint8_t)(JIANGTUN_UINT8_MAX - ry);
}

static void copy_buf_to_str(const jiangtun_uint8_t *buffer, size_t length,
                            char *string) {
    size_t i = 0;
    assert(buffer != NULL);
    assert(length != 0);
    assert(string != NULL);
    /* assert(sizeof(string) >= length); */

    for (i = 0; i < length; i++) {
        string[i] = (char)buffer[i];
    }
    string[i++] = '\0';
}

static void remove_0x_cr_lf(char *string) {
    char *src = string;
    char *dst = string;
    assert(string != NULL);

    while (*src != '\0') {
        if (*src == '0' && *(src + 1) == 'x') {
            src += 2;
        } else if (*src == '\r' || *src == '\n') {
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

static jiangtun_bool_t patch_endrn(const jiangtun_uint8_t *buffer,
                                   size_t length, void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    assert(buffer != NULL);
    assert(length == 4 || (length == 5 && buffer[3] == '\r'));
    assert(buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd' &&
           buffer[length - 1] == '\n');
    assert(report != NULL);

    jiangtun_report_init(report);

    return JIANGTUN_TRUE;
}

static jiangtun_bool_t patch_0xhhhh_hrn(const jiangtun_uint8_t *buffer,
                                        size_t length, void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    char string[sizeof("0x0000 0\r\n")];
    jiangtun_uint16_t btns = 0;
    jiangtun_uint16_t hat = POKECON_HAT_NEUTRAL;
    assert(buffer != NULL);
    assert(sizeof("0 0\n") - 1 <= length && length <= sizeof(string) - 1);
    assert(report != NULL);

    /*
     * Use uint16_t and "%hx" instead of uint8_t and "%hhx" because Newlib-nano
     * does not support C99 notation.
     * ref: https://ja-support.renesas.com/knowledgeBase/20441082
     *
     * The "0x" prefix must be removed. This might be another limitation of
     * Newlib-nano.
     */
    copy_buf_to_str(buffer, length, string);
    remove_0x_cr_lf(string);
    sscanf(string, "%hx %hx", &btns, &hat);
    if (hat > POKECON_HAT_NEUTRAL) {
        assert(hat <= POKECON_HAT_NEUTRAL);
        return JIANGTUN_FALSE;
    }

    patch_btns(report, btns);
    patch_hat(report, hat);

    return JIANGTUN_TRUE;
}

static jiangtun_bool_t
patch_0xhhhh_h_0xhh_0xhhrn(const jiangtun_uint8_t *buffer, size_t length,
                           void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    char string[sizeof("0x0001 0 0x00 0x00\r\n")];
    jiangtun_uint16_t btns = 0;
    jiangtun_uint16_t hat = POKECON_HAT_NEUTRAL;
    jiangtun_uint16_t x = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t y = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_bool_t update_r = JIANGTUN_FALSE;
    jiangtun_bool_t update_l = JIANGTUN_FALSE;
    assert(buffer != NULL);
    assert(sizeof("1 0 0 0\n") - 1 <= length && length <= sizeof(string) - 1);
    assert(report != NULL);

    /* Refer to `patch_0xhhhh_hrn` for these restrictions */
    copy_buf_to_str(buffer, length, string);
    remove_0x_cr_lf(string);
    sscanf(string, "%hx %hx %hx %hx", &btns, &hat, &x, &y);
    if (hat > POKECON_HAT_NEUTRAL || x > JIANGTUN_UINT8_MAX ||
        y > JIANGTUN_UINT8_MAX) {
        assert(hat <= POKECON_HAT_NEUTRAL);
        assert(x <= JIANGTUN_UINT8_MAX);
        assert(y <= JIANGTUN_UINT8_MAX);
        return JIANGTUN_FALSE;
    }

    update_r = (btns >> 0) & 1;
    update_l = (btns >> 1) & 1;
    if ((!update_r && !update_l) ||
        /* When releasing the stick input generated by "Use LStick Mouse",
           unexpected input "3 8 80 80\n" occurs. */
        (update_r && update_l && btns != 3)) {
        return JIANGTUN_FALSE;
    }

    patch_btns(report, btns);
    patch_hat(report, hat);
    if (update_r) {
        patch_rs(report, x, y);
    }
    if (update_l) {
        patch_ls(report, x, y);
    }

    return JIANGTUN_TRUE;
}

static jiangtun_bool_t
patch_0xhhhh_h_0xhh_0xhh_0xhh_0xhhrn(const jiangtun_uint8_t *buffer,
                                     size_t length, void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    char string[sizeof("0x0003 0 0x00 0x00 0x00 0x00\r\n")];
    jiangtun_uint16_t btns = 0;
    jiangtun_uint16_t hat = POKECON_HAT_NEUTRAL;
    jiangtun_uint16_t lx = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t ly = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t rx = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_uint16_t ry = JIANGTUN_REPORT_STICK_NEUTRAL;
    jiangtun_bool_t update_r = JIANGTUN_FALSE;
    jiangtun_bool_t update_l = JIANGTUN_FALSE;
    assert(buffer != NULL);
    assert(sizeof("3 0 0 0 0 0\n") - 1 <= length &&
           length <= sizeof(string) - 1);
    assert(report != NULL);

    /* Refer to `patch_0xhhhh_hrn` for these restrictions */
    copy_buf_to_str(buffer, length, string);
    remove_0x_cr_lf(string);
    sscanf(string, "%hx %hx %hx %hx %hx %hx", &btns, &hat, &lx, &ly, &rx, &ry);
    if (hat > POKECON_HAT_NEUTRAL || lx > JIANGTUN_UINT8_MAX ||
        ly > JIANGTUN_UINT8_MAX || rx > JIANGTUN_UINT8_MAX ||
        ry > JIANGTUN_UINT8_MAX || !(update_r = (btns >> 0) & 1) ||
        !(update_l = (btns >> 1) & 1)) {
        assert(hat <= POKECON_HAT_NEUTRAL);
        assert(lx <= JIANGTUN_UINT8_MAX);
        assert(ly <= JIANGTUN_UINT8_MAX);
        assert(rx <= JIANGTUN_UINT8_MAX);
        assert(ry <= JIANGTUN_UINT8_MAX);
        return JIANGTUN_FALSE;
    }

    patch_btns(report, btns);
    patch_hat(report, hat);
    patch_rs(report, rx, ry);
    patch_ls(report, lx, ly);

    return JIANGTUN_TRUE;
}

static jiangtun_bool_t is_hex_digit(jiangtun_uint8_t c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f');
}

static jiangtun_bool_t is_digit_0_to_8(jiangtun_uint8_t c) {
    return '0' <= c && c <= '8';
}

static void
transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhhr(jiangtun_command_t *command,
                                         jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL,
                           c == '\n' ? patch_0xhhhh_h_0xhh_0xhh_0xhh_0xhhrn
                                     : NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhh(jiangtun_command_t *command,
                                                    jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, c == '\r' ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhhr : NULL,
        c == '\n' ? patch_0xhhhh_h_0xhh_0xhh_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh_0xh(jiangtun_command_t *command,
                                                   jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c,
        is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhh
        : c == '\r'     ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhhr
                        : NULL,
        c == '\n' ? patch_0xhhhh_h_0xhh_0xhh_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh_0x(jiangtun_command_t *command,
                                                  jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c,
        is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xh : NULL, NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh_0(jiangtun_command_t *command,
                                                 jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c,
        c == 'x'          ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0x
        : is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhh
        : c == '\r'       ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xhhr
                          : NULL,
        c == '\n' ? patch_0xhhhh_h_0xhh_0xhh_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh_(jiangtun_command_t *command,
                                                jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == '0' ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0
                           : is_hex_digit(c)
                               ? transition_0xhhhh_h_0xhh_0xhh_0xhh_0xh
                               : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xhh(jiangtun_command_t *command,
                                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, c == ' ' ? transition_0xhhhh_h_0xhh_0xhh_0xhh_ : NULL,
        NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0xh(jiangtun_command_t *command,
                                              jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xhh
                           : c == ' '      ? transition_0xhhhh_h_0xhh_0xhh_0xhh_
                                           : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0x(jiangtun_command_t *command,
                                             jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xh : NULL,
        NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_0(jiangtun_command_t *command,
                                            jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == 'x' ? transition_0xhhhh_h_0xhh_0xhh_0x
                           : is_hex_digit(c)
                               ? transition_0xhhhh_h_0xhh_0xhh_0xhh
                           : c == ' ' ? transition_0xhhhh_h_0xhh_0xhh_0xhh_
                                      : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh_(jiangtun_command_t *command,
                                           jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == '0'          ? transition_0xhhhh_h_0xhh_0xhh_0
                           : is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh_0xh
                                             : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0xhh_0xhhr(jiangtun_command_t *command,
                                           jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL,
                           c == '\n' ? patch_0xhhhh_h_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0xhh(jiangtun_command_t *command,
                                          jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == ' '    ? transition_0xhhhh_h_0xhh_0xhh_
                           : c == '\r' ? transition_0xhhhh_h_0xhh_0xhhr
                                       : NULL,
                           c == '\n' ? patch_0xhhhh_h_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0xh(jiangtun_command_t *command,
                                         jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh
                           : c == ' '      ? transition_0xhhhh_h_0xhh_0xhh_
                           : c == '\r'     ? transition_0xhhhh_h_0xhh_0xhhr
                                           : NULL,
                           c == '\n' ? patch_0xhhhh_h_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_0x(jiangtun_command_t *command,
                                        jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xh : NULL,
        NULL);
}

static void transition_0xhhhh_h_0xhh_0(jiangtun_command_t *command,
                                       jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == 'x'          ? transition_0xhhhh_h_0xhh_0x
                           : is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xhh
                           : c == ' '        ? transition_0xhhhh_h_0xhh_0xhh_
                           : c == '\r'       ? transition_0xhhhh_h_0xhh_0xhhr
                                             : NULL,
                           c == '\n' ? patch_0xhhhh_h_0xhh_0xhhrn : NULL);
}

static void transition_0xhhhh_h_0xhh_(jiangtun_command_t *command,
                                      jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == '0'          ? transition_0xhhhh_h_0xhh_0
                           : is_hex_digit(c) ? transition_0xhhhh_h_0xhh_0xh
                                             : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0xhh(jiangtun_command_t *command,
                                     jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == ' ' ? transition_0xhhhh_h_0xhh_ : NULL, NULL);
}

static void transition_0xhhhh_h_0xh(jiangtun_command_t *command,
                                    jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhhhh_h_0xhh
                           : c == ' '      ? transition_0xhhhh_h_0xhh_
                                           : NULL,
                           NULL);
}

static void transition_0xhhhh_h_0x(jiangtun_command_t *command,
                                   jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, is_hex_digit(c) ? transition_0xhhhh_h_0xh : NULL, NULL);
}

static void transition_0xhhhh_h_0(jiangtun_command_t *command,
                                  jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == 'x'          ? transition_0xhhhh_h_0x
                           : is_hex_digit(c) ? transition_0xhhhh_h_0xhh
                           : c == ' '        ? transition_0xhhhh_h_0xhh_
                                             : NULL,
                           NULL);
}

static void transition_0xhhhh_h_(jiangtun_command_t *command,
                                 jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == '0'          ? transition_0xhhhh_h_0
                           : is_hex_digit(c) ? transition_0xhhhh_h_0xh
                                             : NULL,
                           NULL);
}

static void transition_0xhhhh_hr(jiangtun_command_t *command,
                                 jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL,
                           c == '\n' ? patch_0xhhhh_hrn : NULL);
}

static void transition_0xhhhh_h(jiangtun_command_t *command,
                                jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == ' '    ? transition_0xhhhh_h_
                           : c == '\r' ? transition_0xhhhh_hr
                                       : NULL,
                           c == '\n' ? patch_0xhhhh_hrn : NULL);
}

static void transition_0xhhhh_(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(
        command, c, is_digit_0_to_8(c) ? transition_0xhhhh_h : NULL, NULL);
}

static void transition_0xhhhh(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, c == ' ' ? transition_0xhhhh_ : NULL,
                           NULL);
}

static void transition_0xhhh(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhhhh
                           : c == ' '      ? transition_0xhhhh_
                                           : NULL,
                           NULL);
}

static void transition_0xhh(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhhh
                           : c == ' '      ? transition_0xhhhh_
                                           : NULL,
                           NULL);
}

static void transition_0xh(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           is_hex_digit(c) ? transition_0xhh
                           : c == ' '      ? transition_0xhhhh_
                                           : NULL,
                           NULL);
}

static void transition_0x(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, is_hex_digit(c) ? transition_0xh : NULL,
                           NULL);
}

static void transition_0(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == 'x'          ? transition_0x
                           : is_hex_digit(c) ? transition_0xhh
                           : c == ' '        ? transition_0xhhhh_
                                             : NULL,
                           NULL);
}

static void transition_endr(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL, c == '\n' ? patch_endrn : NULL);
}

static void transition_end(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, c == '\r' ? transition_endr : NULL,
                           c == '\n' ? patch_endrn : NULL);
}

static void transition_en(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, c == 'd' ? transition_end : NULL, NULL);
}

static void transition_e(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == 'n'          ? transition_en
                           : is_hex_digit(c) ? transition_0xhh
                           : c == ' '        ? transition_0xhhhh_
                                             : NULL,
                           NULL);
}

static void transition_initial(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == '0'          ? transition_0
                           : c == 'e'        ? transition_e
                           : is_hex_digit(c) ? transition_0xh
                                             : NULL,
                           NULL);
}

void jiangtun_pokecon_init(jiangtun_pokecon_command_t *command) {
    assert(command != NULL);

    command->base.buffer = command->buffer;
    command->base.length = 0;

    jiangtun_command_init(&(command->base), transition_initial, NULL);
}