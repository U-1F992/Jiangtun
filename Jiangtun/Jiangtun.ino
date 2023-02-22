// #define DEBUG

#include "Nintendo.h"

#include "nxmc_packet.h"
#include "gamecube_report.h"
#include "utils.h"

NXMCPacket nxmc_packet;

CGamecubeConsole gc_console(5);
Gamecube_Data_t gc_data = defaultGamecubeData;

void setup()
{
    initialize(gc_data.report);

    // Omajinai to recognize the controller
    gc_data.report.start = 1;
    gc_console.write(gc_data);
    gc_data.report.start = 0;
    gc_console.write(gc_data);

    Serial.setTimeout(100);
    Serial.begin(9600);
}

void loop()
{
    if (0 < Serial.available())
    {
        read(nxmc_packet);
        convert(nxmc_packet, gc_data);
    }
    write(gc_console, gc_data);
}
