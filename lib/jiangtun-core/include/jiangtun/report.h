#ifndef JIANGTUN_REPORT_H
#define JIANGTUN_REPORT_H

#include "compat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Equivalent data structure to `Gamecube_Report_t`
 * Be careful when updating Bluewhale.
 *
 * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/src/Gamecube.h#L62-L90
 */
typedef struct jiangtun_report_mode3_t {
    jiangtun_bool_t a;
    jiangtun_bool_t b;
    jiangtun_bool_t x;
    jiangtun_bool_t y;
    jiangtun_bool_t start;
    jiangtun_bool_t origin;
    jiangtun_bool_t errlatch;
    jiangtun_bool_t errstat;

    jiangtun_bool_t dleft;
    jiangtun_bool_t dright;
    jiangtun_bool_t ddown;
    jiangtun_bool_t dup;
    jiangtun_bool_t z;
    jiangtun_bool_t r;
    jiangtun_bool_t l;
    jiangtun_bool_t high1;

    jiangtun_uint8_t xAxis;
    jiangtun_uint8_t yAxis;
    jiangtun_uint8_t cxAxis;
    jiangtun_uint8_t cyAxis;
    jiangtun_uint8_t left;
    jiangtun_uint8_t right;

    jiangtun_bool_t reset;
} jiangtun_report_mode3_t;

#define JIANGTUN_REPORT_STICK_NEUTRAL 128
#define JIANGTUN_REPORT_STICK_DOWN 0
#define JIANGTUN_REPORT_STICK_UP 255
#define JIANGTUN_REPORT_STICK_LEFT 0
#define JIANGTUN_REPORT_STICK_RIGHT 255

/**
 * https://github.com/mizuyoukanao/Bluewhale/blob/c8413b9aab7248089c9144295796d97794a10c64/examples/WHALE/WHALE.ino#L17-L34
 */
void jiangtun_report_init(jiangtun_report_mode3_t *);

/**
 * Adjust the axis values to fall within the range of 1 to 254 to ensure proper
 * recognition by the games.
 *
 * The theoretical upper and lower limits (0 and 255) of the GameCube
 * controller's axis values do not usually occur in actual input. As a result,
 * some games may not handle these values correctly.
 */
void jiangtun_report_emend_axis(jiangtun_report_mode3_t *);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_REPORT_H */