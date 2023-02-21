#ifndef JIANGTUN_REPORT_H_
#define JIANGTUN_REPORT_H_

#include "Nintendo.h"

enum Hat
{
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT,
    NEUTRAL
};

typedef union
{
    uint8_t raw[11];
    struct
    {
        uint8_t header;

        uint8_t y : 1;
        uint8_t b : 1;
        uint8_t a : 1;
        uint8_t x : 1;
        uint8_t l : 1;
        uint8_t r : 1;
        uint8_t zl : 1;
        uint8_t zr : 1;

        uint8_t minus : 1;
        uint8_t plus : 1;
        uint8_t lclick : 1;
        uint8_t rclick : 1;
        uint8_t home : 1;
        uint8_t capture : 1;
        uint8_t unused_ : 2;

        uint8_t hat;
        uint8_t lx;
        uint8_t ly;
        uint8_t rx;
        uint8_t ry;
        uint8_t ext0;
        uint8_t ext1;
        uint8_t ext2;
    };
} NXReport;

inline void initialize(Gamecube_Report_t &report)
{
    report.a = 0;
    report.b = 0;
    report.x = 0;
    report.y = 0;
    report.start = 0;
    report.dleft = 0;
    report.dright = 0;
    report.ddown = 0;
    report.dup = 0;
    report.z = 0;
    report.r = 0;
    report.l = 0;
    report.xAxis = 128;
    report.yAxis = 128;
    report.cxAxis = 128;
    report.cyAxis = 128;
    report.left = 0;
    report.right = 0;
}

const uint8_t HEADER = 0xAB;
const uint8_t BUTTONS_UPPER_ALL = 0b00111111;

inline bool is_valid(const NXReport &nx_report, const int index)
{
    bool is_valid_header = index != 0 || nx_report.header == HEADER;
    bool is_valid_buttons_upper = index != 2 || nx_report.raw[index] <= BUTTONS_UPPER_ALL;
    bool is_valid_hat = index != 3 || nx_report.raw[index] <= NEUTRAL;
    return is_valid_header && is_valid_buttons_upper && is_valid_hat;
}

inline void buttons_(const NXReport &nx_report, Gamecube_Report_t &report)
{
    report.y = nx_report.y;
    report.b = nx_report.b;
    report.a = nx_report.a;
    report.x = nx_report.x;
    report.l = nx_report.l;
    report.r = nx_report.r;
    report.z = nx_report.zr;
    report.start = nx_report.plus;
}

inline void hat_(const NXReport &nx_report, Gamecube_Report_t &report)
{
    switch (nx_report.hat)
    {
    case UP:
        report.dup = 1;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case UP_RIGHT:
        report.dup = 1;
        report.dright = 1;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case RIGHT:
        report.dup = 0;
        report.dright = 1;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case DOWN_RIGHT:
        report.dup = 0;
        report.dright = 1;
        report.ddown = 1;
        report.dleft = 0;
        break;

    case DOWN:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 1;
        report.dleft = 0;
        break;

    case DOWN_LEFT:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 1;
        report.dleft = 1;
        break;

    case LEFT:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 1;
        break;

    case UP_LEFT:
        report.dup = 1;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 1;
        break;

    case NEUTRAL:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 0;
        break;
    }
}

inline void axis_(const NXReport &nx_report, Gamecube_Report_t &report)
{
    report.xAxis = nx_report.lx;
    report.yAxis = 0xFF - nx_report.ly;
    report.cxAxis = nx_report.rx;
    report.cyAxis = 0xFF - nx_report.ry;
}

inline void convert(const NXReport &nx_report, Gamecube_Report_t &report)
{
    buttons_(nx_report, report);
    hat_(nx_report, report);
    axis_(nx_report, report);
}

inline void pretty_print(NXReport &report)
{
    char buffer[256];
    sprintf(
        buffer,
        "{\"header\":%d,\"y\":%d,\"b\":%d,\"a\":%d,\"x\":%d,\"l\":%d,\"r\":%d,\"zl\":%d,\"zr\":%d,\"plus\":%d,\"minus\":%d,\"lclick\":%d,\"rclick\":%d,\"home\":%d,\"capture\":%d,\"hat\":%d,\"lx\":%d,\"ly\":%d,\"rx\":%d,\"ry\":%d,\"ext0\":%d,\"ext1\":%d,\"ext2\":%d}",
        report.header,
        report.y,
        report.b,
        report.a,
        report.x,
        report.l,
        report.r,
        report.zl,
        report.zr,
        report.plus,
        report.minus,
        report.lclick,
        report.rclick,
        report.home,
        report.capture,
        report.hat,
        report.lx,
        report.ly,
        report.rx,
        report.ry,
        report.ext0,
        report.ext1,
        report.ext2);
    Serial.println(buffer);
}

uint8_t previous[8];

inline void pretty_print(Gamecube_Report_t &report)
{
    if (memcmp(previous, report.raw8, sizeof(previous)) == 0)
    {
        return;
    }
    char buffer[256];
    sprintf(
        buffer,
        "{\"a\":%d,\"b\":%d,\"x\":%d,\"y\":%d,\"start\":%d,\"origin\":%d,\"errlatch\":%d,\"errstat\":%d,\"dleft\":%d,\"dright\":%d,\"ddown\":%d,\"dup\":%d,\"z\":%d,\"r\":%d,\"l\":%d,\"high1\":%d,\"xAxis\":%d,\"yAxis\":%d,\"cxAxis\":%d,\"cyAxis\":%d,\"left\":%d,\"right\":%d}",
        report.a,
        report.b,
        report.x,
        report.y,
        report.start,
        report.origin,
        report.errlatch,
        report.errstat,
        report.dleft,
        report.dright,
        report.ddown,
        report.dup,
        report.z,
        report.r,
        report.l,
        report.high1,
        report.xAxis,
        report.yAxis,
        report.cxAxis,
        report.cyAxis,
        report.left,
        report.right);
    Serial.println(buffer);
    memcpy(previous, report.raw8, sizeof(previous));
}

#endif // JIANGTUN_REPORT_H_
