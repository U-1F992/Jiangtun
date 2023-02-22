#ifndef JIANGTUN_GAMECUBE_REPORT_H_
#define JIANGTUN_GAMECUBE_REPORT_H_

#include "Nintendo.h"

inline void initialize(Gamecube_Report_t &gc_report)
{
    gc_report.a = 0;
    gc_report.b = 0;
    gc_report.x = 0;
    gc_report.y = 0;
    gc_report.start = 0;
    gc_report.dleft = 0;
    gc_report.dright = 0;
    gc_report.ddown = 0;
    gc_report.dup = 0;
    gc_report.z = 0;
    gc_report.r = 0;
    gc_report.l = 0;
    gc_report.xAxis = 128;
    gc_report.yAxis = 128;
    gc_report.cxAxis = 128;
    gc_report.cyAxis = 128;
    gc_report.left = 0;
    gc_report.right = 0;
}

uint8_t previous_[8];

inline void print(const Gamecube_Report_t &report)
{
    if (memcmp(previous_, report.raw8, sizeof(previous_)) == 0)
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
    memcpy(previous_, report.raw8, sizeof(previous_));
}

#endif // JIANGTUN_GAMECUBE_REPORT_H_
