#include <jiangtun.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

typedef struct test_case_t {
    enum { TEST_NXMC2, TEST_POKECON, TEST_ORCA, TEST_DOL } command;
    jiangtun_uint8_t *buffer;
    size_t length;
    enum { TEST_PENDING, TEST_ACCEPTED, TEST_REJECTED } expected;
    jiangtun_report_mode3_t *expected_report;
} test_case_t;

#define TEST_CASES_APPEND(command_, buffer_, length_, expected_)               \
    do {                                                                       \
        test_cases[test_cases_length].command = (command_);                    \
        test_cases[test_cases_length].buffer = (jiangtun_uint8_t *)(buffer_);  \
        test_cases[test_cases_length].length = (length_);                      \
        test_cases[test_cases_length].expected = (expected_);                  \
        test_cases[test_cases_length].expected_report = NULL;                  \
        test_cases_length++;                                                   \
    } while (0)

#define TEST_ASSERT(expr)                                                      \
    if (!(expr)) {                                                             \
        fprintf(stderr, "Assertion failed: \"%s\", %s:%d, index: %lu\n",       \
                #expr, __FILE__, __LINE__, (unsigned long)i);                  \
        test_failed_count++;                                                   \
        continue;                                                              \
    }                                                                          \
    do {                                                                       \
    } while (0)

#define TEST_SET(report_, a_, b_, x_, y_, start_, dleft_, dright_, ddown_,     \
                 dup_, z_, r_, l_, xAxis_, yAxis_, cxAxis_, cyAxis_, left_,    \
                 right_, reset_)                                               \
    do {                                                                       \
        (report_)->a = (a_);                                                   \
        (report_)->b = (b_);                                                   \
        (report_)->x = (x_);                                                   \
        (report_)->y = (y_);                                                   \
        (report_)->start = (start_);                                           \
        (report_)->dleft = (dleft_);                                           \
        (report_)->dright = (dright_);                                         \
        (report_)->ddown = (ddown_);                                           \
        (report_)->dup = (dup_);                                               \
        (report_)->z = (z_);                                                   \
        (report_)->r = (r_);                                                   \
        (report_)->l = (l_);                                                   \
        (report_)->xAxis = (xAxis_);                                           \
        (report_)->yAxis = (yAxis_);                                           \
        (report_)->cxAxis = (cxAxis_);                                         \
        (report_)->cyAxis = (cyAxis_);                                         \
        (report_)->left = (left_);                                             \
        (report_)->right = (right_);                                           \
        (report_)->reset = (reset_);                                           \
    } while (0)

#define TEST_COMPARE(expected_, actual_)                                       \
    do {                                                                       \
        TEST_ASSERT((expected_)->a == (actual_)->a);                           \
        TEST_ASSERT((expected_)->b == (actual_)->b);                           \
        TEST_ASSERT((expected_)->x == (actual_)->x);                           \
        TEST_ASSERT((expected_)->y == (actual_)->y);                           \
        TEST_ASSERT((expected_)->start == (actual_)->start);                   \
        TEST_ASSERT((expected_)->dleft == (actual_)->dleft);                   \
        TEST_ASSERT((expected_)->dright == (actual_)->dright);                 \
        TEST_ASSERT((expected_)->ddown == (actual_)->ddown);                   \
        TEST_ASSERT((expected_)->dup == (actual_)->dup);                       \
        TEST_ASSERT((expected_)->z == (actual_)->z);                           \
        TEST_ASSERT((expected_)->r == (actual_)->r);                           \
        TEST_ASSERT((expected_)->l == (actual_)->l);                           \
        TEST_ASSERT(abs((int)((expected_)->xAxis) - (actual_)->xAxis) <= 1);   \
        TEST_ASSERT(abs((int)((expected_)->yAxis) - (actual_)->yAxis) <= 1);   \
        TEST_ASSERT(abs((int)((expected_)->cxAxis) - (actual_)->cxAxis) <= 1); \
        TEST_ASSERT(abs((int)((expected_)->cyAxis) - (actual_)->cyAxis) <= 1); \
        TEST_ASSERT((expected_)->left == (actual_)->left);                     \
        TEST_ASSERT((expected_)->right == (actual_)->right);                   \
        TEST_ASSERT((expected_)->reset == (actual_)->reset);                   \
    } while (0)

