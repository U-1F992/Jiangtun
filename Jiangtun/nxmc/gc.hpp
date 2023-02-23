#ifndef JIANGTUN_NXMC_GC_H_
#define JIANGTUN_NXMC_GC_H_

#include "Nintendo.h"
#include "nxmc.hpp"

inline void RemapButtons_(const Packet &packet, Gamecube_Report_t &report)
{
    report.y = packet.y;
    report.b = packet.b;
    report.a = packet.a;
    report.x = packet.x;
    report.l = packet.l;
    report.r = packet.r;
    report.z = packet.zr;
    report.start = packet.plus;
}

inline void RemapHat_(const Packet &packet, Gamecube_Report_t &report)
{
    switch (static_cast<Hat>(packet.hat))
    {
    case Hat::Up:
        report.dup = 1;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case Hat::UpRight:
        report.dup = 1;
        report.dright = 1;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case Hat::Right:
        report.dup = 0;
        report.dright = 1;
        report.ddown = 0;
        report.dleft = 0;
        break;

    case Hat::DownRight:
        report.dup = 0;
        report.dright = 1;
        report.ddown = 1;
        report.dleft = 0;
        break;

    case Hat::Down:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 1;
        report.dleft = 0;
        break;

    case Hat::DownLeft:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 1;
        report.dleft = 1;
        break;

    case Hat::Left:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 1;
        break;

    case Hat::UpLeft:
        report.dup = 1;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 1;
        break;

    case Hat::Neutral:
        report.dup = 0;
        report.dright = 0;
        report.ddown = 0;
        report.dleft = 0;
        break;
    }
}

inline void RemapAxis_(const Packet &packet, Gamecube_Report_t &report)
{
    report.xAxis = packet.lx;
    report.yAxis = 0xFF - packet.ly;
    report.cxAxis = packet.rx;
    report.cyAxis = 0xFF - packet.ry;
}

inline void ToReport(const Packet packet, Gamecube_Report_t &report)
{
    RemapButtons_(packet, report);
    RemapHat_(packet, report);
    RemapAxis_(packet, report);
}

#endif // JIANGTUN_NXMC_GC_H_
