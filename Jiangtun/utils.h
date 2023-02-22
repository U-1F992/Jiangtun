#ifndef JIANGTUN_UTILS_H_
#define JIANGTUN_UTILS_H_

#include "nxmc_packet.h"

inline void print_incomplete_data_error_(const NXMCPacket &nxmc_packet, const size_t length)
{
    char message[] = "Incomplete data:";
    for (int i = 0; i < length; i++)
    {
        char tmp[2];
        sprintf(tmp, " %02X", nxmc_packet.raw[i]);
        strcat(message, tmp);
    }
    Serial.println(message);
}

inline void print_invalid_data_error_(const NXMCPacket &nxmc_packet)
{
    char message[46];
    sprintf(
        message, "Invalid data: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
        nxmc_packet.raw[0], nxmc_packet.raw[1], nxmc_packet.raw[2], nxmc_packet.raw[3], nxmc_packet.raw[4], nxmc_packet.raw[5], nxmc_packet.raw[6], nxmc_packet.raw[7], nxmc_packet.raw[8], nxmc_packet.raw[9], nxmc_packet.raw[10]);
    Serial.println(message);
}

inline void print_write_error_()
{
    Serial.println("Write error: GameCube doesn't seem to be connected");
}

inline void read(NXMCPacket &nxmc_packet)
{
    size_t length = Serial.readBytes(nxmc_packet.raw, PACKET_LENGTH);
    if (length != PACKET_LENGTH)
    {
        print_incomplete_data_error_(nxmc_packet, length);
    }
}

inline void convert(NXMCPacket &nxmc_packet, Gamecube_Data_t &gc_data)
{
    if (!to_gamecube_report(nxmc_packet, gc_data.report))
    {
        print_invalid_data_error_(nxmc_packet);
    }

#ifdef DEBUG
    else
    {
        print(nxmc_packet);
        print(gc_data.report);
    }
#endif
}

inline void write(CGamecubeConsole &console, Gamecube_Data_t &data)
{
    if (!console.write(data))
    {
        print_write_error_();
    }
}

#endif // JIANGTUN_UTILS_H_
