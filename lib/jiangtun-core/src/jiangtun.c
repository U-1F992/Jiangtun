#include <jiangtun.h>

#include <assert.h>
#include <time.h>

static void serial_log(jiangtun_t *j, jiangtun_log_level_t level,
                       const char *s) {
    sprintf(j->log_buffer, "[%s]\t%s\n",
            level == JIANGTUN_LOG_LEVEL_WARN    ? "warn"
            : level == JIANGTUN_LOG_LEVEL_INFO  ? "info"
            : level == JIANGTUN_LOG_LEVEL_DEBUG ? "debug"
                                                : "unknown",
            s);
    jiangtun_board_serial_puts(j->board,
                               j->log_level >= level ? j->log_buffer : "");
}

static void init_commands(jiangtun_t *j) {
    assert(j != NULL);

    j->any_pending = JIANGTUN_FALSE;

    j->commands[0] = &(j->nxmc2.base);
    j->commands[1] = &(j->pokecon.base);
    j->commands[2] = &(j->orca.base);
    j->commands[3] = &(j->dol.base);

    if (j->features & JIANGTUN_FEATURE_ENABLE_NXMC2) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                   "initializing NX Macro Controller");
        jiangtun_nxmc2_init(&(j->nxmc2));
    } else {
        jiangtun_command_init(j->commands[0], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_POKECON) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "initializing Poke-Controller");
        jiangtun_pokecon_init(&(j->pokecon));
    } else {
        jiangtun_command_init(j->commands[1], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_ORCA) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                   "initializing ORCA GC Controller");
        jiangtun_orca_init(&(j->orca));
    } else {
        jiangtun_command_init(j->commands[2], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_DOL) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                   "initializing DOL Macro Controller");
        jiangtun_dol_init(&(j->dol));
    } else {
        jiangtun_command_init(j->commands[3], NULL, NULL);
    }
}

static void init_reports(jiangtun_t *j) {
    assert(j != NULL);

    jiangtun_report_init(&(j->reports[0]));
    jiangtun_report_init(&(j->reports[1]));
    jiangtun_report_init(&(j->reports[2]));
    jiangtun_report_init(&(j->reports[3]));

    j->recently_patched = &(j->reports[0]);
}

void jiangtun_init(jiangtun_t *j, jiangtun_board_t *board,
                   jiangtun_feature_flag_t features,
                   jiangtun_log_level_t log_level) {
    jiangtun_bool_t handshake_result = JIANGTUN_FALSE;
    assert(j != NULL);
    assert(board != NULL);

    j->board = board;
    j->features = features;
    init_commands(j);
    init_reports(j);
    j->any_pending = JIANGTUN_FALSE;
    jiangtun_optional_uint8_clear(&(j->carry_over));
    j->timeout_loop = JIANGTUN_LOOPS_FOR_TIMEOUT;
    jiangtun_optional_uint32_clear(&(j->led_on_start_time));
    jiangtun_optional_uint32_clear(&(j->reset_blocking_start_time));
    j->log_level = log_level;

    /*
     * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L36-L41
     */
    j->reports[0].start = JIANGTUN_TRUE;
    handshake_result =
        jiangtun_board_gamecube_send(j->board, JIANGTUN_TRUE, &(j->reports[0]));
    j->reports[0].start = JIANGTUN_FALSE;
    handshake_result = jiangtun_board_gamecube_send(j->board, JIANGTUN_TRUE,
                                                    &(j->reports[0])) &&
                       handshake_result;
    if (!handshake_result) {
        serial_log(j, JIANGTUN_LOG_LEVEL_WARN,
                   "handshake failed during initial communications");
    }
}

