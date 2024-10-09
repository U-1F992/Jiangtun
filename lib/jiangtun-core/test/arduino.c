#include <jiangtun.h>

#include <assert.h>
#include <stdio.h>

static jiangtun_bool_t serial_getc(jiangtun_board_t *board,
                                   jiangtun_uint8_t *c) {
    assert(board != NULL);
    assert(c != NULL);

    fprintf(stderr, "serial_getc\n");
    /* *c = '@'; */
    return JIANGTUN_FALSE;
}

static void serial_puts(jiangtun_board_t *board, const char *s) {
    assert(board != NULL);
    assert(s != NULL);

    fputs(s, stderr);
}

static jiangtun_bool_t gamecube_send(jiangtun_board_t *board,
                                     jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    fprintf(stderr, "gamecube_send\n");
    return JIANGTUN_FALSE;
}

static void led_set(jiangtun_board_t *board, jiangtun_bool_t state) {
    assert(board != NULL);

    fprintf(stderr, "led_set: %u\n", state);
}

int main(void) {
    size_t i = 0;
    jiangtun_t j;
    jiangtun_board_t board;
    jiangtun_feature_flag_t features =
        JIANGTUN_FEATURE_ENABLE_LED_BLINK | JIANGTUN_FEATURE_ENABLE_NXMC2 |
        JIANGTUN_FEATURE_ENABLE_POKECON | JIANGTUN_FEATURE_ENABLE_ORCA;

    jiangtun_board_init(&board, serial_getc, serial_puts, gamecube_send,
                        led_set);
    jiangtun_init(&j, &board, features, JIANGTUN_LOG_LEVEL_DEBUG);

    for (i = 0; i < 5; i++) {
        jiangtun_loop(&j);
    }

    return 0;
}
