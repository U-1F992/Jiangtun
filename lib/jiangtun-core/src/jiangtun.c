#include <jiangtun.h>

#include <assert.h>
#include <time.h>

#define UNIX_EPOCH "1970-01-01T00:00:00Z"

static void serial_log(jiangtun_t *j, jiangtun_log_level_t level,
                       const char *s) {
    char time_string[sizeof(UNIX_EPOCH)];
    time_t now = time(NULL);
    struct tm *tm_info = gmtime(&now);

    if (tm_info != NULL) {
        strftime(time_string, sizeof(time_string), "%Y-%m-%dT%H:%M:%SZ",
                 tm_info);
    } else {
        sprintf(time_string, UNIX_EPOCH);
    }

    sprintf(j->log_buffer, "[%s]\t[%s]\t%s\n", time_string,
            level == JIANGTUN_LOG_LEVEL_WARN    ? "warn"
            : level == JIANGTUN_LOG_LEVEL_INFO  ? "info"
            : level == JIANGTUN_LOG_LEVEL_DEBUG ? "debug"
                                                : "unknown",
            s);
    if (j->log_level >= level) {
        jiangtun_board_serial_puts(j->board, j->log_buffer);
    }
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
    j->led_loop = 0;
    j->reset_blocking_loop = 0;
    j->log_level = log_level;

    /*
     * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L36-L41
     */
    j->reports[0].start = JIANGTUN_TRUE;
    handshake_result = jiangtun_board_gamecube_send(j->board, &(j->reports[0]));
    j->reports[0].start = JIANGTUN_FALSE;
    handshake_result =
        jiangtun_board_gamecube_send(j->board, &(j->reports[0])) &&
        handshake_result;
    if (!handshake_result) {
        serial_log(j, JIANGTUN_LOG_LEVEL_WARN,
                   "handshake failed during initial communications");
    }
}

static void process_input(jiangtun_t *j) {
    size_t i = 0;
    jiangtun_uint8_t c = 0;
    jiangtun_bool_t all_rejected = JIANGTUN_TRUE;
    assert(j != NULL);

    if (j->reset_blocking_loop > 0) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "reset_blocking_loop active");
        return;
    }

    if (jiangtun_optional_uint8_get(&(j->carry_over), &c)) {
        sprintf(j->log_buffer, "carry_over value used: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->log_buffer);

    } else if (jiangtun_board_serial_getc(j->board, &c)) {
        sprintf(j->log_buffer, "received input: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->log_buffer);

    } else {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "no input received");
        if (j->any_pending && --(j->timeout_loop) == 0) {
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, "input timeout");
            init_commands(j);
            j->timeout_loop = JIANGTUN_LOOPS_FOR_TIMEOUT;
        }
        return;
    }
    j->timeout_loop = JIANGTUN_LOOPS_FOR_TIMEOUT;
    jiangtun_optional_uint8_clear(&(j->carry_over));

    for (i = 0; i < 4; i++) {
        jiangtun_command_push(j->commands[i], c);
        if (jiangtun_command_accepted(j->commands[i])) {
            sprintf(j->log_buffer, "command #%lu accepted", (unsigned long)i);
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->log_buffer);

            if (!jiangtun_command_run(j->commands[i], &(j->reports[i]))) {
                serial_log(j, JIANGTUN_LOG_LEVEL_WARN,
                           "something went wrong during jiangtun_command_run");
                /* as rejected */
                continue;
            }
            sprintf(j->log_buffer, "command #%lu executed successfully",
                    (unsigned long)i);
            serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->log_buffer);
            j->recently_patched = &(j->reports[i]);

            jiangtun_board_led_set(
                j->board, j->features & JIANGTUN_FEATURE_ENABLE_LED_BLINK);
            j->led_loop += JIANGTUN_LOOPS_FOR_LED;

            if (!j->any_pending && c == '@') {
                serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                           "'@' received, initiating reset_blocking_loop");
                j->reset_blocking_loop = JIANGTUN_LOOPS_FOR_RESET_BLOCKING;
            }
            init_commands(j);
            return;
        } else if (jiangtun_command_pending(j->commands[i])) {
            all_rejected = JIANGTUN_FALSE;
        }
    }
    if (j->any_pending && all_rejected) {
        sprintf(j->log_buffer, "all commands rejected, carry_over set: %u", c);
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG, j->log_buffer);
        jiangtun_optional_uint8_set(&(j->carry_over), c);
        init_commands(j);
        return;
    }
    j->any_pending = JIANGTUN_TRUE;
}

void jiangtun_loop(jiangtun_t *j) {
    assert(j != NULL);

    process_input(j);

    if (j->reset_blocking_loop > 0 && --(j->reset_blocking_loop) == 0) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                   "reset_blocking_loop completed, reset flag cleared");
        j->recently_patched->reset = JIANGTUN_FALSE;
    }
    jiangtun_report_emend_axis(j->recently_patched);

    if (!jiangtun_board_gamecube_send(j->board, j->recently_patched)) {
        serial_log(j, JIANGTUN_LOG_LEVEL_WARN,
                   "failed to send report: gamecube is either not powered on "
                   "or not connected");
    }

    if (j->led_loop > 0 && --(j->led_loop) == 0) {
        serial_log(j, JIANGTUN_LOG_LEVEL_DEBUG,
                   "led_loop completed, turning off");
        jiangtun_board_led_set(j->board, JIANGTUN_FALSE);
    }
}