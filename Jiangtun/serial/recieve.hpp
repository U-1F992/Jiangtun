#ifndef JIANGTUN_SERIAL_RECIEVE_H_
#define JIANGTUN_SERIAL_RECIEVE_H_

#include "../nxmc/nxmc.hpp"

inline void WarningIncompletePacket_(Logger &logger, const Packet packet, const size_t length)
{
    char message[60] = "Recieve incomplete packet:";
    for (int i = 0; i < length; i++)
    {
        char hex[4];
        sprintf(hex, " %02X", packet.raw[i]);
        strcat(message, hex);
    }
    logger.Warning(message);
}

inline void WarningInvalidPacket_(Logger &logger, const Packet packet)
{
    char message[57];
    sprintf(
        message, "Recieve invalid packet: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
        packet.raw[0], packet.raw[1], packet.raw[2], packet.raw[3], packet.raw[4], packet.raw[5], packet.raw[6], packet.raw[7], packet.raw[8], packet.raw[9], packet.raw[10]);
    logger.Warning(message);
}

void Recieve(Packet &packet, Logger &logger)
{
    if (Serial.available() == 0)
    {
        return;
    }

    size_t packet_length = sizeof(packet.raw);
    size_t length = Serial.readBytes(packet.raw, packet_length);
    if (length != packet_length)
    {
        WarningIncompletePacket_(logger, packet, length);
        return;
    }
    else if (!is_valid(packet))
    {
        WarningInvalidPacket_(logger, packet);
        return;
    }
}

#endif // JIANGTUN_SERIAL_RECIEVE_H_
