#include <jiangtun.h>

#include <assert.h>

static void commands_init(jiangtun_t *j) {
    assert(j != NULL);

    j->commands[0] = &(j->nxmc2.base);
    j->commands[1] = &(j->pokecon.base);
    j->commands[2] = &(j->orca.base);
    j->commands[3] = &(j->dol.base);

    if (j->features & JIANGTUN_FEATURE_ENABLE_NXMC2) {
        jiangtun_nxmc2_init(&(j->nxmc2));
    } else {
        jiangtun_command_init(j->commands[0], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_POKECON) {
        jiangtun_pokecon_init(&(j->pokecon));
    } else {
        jiangtun_command_init(j->commands[1], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_ORCA) {
        jiangtun_orca_init(&(j->orca));
    } else {
        jiangtun_command_init(j->commands[2], NULL, NULL);
    }

    if (j->features & JIANGTUN_FEATURE_ENABLE_DOL) {
        jiangtun_dol_init(&(j->dol));
    } else {
        jiangtun_command_init(j->commands[3], NULL, NULL);
    }
}

static void reports_init(jiangtun_t *j) {
    assert(j != NULL);

    jiangtun_report_init(&(j->reports[0]));
    jiangtun_report_init(&(j->reports[1]));
    jiangtun_report_init(&(j->reports[2]));
    jiangtun_report_init(&(j->reports[3]));
}

void jiangtun_init(jiangtun_t *j, jiangtun_board_t *board,
                   jiangtun_feature_flag_t features) {
    assert(j != NULL);
    assert(board != NULL);

    j->features = features;

    reports_init(j);
    commands_init(j);

    j->board = board;
    j->recently_patched = 0;
    j->stash_carry_over_ = 0;
    j->carry_over = NULL;
    j->timeout_loop_count = 0;
    j->at_sign = JIANGTUN_FALSE;
    j->at_sign_loop_count = 0;

    /*
     * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L36-L41
     */
    j->reports[0].start = JIANGTUN_TRUE;
    jiangtun_board_gamecube_send(j->board, &(j->reports[0]));

    j->reports[0].start = JIANGTUN_FALSE;
    jiangtun_board_gamecube_send(j->board, &(j->reports[0]));
}

void jiangtun_loop(jiangtun_t *j) {
    size_t i = 0;
    jiangtun_uint8_t c = 0;
    jiangtun_bool_t at_sign = JIANGTUN_FALSE;
    jiangtun_bool_t any_pending = JIANGTUN_FALSE;
    jiangtun_report_mode3_t *report = NULL;
    assert(j != NULL);

    if (j->at_sign) {
        goto gamecube_send;
    }

    if (j->carry_over == NULL && !jiangtun_board_serial_read(j->board, &c)) {
        if (++(j->timeout_loop_count) > JIANGTUN_TIMEOUT_5MS) {
            commands_init(j);
            j->timeout_loop_count = 0;
        }
        goto gamecube_send;
    }

    j->timeout_loop_count = 0;
    if (j->carry_over != NULL) {
        c = *(j->carry_over);
        j->carry_over = NULL;
    }

    for (i = 0; i < 4; i++) {
        jiangtun_push(j->commands[i], c);
        if (jiangtun_accepted(j->commands[i])) {
            jiangtun_run(j->commands[i], &(j->reports[i]));
            commands_init(j);
            j->recently_patched = i;
            at_sign = c == '@';
            goto gamecube_send;

        } else if (jiangtun_pending(j->commands[i])) {
            any_pending = JIANGTUN_TRUE;
        }
    }
    if (!any_pending) {
        j->stash_carry_over_ = c;
        j->carry_over = &(j->stash_carry_over_);
        commands_init(j);
    }

gamecube_send:
    report = &(j->reports[j->recently_patched]);
    if (j->at_sign && ++(j->at_sign_loop_count) > JIANGTUN_AT_SIGN_500MS) {
        report->reset = JIANGTUN_FALSE;
        j->at_sign_loop_count = 0;
    }
    jiangtun_emend_axis(report);
    jiangtun_board_gamecube_send(j->board, report);
    if (j->features & JIANGTUN_FEATURE_ENABLE_LED_BLINK) {
        jiangtun_board_led_blink_async(j->board, 50);
    }
    j->at_sign = at_sign;
}