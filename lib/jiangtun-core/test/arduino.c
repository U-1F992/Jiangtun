#include <jiangtun.h>

#include <assert.h>
#include <stdio.h>

static jiangtun_bool_t serial_read(jiangtun_board_t *board,
                                   jiangtun_uint8_t *c) {
    assert(board != NULL);
    assert(c != NULL);

    fprintf(stderr, "serial_read\n");
    /* *c = '@'; */
    return JIANGTUN_FALSE;
}

static void gamecube_send(jiangtun_board_t *board,
                          jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    fprintf(stderr, "gamecube_send\n");
}

static void led_blink_async(jiangtun_board_t *board,
                            jiangtun_uint32_t duration) {
    assert(board != NULL);

    fprintf(stderr, "led_blink_async: %u\n", duration);
}

int main(void) {
    size_t i = 0;
    jiangtun_t j;
    jiangtun_board_t board;
    jiangtun_feature_flag_t features =
        JIANGTUN_FEATURE_ENABLE_LED_BLINK | JIANGTUN_FEATURE_ENABLE_NXMC2 |
        JIANGTUN_FEATURE_ENABLE_POKECON | JIANGTUN_FEATURE_ENABLE_ORCA;

    /* setup */
    jiangtun_board_init(&board, serial_read, gamecube_send, led_blink_async);
    jiangtun_init(&j, &board, features);

    /* loop */
    for (i = 0; i < 5; i++) {
        jiangtun_loop(&j);
    }

    return 0;
}
