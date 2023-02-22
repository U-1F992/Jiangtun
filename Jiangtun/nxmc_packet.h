#ifndef JIANGTUN_NXMC_PACKET_H_
#define JIANGTUN_NXMC_PACKET_H_

#include "Nintendo.h"

#define PACKET_LENGTH (11)
#define HEADER_ (0xAB)
#define BUTTONS_UPPER_INDEX_ (2)
#define BUTTONS_UPPER_ALL_ (0b00111111)

enum Hat_
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
    uint8_t raw[PACKET_LENGTH];
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
} NXMCPacket;

inline bool is_valid_(const NXMCPacket &nxmc_packet)
{
    bool is_valid_header = nxmc_packet.header == HEADER_;
    bool is_valid_buttons_upper = nxmc_packet.raw[BUTTONS_UPPER_INDEX_] <= BUTTONS_UPPER_ALL_;
    bool is_valid_hat = nxmc_packet.hat <= NEUTRAL;
    return is_valid_header && is_valid_buttons_upper && is_valid_hat;
}

inline void buttons_(const NXMCPacket &nxmc_packet, Gamecube_Report_t &gc_report)
{
    gc_report.y = nxmc_packet.y;
    gc_report.b = nxmc_packet.b;
    gc_report.a = nxmc_packet.a;
    gc_report.x = nxmc_packet.x;
    gc_report.l = nxmc_packet.l;
    gc_report.r = nxmc_packet.r;
    gc_report.z = nxmc_packet.zr;
    gc_report.start = nxmc_packet.plus;
}

inline void hat_(const NXMCPacket &nxmc_packet, Gamecube_Report_t &gc_report)
{
    switch (nxmc_packet.hat)
    {
    case UP:
        gc_report.dup = 1;
        gc_report.dright = 0;
        gc_report.ddown = 0;
        gc_report.dleft = 0;
        break;

    case UP_RIGHT:
        gc_report.dup = 1;
        gc_report.dright = 1;
        gc_report.ddown = 0;
        gc_report.dleft = 0;
        break;

    case RIGHT:
        gc_report.dup = 0;
        gc_report.dright = 1;
        gc_report.ddown = 0;
        gc_report.dleft = 0;
        break;

    case DOWN_RIGHT:
        gc_report.dup = 0;
        gc_report.dright = 1;
        gc_report.ddown = 1;
        gc_report.dleft = 0;
        break;

    case DOWN:
        gc_report.dup = 0;
        gc_report.dright = 0;
        gc_report.ddown = 1;
        gc_report.dleft = 0;
        break;

    case DOWN_LEFT:
        gc_report.dup = 0;
        gc_report.dright = 0;
        gc_report.ddown = 1;
        gc_report.dleft = 1;
        break;

    case LEFT:
        gc_report.dup = 0;
        gc_report.dright = 0;
        gc_report.ddown = 0;
        gc_report.dleft = 1;
        break;

    case UP_LEFT:
        gc_report.dup = 1;
        gc_report.dright = 0;
        gc_report.ddown = 0;
        gc_report.dleft = 1;
        break;

    case NEUTRAL:
        gc_report.dup = 0;
        gc_report.dright = 0;
        gc_report.ddown = 0;
        gc_report.dleft = 0;
        break;
    }
}

inline void axis_(const NXMCPacket &nxmc_packet, Gamecube_Report_t &gc_report)
{
    gc_report.xAxis = nxmc_packet.lx;
    gc_report.yAxis = 0xFF - nxmc_packet.ly;
    gc_report.cxAxis = nxmc_packet.rx;
    gc_report.cyAxis = 0xFF - nxmc_packet.ry;
}

inline bool to_gamecube_report(const NXMCPacket &nxmc_packet, Gamecube_Report_t &gc_report)
{
    if (!is_valid_(nxmc_packet))
    {
        return false;
    }
    else
    {
        buttons_(nxmc_packet, gc_report);
        hat_(nxmc_packet, gc_report);
        axis_(nxmc_packet, gc_report);
        return true;
    }
}

inline void print(const NXMCPacket &report)
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

#endif // JIANGTUN_NXMC_PACKET_H_