int test(void) {
    size_t i = 0;
    jiangtun_nxmc2_command_t nxmc2;
    jiangtun_pokecon_command_t pokecon;
    jiangtun_orca_command_t orca;
    jiangtun_dol_command_t dol;
    test_case_t test_cases[256];
    size_t test_cases_length = 0;
    int test_failed_count = 0;

    jiangtun_report_mode3_t expected_y_reset;
    jiangtun_report_mode3_t expected_dup;
    jiangtun_report_mode3_t expected_dup_cupleft;
    jiangtun_report_mode3_t expected_dup_upleft_cupleft;
    jiangtun_report_mode3_t expected_all_bits;
    jiangtun_report_mode3_t expected_neutral;
    jiangtun_report_mode3_t expected_a;
    jiangtun_report_mode3_t expected_cup;
    jiangtun_report_mode3_t expected_reset;

    jiangtun_report_init(&expected_y_reset);
    TEST_SET(&expected_y_reset, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_TRUE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_TRUE);
    jiangtun_report_init(&expected_dup);
    TEST_SET(&expected_dup, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_TRUE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE);
    jiangtun_report_init(&expected_dup_cupleft);
    TEST_SET(&expected_dup_cupleft, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_TRUE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_LEFT,
             JIANGTUN_REPORT_STICK_UP, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE);
    jiangtun_report_init(&expected_dup_upleft_cupleft);
    TEST_SET(&expected_dup_upleft_cupleft, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_TRUE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_LEFT,
             JIANGTUN_REPORT_STICK_UP, JIANGTUN_REPORT_STICK_LEFT,
             JIANGTUN_REPORT_STICK_UP, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE);
    jiangtun_report_init(&expected_all_bits);
    TEST_SET(&expected_all_bits, JIANGTUN_TRUE, JIANGTUN_TRUE, JIANGTUN_TRUE,
             JIANGTUN_TRUE, JIANGTUN_TRUE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_TRUE, JIANGTUN_TRUE,
             JIANGTUN_TRUE, JIANGTUN_REPORT_STICK_RIGHT,
             JIANGTUN_REPORT_STICK_DOWN, JIANGTUN_REPORT_STICK_RIGHT,
             JIANGTUN_REPORT_STICK_DOWN, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_TRUE);
    jiangtun_report_init(&expected_neutral);
    jiangtun_report_init(&expected_a);
    TEST_SET(&expected_a, JIANGTUN_TRUE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE);
    jiangtun_report_init(&expected_cup);
    TEST_SET(&expected_cup, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_UP,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE);
    jiangtun_report_init(&expected_reset);
    TEST_SET(&expected_reset, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_FALSE, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_REPORT_STICK_NEUTRAL,
             JIANGTUN_REPORT_STICK_NEUTRAL, JIANGTUN_FALSE, JIANGTUN_FALSE,
             JIANGTUN_TRUE);

    /* clang-format off */
    TEST_CASES_APPEND(TEST_NXMC2, "\xab\x01\x10\x08\x80\x80\x80\x80\x00\x01\x02", 11, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_y_reset;
    TEST_CASES_APPEND(TEST_NXMC2, "\xab\x00\x00\x00", 4, TEST_PENDING);
    TEST_CASES_APPEND(TEST_NXMC2, "\xff", 1, TEST_REJECTED);

    TEST_CASES_APPEND(TEST_POKECON, "0 0\n", 4, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0 0\r\n", 5, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0000 0\n", 7, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0000 0\r\n", 8, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0x0 0\n", 6, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0x0 0\r\n", 7, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0x0000 0\n", 9, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "0x0000 0\r\n", 10, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0 0\n", 8, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0 0\r\n", 9, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 00 00\n", 10, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 00 00\r\n", 11, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0x0 0x0\n", 12, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0x0 0x0\r\n", 13, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0x00 0x00\n", 14, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "1 0 0x00 0x00\r\n", 15, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0 0\n", 12, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0 0\r\n", 13, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 00 00\n", 14, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 00 00\r\n", 15, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0x0 0x0\n", 16, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0x0 0x0\r\n", 17, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0x00 0x00\n", 18, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "3 0 0 0 0x00 0x00\r\n", 19, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_dup_upleft_cupleft;
    TEST_CASES_APPEND(TEST_POKECON, "0xffff 8 0xff 0xff 0xff 0xff\r\n",30, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_all_bits;
    TEST_CASES_APPEND(TEST_POKECON, "end\n", 4, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_neutral;
    TEST_CASES_APPEND(TEST_POKECON, "end\r\n", 5, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_neutral;
    TEST_CASES_APPEND(TEST_POKECON, "0 0", 3, TEST_PENDING);
    TEST_CASES_APPEND(TEST_POKECON, "h", 1, TEST_REJECTED);

    TEST_CASES_APPEND(TEST_ORCA, "\x80\x00\x00", 3, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_neutral;
    TEST_CASES_APPEND(TEST_ORCA, "\x80\xff\x00\x00", 4, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_neutral;
    TEST_CASES_APPEND(TEST_ORCA, "\x80\x00\xff\x00", 4, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_neutral;
    TEST_CASES_APPEND(TEST_ORCA, "@", 1, TEST_ACCEPTED);
    TEST_CASES_APPEND(TEST_ORCA, "\x80\x00", 2, TEST_PENDING);
    TEST_CASES_APPEND(TEST_ORCA, "\x79", 2, TEST_REJECTED);

    TEST_CASES_APPEND(TEST_DOL, "a", 1, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_a;
    TEST_CASES_APPEND(TEST_DOL, "0", 1, TEST_ACCEPTED);
    TEST_CASES_APPEND(TEST_DOL, "@", 1, TEST_ACCEPTED);
    test_cases[test_cases_length - 1].expected_report = &expected_reset;
    TEST_CASES_APPEND(TEST_DOL, "X", 1, TEST_REJECTED);
    /* clang-format on */

    for (i = 0; i < test_cases_length; i++) {
        size_t j = 0;
        test_case_t *test_case = &(test_cases[i]);
        jiangtun_report_mode3_t report;
        jiangtun_command_t *command = NULL;

        jiangtun_report_init(&report);

        switch (test_case->command) {
        case TEST_NXMC2:
            jiangtun_nxmc2_init(&nxmc2);
            command = &(nxmc2.base);
            break;
        case TEST_POKECON:
            jiangtun_pokecon_init(&pokecon);
            command = &(pokecon.base);
            break;
        case TEST_ORCA:
            jiangtun_orca_init(&orca);
            command = &(orca.base);
            break;
        case TEST_DOL:
            jiangtun_dol_init(&dol);
            command = &(dol.base);
            break;
        default:
            assert(0);
            break;
        }

        for (j = 0; j < test_case->length; j++) {
            jiangtun_push(command, test_case->buffer[j]);
        }

        switch (test_case->expected) {
        case TEST_PENDING:
            TEST_ASSERT(jiangtun_pending(command));
            break;
        case TEST_ACCEPTED:
            TEST_ASSERT(jiangtun_accepted(command));
            TEST_ASSERT(jiangtun_run(command, &report));
            if (test_case->expected_report != NULL) {
                TEST_COMPARE(test_case->expected_report, &(report));
            }
            break;
        case TEST_REJECTED:
            TEST_ASSERT(jiangtun_rejected(command));
            break;
        default:
            assert(0);
        }
    }
    return test_failed_count;
}

int main(void) {
    int test_failed_count = 0;
    test_failed_count += test();
    return test_failed_count;
}