#ifndef NXMC_ARDUINO_H_
#define NXMC_ARDUINO_H_
#ifdef ARDUINO

#include <Arduino.h>

#include "nxmc.h"
#include "nxmc/helper.h"

namespace nxmc::arduino
{
    inline void Println(const std::string &s)
    {
        Serial.println(s.c_str());
    }
    inline ExpectRawPacket TryReadBytes()
    {
        if (Serial.available() == 0)
        {
            return ExpectRawPacket(UnexpectedReason("no bytes to read"));
        }

        uint8_t raw[kRawLength];
        int length_read = Serial.readBytes(raw, kRawLength);
        if (length_read != kRawLength)
        {
            std::string message = "incomplete packet:";
            for (int i = 0; i < length_read; i++)
            {
                char hex[4];
                std::sprintf(hex, " %02X", raw[i]);
                message.append(hex);
            }
            return ExpectRawPacket(UnexpectedReason(message));
        }

        RawPacket ret;
        std::copy(std::begin(raw), std::end(raw), ret.begin());
        return ret;
    }
}

#endif
#endif // NXMC_ARDUINO_H_
