#ifndef JIANGTUN_BOARD_H
#define JIANGTUN_BOARD_H

#include "compat.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct jiangtun_board_t {
    jiangtun_bool_t (*serial_read)(struct jiangtun_board_t *,
                                   jiangtun_uint8_t *);
    void (*gamecube_send)(struct jiangtun_board_t *, jiangtun_report_mode3_t *);
    void (*led_blink_async)(struct jiangtun_board_t *, jiangtun_uint32_t);
} jiangtun_board_t;

typedef jiangtun_bool_t (*jiangtun_serial_read_t)(jiangtun_board_t *,
                                                  jiangtun_uint8_t *);
typedef void (*jiangtun_gamecube_send_t)(jiangtun_board_t *,
                                         jiangtun_report_mode3_t *);
typedef void (*jiangtun_led_blink_async_t)(jiangtun_board_t *,
                                           jiangtun_uint32_t);

jiangtun_bool_t jiangtun_board_serial_read(jiangtun_board_t *,
                                           jiangtun_uint8_t *);
void jiangtun_board_gamecube_send(jiangtun_board_t *,
                                  jiangtun_report_mode3_t *);
void jiangtun_board_led_blink_async(jiangtun_board_t *, jiangtun_uint32_t);
void jiangtun_board_init(jiangtun_board_t *, jiangtun_serial_read_t,
                         jiangtun_gamecube_send_t, jiangtun_led_blink_async_t);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_BOARD_H */