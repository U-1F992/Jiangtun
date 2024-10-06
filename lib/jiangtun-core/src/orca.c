#include <jiangtun/command.h>

#include "internal.h"

#include <assert.h>

#define ORCA_HEADER 0x80
#define ORCA_RELEASE 0x80
#define ORCA_SKIP 0xFF

static jiangtun_bool_t patch_at_sign(const jiangtun_uint8_t *buffer,
                                     size_t length, void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    assert(buffer != NULL);
    assert(length == 1);
    assert(buffer[0] == '@');
    assert(report != NULL);

    report->reset = JIANGTUN_TRUE;

    return JIANGTUN_TRUE;
}

/** `\xff*([^0xff])\xff*([^0xff])` */
static jiangtun_bool_t find_btns(const jiangtun_uint8_t *buffer, size_t length,
                                 jiangtun_uint8_t *btn0,
                                 jiangtun_uint8_t *btn1) {
    size_t i = 0;
    assert(buffer != NULL);
    assert(btn0 != NULL);
    assert(btn1 != NULL);

    while (i < length && buffer[i] == ORCA_SKIP) {
        i++;
    }
    if (i < length && buffer[i] != ORCA_SKIP) {
        *btn0 = buffer[i];
        i++;
    } else {
        return JIANGTUN_FALSE;
    }

    while (i < length - 1 && buffer[i] == ORCA_SKIP) {
        i++;
    }
    if (i == length - 1 && buffer[i] != ORCA_SKIP) {
        *btn1 = buffer[i];
    } else {
        return JIANGTUN_FALSE;
    }

    return JIANGTUN_TRUE;
}

/**
 * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L207-L261
 */
static jiangtun_bool_t patch(const jiangtun_uint8_t *buffer, size_t length,
                             void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    jiangtun_uint8_t btn0 = 0;
    jiangtun_uint8_t btn1 = 0;
    assert(buffer != NULL);
    assert(length == 3);
    assert(buffer[0] == ORCA_HEADER);
    assert(report != NULL);

    if (!find_btns(&(buffer[1]), length - 1, &btn0, &btn1)) {
        assert(JIANGTUN_FALSE);
        return JIANGTUN_FALSE;
    }

    if (btn0 == ORCA_RELEASE) {
        report->a = JIANGTUN_FALSE;
        report->b = JIANGTUN_FALSE;
        report->x = JIANGTUN_FALSE;
        report->y = JIANGTUN_FALSE;
        report->l = JIANGTUN_FALSE;
        report->r = JIANGTUN_FALSE;
    } else {
        report->a = (btn0 >> 0) & 1;
        report->b = (btn0 >> 1) & 1;
        report->x = (btn0 >> 2) & 1;
        report->y = (btn0 >> 3) & 1;
        report->l = (btn0 >> 4) & 1;
        report->r = (btn0 >> 5) & 1;
    }

    if (btn1 == ORCA_RELEASE) {
        report->z = JIANGTUN_FALSE;
        report->start = JIANGTUN_FALSE;
        report->dleft = JIANGTUN_FALSE;
        report->dright = JIANGTUN_FALSE;
        report->dup = JIANGTUN_FALSE;
        report->ddown = JIANGTUN_FALSE;
    } else {
        report->z = (btn1 >> 0) & 1;
        report->start = (btn1 >> 1) & 1;
        report->dleft = (btn1 >> 2) & 1;
        report->dright = (btn1 >> 3) & 1;
        report->dup = (btn1 >> 4) & 1;
        report->ddown = (btn1 >> 5) & 1;
    }

    return JIANGTUN_TRUE;
}

static void transition_0x80_btn0(jiangtun_command_t *command,
                                 jiangtun_uint8_t c) {
    assert(command != NULL);

    if (c != ORCA_SKIP) {
        command->buffer[command->length++] = c;
    }
    jiangtun_command_init(command, c == ORCA_SKIP ? transition_0x80_btn0 : NULL,
                          c == ORCA_SKIP ? NULL : patch);
}

static void transition_0x80(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    if (c != ORCA_SKIP) {
        command->buffer[command->length++] = c;
    }
    jiangtun_command_init(
        command, c == ORCA_SKIP ? transition_0x80 : transition_0x80_btn0, NULL);
}

static void transition_initial(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c,
                           c == ORCA_HEADER ? transition_0x80 : NULL,
                           c == '@' ? patch_at_sign : NULL);
}

void jiangtun_orca_init(jiangtun_orca_command_t *command) {
    assert(command != NULL);

    command->base.buffer = command->buffer;
    command->base.length = 0;

    jiangtun_command_init(&(command->base), transition_initial, NULL);
}