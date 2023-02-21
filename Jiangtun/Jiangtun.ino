#include "Nintendo.h"
#include "utils.hpp"

// #define DEBUG

CGamecubeConsole gc_console(5);
Gamecube_Data_t gc_data = defaultGamecubeData;

NXReport nx_report;
int index = 0;
int loop_count = 0;

void setup()
{
    initialize(gc_data.report);

    // Omajinai to recognize the controller
    gc_data.report.start = 1;
    gc_console.write(gc_data);
    gc_data.report.start = 0;
    gc_console.write(gc_data);

    Serial.begin(9600);
}

inline void update()
{
    if (0 < Serial.available())
    {
        loop_count = 0;
        nx_report.raw[index] = Serial.read();

        if (!is_valid(nx_report, index))
        {
            char buffer[256];
            sprintf(buffer, "Parse error: {\"index\":%d,\"value\":%d}", index, nx_report.raw[index]);
            Serial.println(buffer);

            loop_count = 0;
            index = 0;
            return;
        }

        if (index == 10)
        {
            convert(nx_report, gc_data.report);
            loop_count = 0;
            index = 0;
#ifdef DEBUG
            pretty_print(nx_report);
            pretty_print(gc_data.report);
#endif
        }
        else
        {
            index++;
        }
    }
    else if (1000 < loop_count)
    {
        Serial.println("Timeout error: discard incomplete data");
        loop_count = 0;
        index = 0;
    }
    else if (0 < index)
    {
        loop_count++;
    }
}

void loop()
{
    update();
    if (!gc_console.write(gc_data))
    {
        Serial.println("Send error: not connected");
    }
}