static jiangtun_bool_t process_input(jiangtun_t *j,
                                     jiangtun_uint32_t current_millis) {
    size_t i = 0;
    jiangtun_uint32_t reset_blocking_start_time = 0;
    jiangtun_uint8_t c = 0;
    jiangtun_bool_t all_rejected = JIANGTUN_TRUE;
    assert(j != NULL);

    if (jiangtun_optional_uint32_get(&(j->reset_blocking_start_time),
                                     &reset_blocking_start_time)) {
        if (current_millis - reset_blocking_start_time <
            JIANGTUN_RESET_BLOCKING_MILLIS) {
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "reset_blocking");
            return JIANGTUN_FALSE;
        } else {
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "reset_blocking completed");
            j->recently_patched->reset = JIANGTUN_FALSE;
            jiangtun_optional_uint32_clear(&(j->reset_blocking_start_time));
            return JIANGTUN_TRUE;
        }
    }

    if (jiangtun_optional_uint8_get(&(j->carry_over), &c)) {
        sprintf(j->buffer, "carry_over value used: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->buffer);

    } else if (jiangtun_board_serial_getc(j->board, &c)) {
        sprintf(j->buffer, "received input: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->buffer);

    } else {
        if (j->any_pending && --(j->timeout_loop) == 0) {
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "input timeout");
            init_commands(j);
            j->timeout_loop = JIANGTUN_LOOPS_FOR_TIMEOUT;
        }
        return JIANGTUN_FALSE;
    }
    j->timeout_loop = JIANGTUN_LOOPS_FOR_TIMEOUT;
    jiangtun_optional_uint8_clear(&(j->carry_over));

    for (i = 0; i < 4; i++) {
        jiangtun_command_push(j->commands[i], c);
        if (jiangtun_command_accepted(j->commands[i])) {
            sprintf(j->buffer, "command #%lu accepted", (unsigned long)i);
            serial_log(j, JIANGTUN_LOG_LEVEL_INFO, j->buffer);

            if (!jiangtun_command_run(j->commands[i], &(j->reports[i]))) {
                serial_log(j, JIANGTUN_LOG_LEVEL_WARN,
                           "something went wrong during jiangtun_command_run");
                /* as rejected */
                continue;
            }
            j->recently_patched = &(j->reports[i]);

            jiangtun_board_led_set(
                j->board, j->features & JIANGTUN_FEATURE_ENABLE_LED_BLINK);
            jiangtun_optional_uint32_set(&(j->led_on_start_time),
                                         current_millis);

            if (!j->any_pending && c == '@') {
                serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "'@' received");
                jiangtun_optional_uint32_set(&(j->reset_blocking_start_time),
                                             current_millis);
            }
            init_commands(j);
            return JIANGTUN_TRUE;

        } else if (jiangtun_command_pending(j->commands[i])) {
            all_rejected = JIANGTUN_FALSE;
        }
    }
    if (j->any_pending && all_rejected) {
        sprintf(j->buffer, "all commands rejected, carry_over set: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->buffer);
        jiangtun_optional_uint8_set(&(j->carry_over), c);
        init_commands(j);
        return JIANGTUN_FALSE;
    }
    j->any_pending = JIANGTUN_TRUE;
    return JIANGTUN_FALSE;
}

void jiangtun_loop(jiangtun_t *j) {
    jiangtun_bool_t changed = JIANGTUN_FALSE;
    jiangtun_uint32_t current_millis = jiangtun_board_get_millis(j->board);
    jiangtun_uint32_t led_on_start_time = 0;
    assert(j != NULL);

    if ((changed = process_input(j, current_millis))) {
        jiangtun_report_emend_axis(j->recently_patched);
    }

    if (!jiangtun_board_gamecube_send(j->board, changed, j->recently_patched)) {
        serial_log(j, JIANGTUN_LOG_LEVEL_WARN, "failed to send report");
    }

    if (jiangtun_optional_uint32_get(&(j->led_on_start_time),
                                     &led_on_start_time) &&
        current_millis - led_on_start_time >= JIANGTUN_LED_ON_MILLIS) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "led_on completed");
        jiangtun_board_led_set(j->board, JIANGTUN_FALSE);
        jiangtun_optional_uint32_clear(&(j->led_on_start_time));
    }
}