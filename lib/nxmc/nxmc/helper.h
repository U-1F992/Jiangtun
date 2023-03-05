#ifndef NXMC_HELPER_H_
#define NXMC_HELPER_H_

#include "nxmc.h"

namespace nxmc::helper
{
    inline LogCallback DefineLog(
        const std::function<void(const std::string &)> &write,
        Severity lowest_to_write)
    {
        return [lowest_to_write, write](Severity severity, const std::string &message)
        {
            if (severity < lowest_to_write)
            {
                return;
            }
            std::string buffer;
            switch (severity)
            {
            case Severity::Debug:
                buffer = "DEBUG";
                break;
            case Severity::Info:
                buffer = "INFO";
                break;
            case Severity::Warning:
                buffer = "WARNING";
                break;
            case Severity::Error:
                buffer = "ERROR";
                break;
            default:
                return;
            }
            write(buffer.append("\t").append(message));
        };
    }
    
    inline RetrieveCallback DefineRetrieve(
        const std::function<ExpectRawPacket()> &read)
    {
        return [read](const LogCallback &log)
        {
            auto raw = read();
            if (raw.has_value())
            {
                return Packet::From(raw.value());
            }
            else
            {
                return ExpectPacket(UnexpectedReason(raw.error()));
            }
        };
    }
}

#endif // NXMC_HELPER_H_
