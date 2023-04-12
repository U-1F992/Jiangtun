#include <cstdio>
#include <cstring>

#include "plog/Log.h"

#include "nxmc.h"

namespace nxmc
{
    std::string FormatButton_(const Button value, const std::string name)
    {
        if (value == Button::Pressed)
        {
            return name + " ";
        }
        else
        {
            return "";
        }
    }

    std::string FormatButtons_(const Packet &p)
    {
        char buf[58];
        snprintf(
            buf, 58,
            " %s%s%s%s%s%s%s%s%s%s%s%s%s%s",
            FormatButton_(p.y, "y").c_str(),
            FormatButton_(p.b, "b").c_str(),
            FormatButton_(p.a, "a").c_str(),
            FormatButton_(p.x, "x").c_str(),
            FormatButton_(p.l, "l").c_str(),
            FormatButton_(p.r, "r").c_str(),
            FormatButton_(p.zl, "zl").c_str(),
            FormatButton_(p.zr, "zr").c_str(),
            FormatButton_(p.plus, "plus").c_str(),
            FormatButton_(p.minus, "minus").c_str(),
            FormatButton_(p.lclick, "lclick").c_str(),
            FormatButton_(p.rclick, "rclick").c_str(),
            FormatButton_(p.home, "home").c_str(),
            FormatButton_(p.capture, "capture").c_str());
        return std::string(buf);
    }

    std::string FormatHat_(const Hat value)
    {
        switch (value)
        {
        case Hat::Up:
            return "up";
        case Hat::UpRight:
            return "up_right";
        case Hat::Right:
            return "right";
        case Hat::DownRight:
            return "down_right";
        case Hat::Down:
            return "down";
        case Hat::DownLeft:
            return "down_left";
        case Hat::Left:
            return "left";
        case Hat::UpLeft:
            return "up_left";
        case Hat::Neutral:
            return "neutral";
        default:
            return "";
        }
    }

    std::string ToString_(const Packet &p)
    {
        char buf[135];
        snprintf(
            buf, 135,
            "{ btns:[%s] hat:%s l:(%d,%d) r:(%d,%d) ext0:%d ext1:%d ext2:%d }",
            FormatButtons_(p).c_str(), FormatHat_(p.hat).c_str(), p.lx, p.ly, p.rx, p.ry, p.ext0, p.ext1, p.ext2);
        return std::string(buf);
    }

    auto HeaderInvalid_(uint8_t b)
    {
        char msg[26];
        snprintf(msg, 26, "header must be 0xAB: 0x%02X", b);
        return etl::unexpected<std::string>(std::string(msg));
    }

    auto ButtonsOutOfRange_(uint8_t b)
    {
        char msg[40];
        snprintf(msg, 40, "upper 2 bits of buttons must be 0: 0x%02X", b);
        return etl::unexpected<std::string>(std::string(msg));
    }

    auto HatOutOfRange_(uint8_t b)
    {
        char msg[26];
        snprintf(msg, 26, "hat must be within 8: %d", b);
        return etl::unexpected<std::string>(std::string(msg));
    }

    auto PacketNotYetComplete_(const uint8_t *buf, const int pos)
    {
        char full[55];
        size_t len = 28 + 3 * pos;
        char msg[len];
        snprintf(
            full, 55,
            "packet not yet complete: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
            buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8], buf[9]);
        strncpy(msg, full, len);
        msg[len - 1] = '\0';
        return etl::unexpected<std::string>(std::string(msg));
    }

    constexpr int kTimeout = 100;

    std::function<etl::expected<nxmc::Packet, std::string>()> PacketReceiver(const std::function<etl::expected<uint8_t, std::string>()> &Read)
    {
        uint8_t buf[11] = {0};
        int pos = 0;
        int pended = 0;

        return [Read, buf, pos, pended]() mutable
        {
            typedef etl::expected<Packet, std::string> _; // Unlike STL, ETL does not allow implicit conversion from TValue or unexpected<TError> to expected<TValue>.
            
            auto eb = Read();
            if (!eb.has_value())
            {
                if (pos != 0)
                {
                    pended++;
                    if (kTimeout < pended)
                    {
                        pos = 0;
                        pended = 0;
                        return _(etl::unexpected<std::string>("(Timeout) " + eb.error()));
                    }
                }
                return _(etl::unexpected<std::string>(eb.error()));
            }
            uint8_t b = eb.value();

            if (pos == 0 && b != kHeader)
            {
                pos = 0;
                return _(HeaderInvalid_(b));
            }
            if (pos == 2 && 0b00111111 < b)
            {
                pos = 0;
                return _(ButtonsOutOfRange_(b));
            }
            if (pos == 3 && static_cast<uint8_t>(Hat::Neutral) < b)
            {
                pos = 0;
                return _(HatOutOfRange_(b));
            }

            buf[pos] = b;

            if (pos != 10)
            {
                auto msg = PacketNotYetComplete_(buf, pos);
                pos++;
                return _(msg);
            }

            Packet p;
            memcpy(p.raw, buf, 11);
            pos = 0;
            PLOGD << ToString_(p);
            return _(p);
        };
    }
}