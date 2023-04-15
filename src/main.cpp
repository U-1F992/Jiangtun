#include <Arduino.h>

#include "plog/Appenders/ArduinoAppender.h"
#include "plog/Init.h"
#include "plog/Log.h"
#include "plog/Formatters/TxtFormatter.h"

#include "nxmc.h"
#include "nxmc/gamecube.h"

constexpr uint8_t kPin = 5;
constexpr unsigned long kBaudRate = 9600;

static plog::ArduinoAppender<plog::TxtFormatter> arduino_appender(Serial);

void setup()
{
    plog::init(plog::warning, &arduino_appender);
    Serial.begin(kBaudRate);
}

auto TryReceive = nxmc::PacketReceiver(
    []()
    {
        typedef etl::expected<uint8_t, std::string> _;
        
        if (Serial.available() == 0)
        {
            return _(etl::unexpected<std::string>("Serial not available"));
        }

        auto ret = Serial.read();
        if (ret == -1)
        {
            return _(etl::unexpected<std::string>("got -1 from Serial.read()"));
        }

        return _(static_cast<uint8_t>(ret));
    });

auto TrySend = nxmc::gamecube::PacketSender(
    kPin,
    [](CGamecubeConsole &console, Gamecube_Data_t &d)
    {
        d.report.b = 1;
        d.report.x = 1;
        d.report.start = 1;
        console.write(d);
        
        // Or use servo, etc.
    },
    [](CGamecubeConsole &console, Gamecube_Data_t &d)
    {
        d.report.b = 0;
        d.report.x = 0;
        d.report.start = 0;
        console.write(d);
    });

void loop()
{
    auto received = TryReceive();
    // if (!received.has_value())
    // {
    //     PLOGW << received.error();
    // }

    auto sent = TrySend(received);
    if (!sent.has_value())
    {
        PLOGW << sent.error();
    }
}
