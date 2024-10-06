#ifndef JIANGTUN_STATE_H
#define JIANGTUN_STATE_H

#include "compat.h"
#include "report.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef jiangtun_bool_t (*jiangtun_action_t)(const jiangtun_uint8_t *, size_t,
                                             void *);

typedef struct jiangtun_command_t {
    jiangtun_uint8_t *buffer;
    size_t length;

    void (*transition)(struct jiangtun_command_t *, jiangtun_uint8_t);
    jiangtun_action_t action;
} jiangtun_command_t;

/*
 * In C, a struct must be declared before it can be used in a function pointer
 * declaration. Therefore, `jiangtun_command_t` is declared first, followed by
 * the `jiangtun_transition_t` function pointer type.
 */
typedef void (*jiangtun_transition_t)(jiangtun_command_t *, jiangtun_uint8_t);

void jiangtun_command_init(jiangtun_command_t *, jiangtun_transition_t,
                           jiangtun_action_t);
void jiangtun_push(jiangtun_command_t *, jiangtun_uint8_t);
jiangtun_bool_t jiangtun_run(jiangtun_command_t *, void *);

jiangtun_bool_t jiangtun_pending(jiangtun_command_t *);
jiangtun_bool_t jiangtun_accepted(jiangtun_command_t *);
jiangtun_bool_t jiangtun_rejected(jiangtun_command_t *);

/**
 * NX Macro Controller
 *
 * - https://blog.bzl-web.com/entry/2020/01/20/165719
 * - https://github.com/608/CH55xSwitchSerialControl
 *
 * `^\xab[\x00-\xff]{2}[\x00-\x08][\x00-\xff]{7}`
 *
 * ref:
 * https://scrapbox.io/yatsuna827827-12010999/Nintendo_Switch%E3%82%92%E6%93%8D%E4%BD%9C%E3%81%99%E3%82%8B%E3%83%97%E3%83%AD%E3%82%B0%E3%83%A9%E3%83%A0%E3%81%AE%E4%BB%95%E6%A7%98%E3%82%92%E8%AA%BF%E3%81%B9%E3%82%8B
 */
typedef struct jiangtun_nxmc2_command_t {
    jiangtun_command_t base;
    jiangtun_uint8_t buffer[11];
} jiangtun_nxmc2_command_t;

void jiangtun_nxmc2_init(jiangtun_nxmc2_command_t *);

/**
 * Poke-Controller
 *
 * - https://github.com/KawaSwitch/Poke-Controller
 * - https://github.com/Moi-poke/Poke-Controller-Modified
 * - https://github.com/futo030/Poke-Controller-Modified-Extension
 *
 * `^(((0x)?[0-9a-f]{1,4} [0-8]( (0x)?[0-9a-f]{1,2} (0x)?[0-9a-f]{1,2}(
 * (0x)?[0-9a-f]{1,2} (0x)?[0-9a-f]{1,2})?)?)|end)\r?\n`
 *
 * ref:
 * https://github.com/KawaSwitch/Poke-Controller/wiki/%E3%82%B7%E3%83%AA%E3%82%A2%E3%83%AB%E3%83%87%E3%83%BC%E3%82%BF%E3%81%AE%E4%B8%AD%E8%BA%AB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6
 */
typedef struct jiangtun_pokecon_command_t {
    jiangtun_command_t base;
    jiangtun_uint8_t buffer[sizeof("0x0003 0 0x00 0x00 0x00 0x00\r\n") - 1];
} jiangtun_pokecon_command_t;

void jiangtun_pokecon_init(jiangtun_pokecon_command_t *);

/**
 * ORCA GC Controller
 *
 * - https://github.com/yatsuna827/Orca-GC-Controller
 * - https://github.com/mizuyoukanao/Bluewhale
 *
 * `^(\x80\xff*[^\xff]\xff*[^\xff])|@`
 */
typedef struct jiangtun_orca_command_t {
    jiangtun_command_t base;
    jiangtun_uint8_t buffer[3];
} jiangtun_orca_command_t;

void jiangtun_orca_init(jiangtun_orca_command_t *);

/**
 * DOL Macro Controller
 *
 * - https://katari-na.hatenablog.com/entry/2021/03/08/160937
 * - https://github.com/mizuyoukanao/Bluewhale
 *
 * `^[a-z0-9@]`
 */
typedef struct jiangtun_dol_command_t {
    jiangtun_command_t base;
    jiangtun_uint8_t buffer[1];
} jiangtun_dol_command_t;

void jiangtun_dol_init(jiangtun_dol_command_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_STATE_H */