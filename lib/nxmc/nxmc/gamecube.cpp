#ifdef ARDUINO
#include "gamecube.h"

namespace nxmc::gamecube
{
    inline void ApplyButtons(Gamecube_Data_t &data, const Packet &packet)
    {
        data.report.y = static_cast<uint8_t>(packet.GetY());
        data.report.b = static_cast<uint8_t>(packet.GetB());
        data.report.a = static_cast<uint8_t>(packet.GetA());
        data.report.x = static_cast<uint8_t>(packet.GetX());
        data.report.l = static_cast<uint8_t>(packet.GetL());
        data.report.r = static_cast<uint8_t>(packet.GetR());
        data.report.z = static_cast<uint8_t>(packet.GetZR());
        data.report.start = static_cast<uint8_t>(packet.GetPlus());
    }
    inline void ApplyHat(Gamecube_Data_t &data, const Packet &packet)
    {
        switch (packet.GetHat())
        {
        case Hat::Up:
            data.report.dup = 1;
            data.report.dright = 0;
            data.report.ddown = 0;
            data.report.dleft = 0;
            break;

        case Hat::UpRight:
            data.report.dup = 1;
            data.report.dright = 1;
            data.report.ddown = 0;
            data.report.dleft = 0;
            break;

        case Hat::Right:
            data.report.dup = 0;
            data.report.dright = 1;
            data.report.ddown = 0;
            data.report.dleft = 0;
            break;

        case Hat::DownRight:
            data.report.dup = 0;
            data.report.dright = 1;
            data.report.ddown = 1;
            data.report.dleft = 0;
            break;

        case Hat::Down:
            data.report.dup = 0;
            data.report.dright = 0;
            data.report.ddown = 1;
            data.report.dleft = 0;
            break;

        case Hat::DownLeft:
            data.report.dup = 0;
            data.report.dright = 0;
            data.report.ddown = 1;
            data.report.dleft = 1;
            break;

        case Hat::Left:
            data.report.dup = 0;
            data.report.dright = 0;
            data.report.ddown = 0;
            data.report.dleft = 1;
            break;

        case Hat::UpLeft:
            data.report.dup = 1;
            data.report.dright = 0;
            data.report.ddown = 0;
            data.report.dleft = 1;
            break;

        case Hat::Neutral:
            data.report.dup = 0;
            data.report.dright = 0;
            data.report.ddown = 0;
            data.report.dleft = 0;
            break;
        }
    }
    inline void ApplySticks(Gamecube_Data_t &data, const Packet &packet)
    {
        data.report.xAxis = packet.GetLX();
        data.report.yAxis = 0xFF - packet.GetLY();
        data.report.cxAxis = packet.GetRX();
        data.report.cyAxis = 0xFF - packet.GetRY();
    }
    void Apply(Gamecube_Data_t &data, const Packet &packet)
    {
        ApplyButtons(data, packet);
        ApplyHat(data, packet);
        ApplySticks(data, packet);
    }
    std::string ToJSONString(const Gamecube_Data_t &data)
    {
        char buffer[244]; // each %d will be within 3 chars, 0~255
        snprintf(
            buffer, sizeof(buffer) / sizeof(char),
            "{\"a\":%d,\"b\":%d,\"x\":%d,\"y\":%d,\"start\":%d,\"origin\":%d,\"errlatch\":%d,\"errstat\":%d,\"dleft\":%d,\"dright\":%d,\"ddown\":%d,\"dup\":%d,\"z\":%d,\"r\":%d,\"l\":%d,\"high1\":%d,\"xAxis\":%d,\"yAxis\":%d,\"cxAxis\":%d,\"cyAxis\":%d,\"left\":%d,\"right\":%d}",
            data.report.a, data.report.b, data.report.x, data.report.y, data.report.start, data.report.origin, data.report.errlatch, data.report.errstat, data.report.dleft, data.report.dright, data.report.ddown, data.report.dup, data.report.z, data.report.r, data.report.l, data.report.high1, data.report.xAxis, data.report.yAxis, data.report.cxAxis, data.report.cyAxis, data.report.left, data.report.right);
        return std::string(buffer);
    }
}

#endif
