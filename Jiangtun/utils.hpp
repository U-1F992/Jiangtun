#ifndef JIANGTUN_UTILS_H_
#define JIANGTUN_UTILS_H_

#include "Nintendo.h"

#include "logger/logger.hpp"
#include "nxmc/nxmc.hpp"

inline void Initialize(CGamecubeConsole &console, Gamecube_Data_t &data)
{
    data.report.a = 0;
    data.report.b = 0;
    data.report.x = 0;
    data.report.y = 0;
    data.report.start = 0;
    data.report.dleft = 0;
    data.report.dright = 0;
    data.report.ddown = 0;
    data.report.dup = 0;
    data.report.z = 0;
    data.report.r = 0;
    data.report.l = 0;
    data.report.xAxis = 128;
    data.report.yAxis = 128;
    data.report.cxAxis = 128;
    data.report.cyAxis = 128;
    data.report.left = 0;
    data.report.right = 0;

    // Omajinai to recognize the controller
    data.report.start = 1;
    console.write(data);
    data.report.start = 0;
    console.write(data);
}

inline void WarningNotConnected(Logger &logger)
{
    logger.Warning("GameCube doesn't seem to be connected");
}

#ifdef DEBUG

inline void DebugReport_(Logger &logger, const Gamecube_Report_t report)
{
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
    logger.Debug(buffer);
}

inline void DebugPacket_(Logger &logger, const Packet packet)
{
    char buffer[256];
    sprintf(
        buffer,
        "{\"header\":%d,\"y\":%d,\"b\":%d,\"a\":%d,\"x\":%d,\"l\":%d,\"r\":%d,\"zl\":%d,\"zr\":%d,\"plus\":%d,\"minus\":%d,\"lclick\":%d,\"rclick\":%d,\"home\":%d,\"capture\":%d,\"hat\":%d,\"lx\":%d,\"ly\":%d,\"rx\":%d,\"ry\":%d,\"ext0\":%d,\"ext1\":%d,\"ext2\":%d}",
        packet.header,
        packet.y,
        packet.b,
        packet.a,
        packet.x,
        packet.l,
        packet.r,
        packet.zl,
        packet.zr,
        packet.plus,
        packet.minus,
        packet.lclick,
        packet.rclick,
        packet.home,
        packet.capture,
        packet.hat,
        packet.lx,
        packet.ly,
        packet.rx,
        packet.ry,
        packet.ext0,
        packet.ext1,
        packet.ext2);
    logger.Debug(buffer);
}

uint8_t previous_packet_[11];
uint8_t previous_report_[8];

inline void DebugPrint(Logger &logger, Packet packet, Gamecube_Data_t data)
{
    if (memcmp(previous_packet_, packet.raw, sizeof(packet.raw)) != 0)
    {
        DebugPacket_(logger, packet);
        memcpy(previous_packet_, packet.raw, sizeof(packet.raw));
    }
    if (memcmp(previous_report_, data.report.raw8, sizeof(data.report.raw8)) != 0)
    {
        DebugReport_(logger, data.report);
        memcpy(previous_report_, data.report.raw8, sizeof(data.report.raw8));
    }
}

#endif

#endif // JIANGTUN_UTILS_H_
