#include <jiangtun/board.h>

#include <assert.h>

jiangtun_bool_t jiangtun_board_serial_getc(jiangtun_board_t *board,
                                           jiangtun_uint8_t *c) {
    assert(board != NULL);
    assert(c != NULL);

    return board->serial_getc(board, c);
}

void jiangtun_board_serial_puts(jiangtun_board_t *board, const char *s) {
    assert(board != NULL);
    assert(s != NULL);

    board->serial_puts(board, s);
}

jiangtun_bool_t jiangtun_board_gamecube_send(jiangtun_board_t *board,
                                             jiangtun_bool_t changed,
                                             jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    return board->gamecube_send(board, changed, report);
}

void jiangtun_board_led_set(jiangtun_board_t *board, jiangtun_bool_t state) {
    assert(board != NULL);

    board->led_set(board, state);
}

void jiangtun_board_init(jiangtun_board_t *board,
                         jiangtun_serial_getc_t serial_getc,
                         jiangtun_serial_puts_t serial_puts,
                         jiangtun_gamecube_send_t gamecube_send,
                         jiangtun_led_set_t led_set) {
    assert(board != NULL);
    assert(serial_getc != NULL);
    assert(serial_puts != NULL);
    assert(gamecube_send != NULL);
    assert(led_set != NULL);

    board->serial_getc = serial_getc;
    board->serial_puts = serial_puts;
    board->gamecube_send = gamecube_send;
    board->led_set = led_set;
}