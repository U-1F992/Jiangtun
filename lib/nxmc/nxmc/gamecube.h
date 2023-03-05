#ifndef NXMC_GAMECUBE_H_
#define NXMC_GAMECUBE_H_
#ifdef ARDUINO

#include "Nintendo.h"

#include "nxmc.h"
#include "nxmc/helper.h"

namespace nxmc::gamecube
{
    void Apply(Gamecube_Data_t &data, const Packet &packet);
    inline SendCallback DefineSendData(uint8_t pin)
    {
        auto console = CGamecubeConsole(pin);
        auto cache = defaultGamecubeData;

        // Omajinai to recognize the controller
        cache.report.start = 1;
        console.write(cache);
        cache.report.start = 0;
        console.write(cache);

        return [console, cache](NullablePacket packet, const LogCallback &log) mutable
        {
            if (packet.has_value())
            {
                Apply(cache, packet.value());
            }

            if (!console.write(cache))
            {
                return ExpectVoid(UnexpectedReason("packet is empty"));
            }
            else
            {
                return ExpectVoid(); // to suppress warning
            }
        };
    }
}

#endif
#endif // NXMC_GAMECUBE_H_
