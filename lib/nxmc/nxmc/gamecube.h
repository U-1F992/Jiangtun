#ifndef NXMC_GAMECUBE_H_
#define NXMC_GAMECUBE_H_

#include "Nintendo.h"

#include "nxmc.h"

namespace nxmc::gamecube
{
    std::function<etl::expected<void, std::string>(const etl::expected<nxmc::Packet, std::string> &)> PacketSender(const uint8_t, const std::function<void()> &);
}

#endif // NXMC_GAMECUBE_H_
