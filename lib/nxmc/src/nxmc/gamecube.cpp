#ifdef ARDUINO
#include "plog/Log.h"

#include "nxmc/gamecube.h"

namespace nxmc::gamecube
{
    void ApplyButtons_(const nxmc::Packet &p, Gamecube_Data_t &d)
    {
        d.report.y = static_cast<uint8_t>(p.y);
        d.report.b = static_cast<uint8_t>(p.b);
        d.report.a = static_cast<uint8_t>(p.a);
        d.report.x = static_cast<uint8_t>(p.x);
        d.report.l = static_cast<uint8_t>(p.l);
        d.report.r = static_cast<uint8_t>(p.r);
        d.report.z = static_cast<uint8_t>(p.zr);
        d.report.start = static_cast<uint8_t>(p.plus);
    }
    void ApplyHat_(const nxmc::Packet &p, Gamecube_Data_t &d)
    {
        switch (p.hat)
        {
        case Hat::Up:
            d.report.dup = 1;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;

        case Hat::UpRight:
            d.report.dup = 1;
            d.report.dright = 1;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;

        case Hat::Right:
            d.report.dup = 0;
            d.report.dright = 1;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;

        case Hat::DownRight:
            d.report.dup = 0;
            d.report.dright = 1;
            d.report.ddown = 1;
            d.report.dleft = 0;
            break;

        case Hat::Down:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 1;
            d.report.dleft = 0;
            break;

        case Hat::DownLeft:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 1;
            d.report.dleft = 1;
            break;

        case Hat::Left:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 1;
            break;

        case Hat::UpLeft:
            d.report.dup = 1;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 1;
            break;

        case Hat::Neutral:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;
        }
    }
    void ApplySticks_(const nxmc::Packet &p, Gamecube_Data_t &d)
    {
        d.report.xAxis = p.lx;
        d.report.yAxis = 0xFF - p.ly;
        d.report.cxAxis = p.rx;
        d.report.cyAxis = 0xFF - p.ry;
    }
    void Apply_(const nxmc::Packet &p, Gamecube_Data_t &d)
    {
        ApplyButtons_(p, d);
        ApplyHat_(p, d);
        ApplySticks_(p, d);
    }
    std::string ToString_(const Gamecube_Data_t &d)
    {
        char buf[170];
        snprintf(
            buf, 170,
            "{ a:%d b:%d x:%d y:%d start:%d origin:%d errlatch:%d errstat:%d dleft:%d dright:%d ddown:%d dup:%d z:%d r:%d l:%d high1:%d xAxis:%d yAxis:%d cxAxis:%d cyAxis:%d left:%d right:%d }",
            d.report.a, d.report.b, d.report.x, d.report.y, d.report.start, d.report.origin, d.report.errlatch, d.report.errstat, d.report.dleft, d.report.dright, d.report.ddown, d.report.dup, d.report.z, d.report.r, d.report.l, d.report.high1, d.report.xAxis, d.report.yAxis, d.report.cxAxis, d.report.cyAxis, d.report.left, d.report.right);
        return std::string(buf);
    }
    std::function<etl::expected<void, std::string>(const etl::expected<nxmc::Packet, std::string> &)> PacketSender(const uint8_t pin, const std::function<void(CGamecubeConsole &, Gamecube_Data_t &)> &PressReset, const std::function<void(CGamecubeConsole &, Gamecube_Data_t &)> &ReleaseReset)
    {
        auto console = CGamecubeConsole(pin);
        auto cache = defaultGamecubeData;

        // Omajinai to recognize the controller
        cache.report.start = 1;
        console.write(cache);
        cache.report.start = 0;
        console.write(cache);

        bool latch = false;

        return [PressReset, ReleaseReset, console, cache, latch](const etl::expected<nxmc::Packet, std::string> &p) mutable
        {
            typedef etl::expected<void, std::string> _;

            if (p.has_value())
            {
                Apply_(p.value(), cache);
                PLOGD << ToString_(cache);
            }

            if (!console.write(cache))
            {
                return _(etl::unexpected<std::string>("GameCube doesn't seem to be connected"));
            }
            if (!p.has_value())
            {
                return _();
            }

            if (p.value().home == Button::Pressed && !latch)
            {
                // Once Reset is fired, it should not be possible to Reset again until home is released.
                PLOGD << "press reset";
                PressReset(console, cache);
                latch = true;
            }
            else if (p.value().home == Button::Released && latch)
            {
                PLOGD << "release reset";
                ReleaseReset(console, cache);
                latch = false;
            }
            return _();
        };
    }
}
#endif
