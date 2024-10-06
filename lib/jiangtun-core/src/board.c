#include <jiangtun/board.h>

#include <assert.h>

jiangtun_bool_t jiangtun_board_serial_read(jiangtun_board_t *board,
                                           jiangtun_uint8_t *c) {
    assert(board != NULL);
    assert(c != NULL);

    return board->serial_read(board, c);
}

void jiangtun_board_gamecube_send(jiangtun_board_t *board,
                                  jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    board->gamecube_send(board, report);
}

void jiangtun_board_led_blink_async(jiangtun_board_t *board,
                                    jiangtun_uint32_t duration) {
    assert(board != NULL);

    board->led_blink_async(board, duration);
}

void jiangtun_board_init(jiangtun_board_t *board,
                         jiangtun_serial_read_t serial_read,
                         jiangtun_gamecube_send_t gamecube_send,
                         jiangtun_led_blink_async_t led_blink_async) {
    assert(board != NULL);
    assert(serial_read != NULL);
    assert(gamecube_send != NULL);
    assert(led_blink_async != NULL);

    board->serial_read = serial_read;
    board->gamecube_send = gamecube_send;
    board->led_blink_async = led_blink_async;
}