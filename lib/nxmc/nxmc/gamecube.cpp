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
}

#endif
