#include <jiangtun/command.h>

#include "internal.h"

#include <assert.h>

static jiangtun_bool_t is_valid_char(jiangtun_uint8_t c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || c == '@';
}

/**
 * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L47-L194
 */
static jiangtun_bool_t patch(const jiangtun_uint8_t *buffer, size_t length,
                             void *context) {
    jiangtun_report_mode3_t *report = (jiangtun_report_mode3_t *)context;
    assert(buffer != NULL);
    assert(length == 1);
    assert(is_valid_char(buffer[0]));
    assert(report != NULL);

    switch (buffer[0]) {
    case 'a':
        report->a = JIANGTUN_TRUE;
        break;
    case 'b':
        report->b = JIANGTUN_TRUE;
        break;
    case 'c':
        report->x = JIANGTUN_TRUE;
        break;
    case 'd':
        report->y = JIANGTUN_TRUE;
        break;
    case 'e':
        report->l = JIANGTUN_TRUE;
        break;
    case 'f':
        report->r = JIANGTUN_TRUE;
        break;
    case 'g':
        report->z = JIANGTUN_TRUE;
        break;
    case 'h':
        report->start = JIANGTUN_TRUE;
        break;
    case 'i':
        report->dleft = JIANGTUN_TRUE;
        break;
    case 'j':
        report->dright = JIANGTUN_TRUE;
        break;
    case 'k':
        report->ddown = JIANGTUN_TRUE;
        break;
    case 'l':
        report->dup = JIANGTUN_TRUE;
        break;
    case 'm':
        report->a = JIANGTUN_FALSE;
        break;
    case 'n':
        report->b = JIANGTUN_FALSE;
        break;
    case 'o':
        report->x = JIANGTUN_FALSE;
        break;
    case 'p':
        report->y = JIANGTUN_FALSE;
        break;
    case 'q':
        report->l = JIANGTUN_FALSE;
        break;
    case 'r':
        report->r = JIANGTUN_FALSE;
        break;
    case 's':
        report->z = JIANGTUN_FALSE;
        break;
    case 't':
        report->start = JIANGTUN_FALSE;
        break;
    case 'u':
        report->dleft = JIANGTUN_FALSE;
        break;
    case 'v':
        report->dright = JIANGTUN_FALSE;
        break;
    case 'w':
        report->ddown = JIANGTUN_FALSE;
        break;
    case 'x':
        report->dup = JIANGTUN_FALSE;
        break;
    case 'y':
        report->xAxis = JIANGTUN_REPORT_STICK_LEFT;
        break;
    case 'z':
        report->xAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
        break;
    case '1':
        report->xAxis = JIANGTUN_REPORT_STICK_RIGHT;
        break;
    case '2':
        report->yAxis = JIANGTUN_REPORT_STICK_DOWN;
        break;
    case '3':
        report->yAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
        break;
    case '4':
        report->yAxis = JIANGTUN_REPORT_STICK_UP;
        break;
    case '5':
        report->cxAxis = JIANGTUN_REPORT_STICK_LEFT;
        break;
    case '6':
        report->cxAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
        break;
    case '7':
        report->cxAxis = JIANGTUN_REPORT_STICK_RIGHT;
        break;
    case '8':
        report->cyAxis = JIANGTUN_REPORT_STICK_DOWN;
        break;
    case '9':
        report->cyAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
        break;
    case '0':
        report->cyAxis = JIANGTUN_REPORT_STICK_UP;
        break;
    case '@':
        report->reset = JIANGTUN_TRUE;
        break;
    default:
        assert(JIANGTUN_FALSE);
        break;
    }

    return JIANGTUN_TRUE;
}

static void transition_initial(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    append_if_not_rejected(command, c, NULL, is_valid_char(c) ? patch : NULL);
}

void jiangtun_dol_init(jiangtun_dol_command_t *command) {
    assert(command != NULL);

    command->base.buffer = command->buffer;
    command->base.length = 0;

    jiangtun_command_init(&(command->base), transition_initial, NULL);
}