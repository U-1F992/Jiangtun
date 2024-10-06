#include <jiangtun/command.h>

#include "internal.h"

#include <assert.h>

#define NXMC2_HEADER 0xAB
#define NXMC2_HAT_UP 0
#define NXMC2_HAT_UPRIGHT 1
#define NXMC2_HAT_RIGHT 2
#define NXMC2_HAT_DOWNRIGHT 3
#define NXMC2_HAT_DOWN 4
#define NXMC2_HAT_DOWNLEFT 5
#define NXMC2_HAT_LEFT 6
#define NXMC2_HAT_UPLEFT 7
#define NXMC2_HAT_NEUTRAL 8

static jiangtun_bool_t patch(const jiangtun_uint8_t *buffer, size_t length,
                             void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    jiangtun_uint8_t btn0 = 0;
    jiangtun_uint8_t btn1 = 0;
    assert(buffer != NULL);
    assert(length == 11);
    assert(buffer[0] == NXMC2_HEADER);
    assert(buffer[3] <= NXMC2_HAT_NEUTRAL);
    assert(report != NULL);

    btn0 = buffer[1];
    btn1 = buffer[2];

    report->y = (btn0 >> 0) & 1;
    report->b = (btn0 >> 1) & 1;
    report->a = (btn0 >> 2) & 1;
    report->x = (btn0 >> 3) & 1;
    report->l = (btn0 >> 4) & 1;
    report->r = (btn0 >> 5) & 1;
    /* zl = (btn0 >> 6) & 1; */
    report->z = (btn1 >> 7) & 1;
    /* minus = (btn1 >> 0) & 1; */
    report->start = (btn1 >> 1) & 1;
    /* l_click = (btn1 >> 2) & 1; */
    /* r_click = (btn1 >> 3) & 1; */
    report->reset = (btn1 >> 4) & 1;
    /* capture = (btn1 >> 5) & 1; */

    switch (buffer[3]) {
    case NXMC2_HAT_UP:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case NXMC2_HAT_UPRIGHT:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case NXMC2_HAT_RIGHT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case NXMC2_HAT_DOWNRIGHT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_TRUE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case NXMC2_HAT_DOWN:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_FALSE;
        break;
    case NXMC2_HAT_DOWNLEFT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_TRUE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case NXMC2_HAT_LEFT:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case NXMC2_HAT_UPLEFT:
        report->dup = JIANGTUN_TRUE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_TRUE;
        break;
    case NXMC2_HAT_NEUTRAL:
        report->dup = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        break;
    default:
        assert(JIANGTUN_FALSE);
        break;
    }

    report->xAxis = buffer[4];
    report->yAxis = JIANGTUN_UINT8_MAX - buffer[5];

    report->cxAxis = buffer[6];
    report->cyAxis = JIANGTUN_UINT8_MAX - buffer[7];

    (void)buffer[8];
    (void)buffer[9];
    (void)buffer[10];

    return JIANGTUN_TRUE;
}

static void
transition_0xab_btn0_btn1_hat_lx_ly_rx_ry_0_0(jiangtun_command_t *command,
                                              jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL, patch);
}

static void
transition_0xab_btn0_btn1_hat_lx_ly_rx_ry_0(jiangtun_command_t *command,
                                            jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           transition_0xab_btn0_btn1_hat_lx_ly_rx_ry_0_0, NULL);
}

static void
transition_0xab_btn0_btn1_hat_lx_ly_rx_ry(jiangtun_command_t *command,
                                          jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           transition_0xab_btn0_btn1_hat_lx_ly_rx_ry_0, NULL);
}

static void transition_0xab_btn0_btn1_hat_lx_ly_rx(jiangtun_command_t *command,
                                                   jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           transition_0xab_btn0_btn1_hat_lx_ly_rx_ry, NULL);
}

static void transition_0xab_btn0_btn1_hat_lx_ly(jiangtun_command_t *command,
                                                jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, transition_0xab_btn0_btn1_hat_lx_ly_rx,
                           NULL);
}

static void transition_0xab_btn0_btn1_hat_lx(jiangtun_command_t *command,
                                             jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, transition_0xab_btn0_btn1_hat_lx_ly,
                           NULL);
}

static void transition_0xab_btn0_btn1_hat(jiangtun_command_t *command,
                                          jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, transition_0xab_btn0_btn1_hat_lx, NULL);
}

static void transition_0xab_btn0_btn1(jiangtun_command_t *command,
                                      jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c <= 8 ? transition_0xab_btn0_btn1_hat : NULL, NULL);
}

static void transition_0xab_btn0(jiangtun_command_t *command,
                                 jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, transition_0xab_btn0_btn1, NULL);
}

static void transition_0xab(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, transition_0xab_btn0, NULL);
}

static void transition_initial(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == NXMC2_HEADER ? transition_0xab : NULL, NULL);
}

void jiangtun_nxmc2_init(jiangtun_nxmc2_command_t *command) {
    assert(command != NULL);

    command->base.buffer = command->buffer;
    command->base.length = 0;

    jiangtun_command_init(&(command->base), transition_initial, NULL);
}