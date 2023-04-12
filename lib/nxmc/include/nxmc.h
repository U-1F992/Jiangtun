#ifndef NXMC_NXMC_H_
#define NXMC_NXMC_H_

#include <cstdint>
#include <functional>

#include <etl/expected.h>

namespace nxmc
{
    enum class Button : uint8_t
    {
        Released,
        Pressed
    };

    enum class Hat : uint8_t
    {
        Up,
        UpRight,
        Right,
        DownRight,
        Down,
        DownLeft,
        Left,
        UpLeft,
        Neutral
    };

    typedef union
    {
        uint8_t raw[11];
        struct
        {
            uint8_t header;

            Button y : 1;
            Button b : 1;
            Button a : 1;
            Button x : 1;
            Button l : 1;
            Button r : 1;
            Button zl : 1;
            Button zr : 1;

            Button minus : 1;
            Button plus : 1;
            Button lclick : 1;
            Button rclick : 1;
            Button home : 1;
            Button capture : 1;
            uint8_t : 2;

            Hat hat;
            uint8_t lx;
            uint8_t ly;
            uint8_t rx;
            uint8_t ry;

            uint8_t ext0;
            uint8_t ext1;
            uint8_t ext2;
        };
    } Packet;

    constexpr uint8_t kHeader = 0xAB;

    std::function<etl::expected<nxmc::Packet, std::string>()> PacketReceiver(const std::function<etl::expected<uint8_t, std::string>()> &);
}

#endif // NXMC_NXMC_H_
