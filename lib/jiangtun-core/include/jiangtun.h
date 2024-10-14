#ifndef JIANGTUN_H
#define JIANGTUN_H

#include "jiangtun/board.h"
#include "jiangtun/command.h"
#include "jiangtun/compat.h"
#include "jiangtun/optional.h"
#include "jiangtun/report.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JIANGTUN_MICROSECONDS_PER_LOOP
#define JIANGTUN_MICROSECONDS_PER_LOOP 925
#endif
#define JIANGTUN_LOOPS_FOR_MILLISECONDS(ms)                                    \
    ((jiangtun_uint32_t)((ms) * 1000 / JIANGTUN_MICROSECONDS_PER_LOOP) + 1)
#define JIANGTUN_LOOPS_FOR_TIMEOUT JIANGTUN_LOOPS_FOR_MILLISECONDS(5)
#define JIANGTUN_LOOPS_FOR_LED JIANGTUN_LOOPS_FOR_MILLISECONDS(100)
#define JIANGTUN_LOOPS_FOR_RESET_BLOCKING JIANGTUN_LOOPS_FOR_MILLISECONDS(500)

typedef jiangtun_uint32_t jiangtun_feature_flag_t;
#define JIANGTUN_FEATURE_ENABLE_LED_BLINK ((jiangtun_feature_flag_t)(1 << 4))
#define JIANGTUN_FEATURE_ENABLE_NXMC2 ((jiangtun_feature_flag_t)(1 << 3))
#define JIANGTUN_FEATURE_ENABLE_POKECON ((jiangtun_feature_flag_t)(1 << 2))
#define JIANGTUN_FEATURE_ENABLE_ORCA ((jiangtun_feature_flag_t)(1 << 1))
#define JIANGTUN_FEATURE_ENABLE_DOL ((jiangtun_feature_flag_t)(1 << 0))

/**
 * Compared to the levels commonly provided by high-functionality logging
 * libraries, such as "FATAL, ERROR, WARN, INFO, DEBUG, TRACE," the logging
 * levels provided here are limited.
 * First, Jiangtun does not have a path that leads to FATAL. More precisely,
 * there is no way to detect when such a condition occurs. Even if there were a
 * way to detect it, in situations where we would want to output a FATAL or
 * ERROR log from the firmware, the system is likely already in a state where
 * halting cannot be avoided, and it's hard to imagine that output resources
 * could be secured at that point.
 * Additionally, this library is small-scale enough that during debugging, we
 * always want information equivalent to TRACE. There's no need to differentiate
 * between DEBUG and TRACE.
 */
typedef enum jiangtun_log_level_t {
    JIANGTUN_LOG_LEVEL_WARN,
    JIANGTUN_LOG_LEVEL_INFO,
    JIANGTUN_LOG_LEVEL_DEBUG
} jiangtun_log_level_t;

typedef struct jiangtun_t {
    jiangtun_board_t *board;
    jiangtun_feature_flag_t features;

    jiangtun_nxmc2_command_t nxmc2;
    jiangtun_pokecon_command_t pokecon;
    jiangtun_orca_command_t orca;
    jiangtun_dol_command_t dol;
    jiangtun_command_t *commands[4];

    jiangtun_report_mode3_t reports[4];
    jiangtun_report_mode3_t *recently_patched;

    jiangtun_bool_t any_pending;

    jiangtun_optional_uint8_t carry_over;

    jiangtun_uint32_t timeout_loop;
    jiangtun_uint32_t led_loop;
    jiangtun_uint32_t reset_blocking_loop;

    jiangtun_log_level_t log_level;

    char buffer[128];
    char log_buffer[128];
} jiangtun_t;

void jiangtun_init(jiangtun_t *, jiangtun_board_t *, jiangtun_feature_flag_t,
                   jiangtun_log_level_t);
void jiangtun_loop(jiangtun_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_H */
