#ifndef JIANGTUN_H
#define JIANGTUN_H

#include "jiangtun/board.h"
#include "jiangtun/command.h"
#include "jiangtun/compat.h"
#include "jiangtun/report.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JIANGTUN_TIMEOUT_5MS 50
#define JIANGTUN_AT_SIGN_500MS 500

typedef jiangtun_uint32_t jiangtun_feature_flag_t;
#define JIANGTUN_FEATURE_ENABLE_LED_BLINK ((jiangtun_feature_flag_t)(1 << 4))
#define JIANGTUN_FEATURE_ENABLE_NXMC2 ((jiangtun_feature_flag_t)(1 << 3))
#define JIANGTUN_FEATURE_ENABLE_POKECON ((jiangtun_feature_flag_t)(1 << 2))
#define JIANGTUN_FEATURE_ENABLE_ORCA ((jiangtun_feature_flag_t)(1 << 1))
#define JIANGTUN_FEATURE_ENABLE_DOL ((jiangtun_feature_flag_t)(1 << 0))

typedef struct jiangtun_t {
    jiangtun_board_t *board;

    jiangtun_nxmc2_command_t nxmc2;
    jiangtun_pokecon_command_t pokecon;
    jiangtun_orca_command_t orca;
    jiangtun_dol_command_t dol;

    jiangtun_command_t *commands[4];
    jiangtun_report_mode3_t reports[4];
    size_t recently_patched;

    jiangtun_uint8_t stash_carry_over_;
    jiangtun_uint8_t *carry_over;

    jiangtun_uint32_t timeout_loop_count;

    jiangtun_bool_t at_sign;
    jiangtun_uint32_t at_sign_loop_count;

    jiangtun_feature_flag_t features;
} jiangtun_t;

void jiangtun_init(jiangtun_t *, jiangtun_board_t *, jiangtun_feature_flag_t);
void jiangtun_loop(jiangtun_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_H */
