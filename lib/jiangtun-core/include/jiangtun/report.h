#ifndef JIANGTUN_REPORT_H
#define JIANGTUN_REPORT_H

#include "compat.h"
#include "error.h"

/*
 * These codes actually depend on the PODs defined in `Bluewhale.h`.
 */
#ifdef ARDUINO
#include <Bluewhale.h>
#if __cplusplus >= 201103L
#include <type_traits>
static_assert(std::is_trivially_copyable<Gamecube_Report_t>::value,
              "Gamecube_Report_t must be a POD type");
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
/**
 * Equivalent data structure to `Bluewhale::Gamecube_Report_t` at the source
 * code level, for ease of testing from pure C code. Please take a diff and
 * update this if Bluewhale is changed.
 * 
 * `$ diff -u --minimal <(sed -n '31,217p' include/jiangtun/report.h) <(sed -n '48,234p' lib/Bluewhale/src/Gamecube.h)`
 */
typedef union jiangtun_report_t {
    /* 8 bytes of datareport that we get from the controller */
    jiangtun_uint8_t raw8[8];
    /* jiangtun_uint16_t raw16[0]; - ISO C forbids zero-size array */
    /* jiangtun_uint32_t raw32[0]; - ISO C forbids zero-size array */

    struct {
        jiangtun_uint8_t buttons0;
        union {
            jiangtun_uint8_t buttons1;
            jiangtun_uint8_t dpad/* : 4 - type of bit-field ~ is a GCC extension*/;
        } unnamed_0; /* - ISO C90 doesn't support unnamed structs/unions */
    } unnamed_1;

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t a/* : 1 */;
        jiangtun_uint8_t b/* : 1 */;
        jiangtun_uint8_t x/* : 1 */;
        jiangtun_uint8_t y/* : 1 */;
        jiangtun_uint8_t start/* : 1 */;
        jiangtun_uint8_t origin/* : 1 */; /* Indicates if GetOrigin(0x41) was called (LOW) */
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        /* second data byte */
        jiangtun_uint8_t dleft/* : 1 */;
        jiangtun_uint8_t dright/* : 1 */;
        jiangtun_uint8_t ddown/* : 1 */;
        jiangtun_uint8_t dup/* : 1 */;
        jiangtun_uint8_t z/* : 1 */;
        jiangtun_uint8_t r/* : 1 */;
        jiangtun_uint8_t l/* : 1 */;
        jiangtun_uint8_t high1/* : 1 */;

        /* 3rd-8th data byte */
        jiangtun_uint8_t xAxis;
        jiangtun_uint8_t yAxis;
        jiangtun_uint8_t cxAxis;
        jiangtun_uint8_t cyAxis;
        jiangtun_uint8_t left;
        jiangtun_uint8_t right;
    } mode3; /* mode3 (default reading mode) */

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t a/* : 1 */;
        jiangtun_uint8_t b/* : 1 */;
        jiangtun_uint8_t x/* : 1 */;
        jiangtun_uint8_t y/* : 1 */;
        jiangtun_uint8_t start/* : 1 */;
        jiangtun_uint8_t origin/* : 1 */; /* Indicates if GetOrigin(0x41) was called (LOW) */
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        /* second data byte */
        jiangtun_uint8_t dleft/* : 1 */;
        jiangtun_uint8_t dright/* : 1 */;
        jiangtun_uint8_t ddown/* : 1 */;
        jiangtun_uint8_t dup/* : 1 */;
        jiangtun_uint8_t z/* : 1 */;
        jiangtun_uint8_t r/* : 1 */;
        jiangtun_uint8_t l/* : 1 */;
        jiangtun_uint8_t high1/* : 1 */;

        /* 3rd-8th data byte */
        jiangtun_uint8_t xAxis;
        jiangtun_uint8_t yAxis;
        jiangtun_uint8_t cxAxis;
        jiangtun_uint8_t cyAxis;
        jiangtun_uint8_t right/* : 4 */; /* note: within bytes, LSB is first */
        jiangtun_uint8_t left/* : 4 */;
        jiangtun_uint8_t analogB/* : 4 */;
        jiangtun_uint8_t analogA/* : 4 */;
    } mode0;

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t a/* : 1 */;
        jiangtun_uint8_t b/* : 1 */;
        jiangtun_uint8_t x/* : 1 */;
        jiangtun_uint8_t y/* : 1 */;
        jiangtun_uint8_t start/* : 1 */;
        jiangtun_uint8_t origin/* : 1 */; /* Indicates if GetOrigin(0x41) was called (LOW) */
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        /* second data byte */
        jiangtun_uint8_t dleft/* : 1 */;
        jiangtun_uint8_t dright/* : 1 */;
        jiangtun_uint8_t ddown/* : 1 */;
        jiangtun_uint8_t dup/* : 1 */;
        jiangtun_uint8_t z/* : 1 */;
        jiangtun_uint8_t r/* : 1 */;
        jiangtun_uint8_t l/* : 1 */;
        jiangtun_uint8_t high1/* : 1 */;

        /* 3rd-8th data byte */
        jiangtun_uint8_t xAxis;
        jiangtun_uint8_t yAxis;
        jiangtun_uint8_t cyAxis/* : 4 */;
        jiangtun_uint8_t cxAxis/* : 4 */;
        jiangtun_uint8_t left;
        jiangtun_uint8_t right;
        jiangtun_uint8_t analogB/* : 4 */;
        jiangtun_uint8_t analogA/* : 4 */;
    } mode1;

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t a/* : 1 */;
        jiangtun_uint8_t b/* : 1 */;
        jiangtun_uint8_t x/* : 1 */;
        jiangtun_uint8_t y/* : 1 */;
        jiangtun_uint8_t start/* : 1 */;
        jiangtun_uint8_t origin/* : 1 */; /* Indicates if GetOrigin(0x41) was called (LOW) */
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        /* second data byte */
        jiangtun_uint8_t dleft/* : 1 */;
        jiangtun_uint8_t dright/* : 1 */;
        jiangtun_uint8_t ddown/* : 1 */;
        jiangtun_uint8_t dup/* : 1 */;
        jiangtun_uint8_t z/* : 1 */;
        jiangtun_uint8_t r/* : 1 */;
        jiangtun_uint8_t l/* : 1 */;
        jiangtun_uint8_t high1/* : 1 */;

        /* 3rd-8th data byte */
        jiangtun_uint8_t xAxis;
        jiangtun_uint8_t yAxis;
        jiangtun_uint8_t cyAxis/* : 4 */;
        jiangtun_uint8_t cxAxis/* : 4 */;
        jiangtun_uint8_t right/* : 4 */;
        jiangtun_uint8_t left/* : 4 */;
        jiangtun_uint8_t analogA;
        jiangtun_uint8_t analogB;
    } mode2;

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t a/* : 1 */;
        jiangtun_uint8_t b/* : 1 */;
        jiangtun_uint8_t x/* : 1 */;
        jiangtun_uint8_t y/* : 1 */;
        jiangtun_uint8_t start/* : 1 */;
        jiangtun_uint8_t origin/* : 1 */; /* Indicates if GetOrigin(0x41) was called (LOW) */
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        /* second data byte */
        jiangtun_uint8_t dleft/* : 1 */;
        jiangtun_uint8_t dright/* : 1 */;
        jiangtun_uint8_t ddown/* : 1 */;
        jiangtun_uint8_t dup/* : 1 */;
        jiangtun_uint8_t z/* : 1 */;
        jiangtun_uint8_t r/* : 1 */;
        jiangtun_uint8_t l/* : 1 */;
        jiangtun_uint8_t high1/* : 1 */;

        /* 3rd-8th data byte */
        jiangtun_uint8_t xAxis;
        jiangtun_uint8_t yAxis;
        jiangtun_uint8_t cxAxis;
        jiangtun_uint8_t cyAxis;
        jiangtun_uint8_t analogA;
        jiangtun_uint8_t analogB;
    } mode4;

    struct {
        /* first data byte (bitfields are sorted in LSB order) */
        jiangtun_uint8_t unknown_counter1/* : 4 */;
        jiangtun_uint8_t unknown1/* : 2 */;
        jiangtun_uint8_t errlatch/* : 1 */;
        jiangtun_uint8_t errstat/* : 1 */;

        jiangtun_uint16_t unknown2/* : 16 */;
        jiangtun_uint8_t unknown3;

        jiangtun_uint8_t keypress[3];

        jiangtun_uint8_t unknown_counter2/* : 4 */;
        jiangtun_uint8_t unknown4/* : 4 */;
    } keyboard;

} jiangtun_report_t;
/* clang-format on */

#define JIANGTUN_REPORT_STICK_NEUTRAL 128

jiangtun_error_t jiangtun_report_init(jiangtun_report_t *);
/**
 * Adjust the axis values to fall within the range of 1 to 254 to ensure proper
 * recognition by the games.
 *
 * The theoretical upper and lower limits (0 and 255) of the GameCube
 * controller's axis values do not usually occur in actual input. As a result,
 * some games may not handle these values correctly.
 */
jiangtun_error_t jiangtun_report_emend_axis(jiangtun_report_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_REPORT_H */