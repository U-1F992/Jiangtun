#include <jiangtun.h>

#include <assert.h>
#include <stdio.h>

typedef struct test_case_t {
    jiangtun_uint8_t *buffer;
    size_t buffer_length;
    jiangtun_error_t expected_err;
    jiangtun_lexer_result_t expected_ret;
} test_case_t;

#define REGISTER_CASE(buffer_, buffer_length_, expexted_err_, expected_ret_)   \
    do {                                                                       \
        test_cases[i].buffer = (buffer_);                                      \
        test_cases[i].buffer_length = (buffer_length_);                        \
        test_cases[i].expected_err = (expexted_err_);                          \
        test_cases[i].expected_ret = (expected_ret_);                          \
        i++;                                                                   \
    } while (0)

#define TEST_ASSERT(expr)                                                      \
    if (!(expr)) {                                                             \
        fprintf(stderr, "Assertion failed: \"%s\", index: %lu\n", #expr,       \
                (unsigned long)i);                                             \
        test_failed_count++;                                                   \
        continue;                                                              \
    }                                                                          \
    do {                                                                       \
    } while (0)

int main() {
    size_t i = 0;

    test_case_t test_cases[84];
    size_t test_cases_length = sizeof(test_cases) / sizeof(test_case_t);
    size_t test_failed_count = 0;

    /* NULL pointer */
    jiangtun_uint8_t *buffer_0 = NULL;
    /* Zero size */
    jiangtun_uint8_t buffer_1[] = {0x00};
    /* No match */
    jiangtun_uint8_t buffer_2[] = {0xFF};

    /* NX Macro Controller */
    jiangtun_uint8_t buffer_3[] = {0xAB};
    jiangtun_uint8_t buffer_4[] = {0xAB, 0x00};
    jiangtun_uint8_t buffer_5[] = {0xAB, 0x00, 0x00};
    jiangtun_uint8_t buffer_6[] = {0xAB, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_7[] = {0xAB, 0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_8[] = {0xAB, 0x00, 0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_9[] = {0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_10[] = {0xAB, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_11[] = {0xAB, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_12[] = {0xAB, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_13[] = {0xAB, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00};
    jiangtun_uint8_t buffer_14[] = {0xAB, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

    /* Poke-Controller */
    /* clang-format off */
    jiangtun_uint8_t buffer_15[] = "0";
    jiangtun_uint8_t buffer_16[] = "0 ";
    jiangtun_uint8_t buffer_17[] = "0 0";
    jiangtun_uint8_t buffer_18[] = "0 0\r";
    jiangtun_uint8_t buffer_19[] = "0 0 ";
    jiangtun_uint8_t buffer_20[] = "0 0 0";
    jiangtun_uint8_t buffer_21[] = "0 0 0 ";
    jiangtun_uint8_t buffer_22[] = "0 0 0 0";
    jiangtun_uint8_t buffer_23[] = "0 0 0 0\r";
    jiangtun_uint8_t buffer_24[] = "0 0 0 0 ";
    jiangtun_uint8_t buffer_25[] = "0 0 0 0 0";
    jiangtun_uint8_t buffer_26[] = "0 0 0 0 0 ";
    jiangtun_uint8_t buffer_27[] = "0 0 0 0 0 0";
    jiangtun_uint8_t buffer_28[] = "0 0 0 0 0 0\r";

    jiangtun_uint8_t buffer_29[] = "0 0\n";
    jiangtun_uint8_t buffer_30[] = "0 0\r\n";
    jiangtun_uint8_t buffer_31[] = "0 0 0 0\n";
    jiangtun_uint8_t buffer_32[] = "0 0 0 0\r\n";
    jiangtun_uint8_t buffer_33[] = "0 0 0 0 0 0\n";
    jiangtun_uint8_t buffer_34[] = "0 0 0 0 0 0\r\n";

    jiangtun_uint8_t buffer_35[] = "0x";
    jiangtun_uint8_t buffer_36[] = "0x0";
    jiangtun_uint8_t buffer_37[] = "0x0 ";
    jiangtun_uint8_t buffer_38[] = "0x0 0";
    jiangtun_uint8_t buffer_39[] = "0x0 0\r";
    jiangtun_uint8_t buffer_40[] = "0x0 0 ";
    jiangtun_uint8_t buffer_41[] = "0x0 0 0";
    jiangtun_uint8_t buffer_42[] = "0x0 0 0x";
    jiangtun_uint8_t buffer_43[] = "0x0 0 0x0";
    jiangtun_uint8_t buffer_44[] = "0x0 0 0x0 ";
    jiangtun_uint8_t buffer_45[] = "0x0 0 0x0 0";
    jiangtun_uint8_t buffer_46[] = "0x0 0 0x0 0x";
    jiangtun_uint8_t buffer_47[] = "0x0 0 0x0 0x0";
    jiangtun_uint8_t buffer_48[] = "0x0 0 0x0 0x0\r";
    jiangtun_uint8_t buffer_49[] = "0x0 0 0x0 0x0 ";
    jiangtun_uint8_t buffer_50[] = "0x0 0 0x0 0x0 0";
    jiangtun_uint8_t buffer_51[] = "0x0 0 0x0 0x0 0x";
    jiangtun_uint8_t buffer_52[] = "0x0 0 0x0 0x0 0x0";
    jiangtun_uint8_t buffer_53[] = "0x0 0 0x0 0x0 0x0 ";
    jiangtun_uint8_t buffer_54[] = "0x0 0 0x0 0x0 0x0 0";
    jiangtun_uint8_t buffer_55[] = "0x0 0 0x0 0x0 0x0 0x";
    jiangtun_uint8_t buffer_56[] = "0x0 0 0x0 0x0 0x0 0x0";
    jiangtun_uint8_t buffer_57[] = "0x0 0 0x0 0x0 0x0 0x0\r";

    jiangtun_uint8_t buffer_58[] = "0x0 0\n";
    jiangtun_uint8_t buffer_59[] = "0x0 0\r\n";
    jiangtun_uint8_t buffer_60[] = "0x0 0 0x0 0x0\n";
    jiangtun_uint8_t buffer_61[] = "0x0 0 0x0 0x0\r\n";
    jiangtun_uint8_t buffer_62[] = "0x0 0 0x0 0x0 0x0 0x0\n";
    jiangtun_uint8_t buffer_63[] = "0x0 0 0x0 0x0 0x0 0x0\r\n";
    
    jiangtun_uint8_t buffer_64[] = "0x0 0 0x0 0x0 0x0 0x0\r\n\xFF";

    jiangtun_uint8_t buffer_65[] = "e";
    jiangtun_uint8_t buffer_66[] = "en";
    jiangtun_uint8_t buffer_67[] = "end";
    jiangtun_uint8_t buffer_68[] = "end\r";
    jiangtun_uint8_t buffer_69[] = "end\n";
    jiangtun_uint8_t buffer_70[] = "end\r\n";
    jiangtun_uint8_t buffer_71[] = "end\r\n\xFF";
    /* clang-format on */

    jiangtun_uint8_t buffer_72[] = {0x80};
    jiangtun_uint8_t buffer_73[] = {0x80, 0x00};
    jiangtun_uint8_t buffer_74[] = {0x80, 0x00, 0x00};
    jiangtun_uint8_t buffer_75[] = {0x80, 0x00, 0x00, 0xFF};
    jiangtun_uint8_t buffer_76[] = {0x80, 0xFF};
    jiangtun_uint8_t buffer_77[] = {0x80, 0xFF, 0x00};
    jiangtun_uint8_t buffer_78[] = {0x80, 0xFF, 0xFF};
    jiangtun_uint8_t buffer_79[] = {0x80, 0x00};
    jiangtun_uint8_t buffer_80[] = {0x80, 0x00, 0xFF};
    jiangtun_uint8_t buffer_81[] = {0x80, 0x00, 0xFF, 0xFF};
    jiangtun_uint8_t buffer_82[] = {0x80, 0x00, 0xFF, 0xFF, 0x00};
    jiangtun_uint8_t buffer_83[] = {0x80, 0x00, 0xFF, 0xFF, 0x00, 0xFF};
    jiangtun_uint8_t buffer_84[] = "@";

    /* NULL pointer */
    REGISTER_CASE(buffer_0, 1, JIANGTUN_SUCCESS, JIANGTUN_LEXER_NO_MATCH);
    /* Zero size */
    REGISTER_CASE(buffer_1, 0, JIANGTUN_SUCCESS, JIANGTUN_LEXER_NO_MATCH);
    /* No match */
    REGISTER_CASE(buffer_2, sizeof(buffer_2), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);

    /* NX Macro Controller */
    REGISTER_CASE(buffer_3, sizeof(buffer_3), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_4, sizeof(buffer_4), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_5, sizeof(buffer_5), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_6, sizeof(buffer_6), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_7, sizeof(buffer_7), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_8, sizeof(buffer_8), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_9, sizeof(buffer_9), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_10, sizeof(buffer_10), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_11, sizeof(buffer_11), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_12, sizeof(buffer_12), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_13, sizeof(buffer_13), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_NXMC2);
    REGISTER_CASE(buffer_14, sizeof(buffer_14), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);

    /* Poke-Controller */
    REGISTER_CASE(buffer_15, sizeof(buffer_15) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_16, sizeof(buffer_16) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_17, sizeof(buffer_17) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_18, sizeof(buffer_18) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_19, sizeof(buffer_19) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_20, sizeof(buffer_20) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_21, sizeof(buffer_21) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_22, sizeof(buffer_22) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_23, sizeof(buffer_23) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_24, sizeof(buffer_24) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_25, sizeof(buffer_25) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_26, sizeof(buffer_26) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_27, sizeof(buffer_27) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_28, sizeof(buffer_28) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);

    REGISTER_CASE(buffer_29, sizeof(buffer_29) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_30, sizeof(buffer_30) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_31, sizeof(buffer_31) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_32, sizeof(buffer_32) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_33, sizeof(buffer_33) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_34, sizeof(buffer_34) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);

    REGISTER_CASE(buffer_35, sizeof(buffer_35) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_36, sizeof(buffer_36) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_37, sizeof(buffer_37) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_38, sizeof(buffer_38) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_39, sizeof(buffer_39) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_40, sizeof(buffer_40) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_41, sizeof(buffer_41) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_42, sizeof(buffer_42) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_43, sizeof(buffer_43) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_44, sizeof(buffer_44) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_45, sizeof(buffer_45) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_46, sizeof(buffer_46) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_47, sizeof(buffer_47) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_48, sizeof(buffer_48) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_49, sizeof(buffer_49) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_50, sizeof(buffer_50) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_51, sizeof(buffer_51) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_52, sizeof(buffer_52) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_53, sizeof(buffer_53) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_54, sizeof(buffer_54) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_55, sizeof(buffer_55) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_56, sizeof(buffer_56) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_57, sizeof(buffer_57) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);

    REGISTER_CASE(buffer_58, sizeof(buffer_58) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_59, sizeof(buffer_59) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_60, sizeof(buffer_60) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_61, sizeof(buffer_61) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_62, sizeof(buffer_62) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_63, sizeof(buffer_63) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);

    REGISTER_CASE(buffer_64, sizeof(buffer_64) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);

    REGISTER_CASE(buffer_65, sizeof(buffer_65) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_66, sizeof(buffer_66) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_67, sizeof(buffer_67) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_68, sizeof(buffer_68) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_69, sizeof(buffer_69) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_70, sizeof(buffer_70) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_POKECON);
    REGISTER_CASE(buffer_71, sizeof(buffer_71) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);

    REGISTER_CASE(buffer_72, sizeof(buffer_72), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_73, sizeof(buffer_73), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_74, sizeof(buffer_74), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_ORCA);
    REGISTER_CASE(buffer_75, sizeof(buffer_75), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);
    REGISTER_CASE(buffer_76, sizeof(buffer_76), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_77, sizeof(buffer_77), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_78, sizeof(buffer_78), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_79, sizeof(buffer_79), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_80, sizeof(buffer_80), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_81, sizeof(buffer_81), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_PARTIAL);
    REGISTER_CASE(buffer_82, sizeof(buffer_82), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_ORCA);
    REGISTER_CASE(buffer_83, sizeof(buffer_83), JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_NO_MATCH);
    REGISTER_CASE(buffer_84, sizeof(buffer_84) - 1, JIANGTUN_SUCCESS,
                  JIANGTUN_LEXER_MATCH_ORCA);

    for (i = 0; i < test_cases_length; i++) {
        test_case_t *test_case = &(test_cases[i]);
        jiangtun_lexer_t lexer;
        jiangtun_error_t actual_err;
        jiangtun_lexer_result_t actual_ret;

        TEST_ASSERT(JIANGTUN_SUCCESS == jiangtun_lexer_init(&lexer));
        TEST_ASSERT(
            test_case->expected_err ==
            (actual_err = jiangtun_lex(&lexer, test_case->buffer,
                                       test_case->buffer_length, &actual_ret)));
        if (actual_err == JIANGTUN_SUCCESS) {
            TEST_ASSERT(test_case->expected_ret == actual_ret);
        }
        TEST_ASSERT(JIANGTUN_SUCCESS == jiangtun_lexer_deinit(&lexer));
    }

    return 0;
}
