#ifndef JIANGTUN_BOARD_H
#define JIANGTUN_BOARD_H

#include "compat.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jiangtun_board_t {
    jiangtun_bool_t (*serial_getc)(struct jiangtun_board_t *,
                                   jiangtun_uint8_t *);
    void (*serial_puts)(struct jiangtun_board_t *, const char *);
    jiangtun_bool_t (*gamecube_send)(struct jiangtun_board_t *,
                                     jiangtun_report_mode3_t *);
    void (*led_set)(struct jiangtun_board_t *, jiangtun_bool_t);
} jiangtun_board_t;

typedef jiangtun_bool_t (*jiangtun_serial_getc_t)(jiangtun_board_t *,
                                                  jiangtun_uint8_t *);
typedef void (*jiangtun_serial_puts_t)(jiangtun_board_t *, const char *);
typedef jiangtun_bool_t (*jiangtun_gamecube_send_t)(jiangtun_board_t *,
                                                    jiangtun_report_mode3_t *);
typedef void (*jiangtun_led_set_t)(jiangtun_board_t *, jiangtun_bool_t);

jiangtun_bool_t jiangtun_board_serial_getc(jiangtun_board_t *,
                                           jiangtun_uint8_t *);
void jiangtun_board_serial_puts(jiangtun_board_t *, const char *);
jiangtun_bool_t jiangtun_board_gamecube_send(jiangtun_board_t *,
                                             jiangtun_report_mode3_t *);
void jiangtun_board_led_set(jiangtun_board_t *, jiangtun_bool_t);
void jiangtun_board_init(jiangtun_board_t *, jiangtun_serial_getc_t,
                         jiangtun_serial_puts_t, jiangtun_gamecube_send_t,
                         jiangtun_led_set_t);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_BOARD_H */