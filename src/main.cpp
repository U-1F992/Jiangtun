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
    plog::init(plog::verbose, &arduino_appender);
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
    []()
    {
        // Use servo, etc.
    });

void loop()
{
    auto received = TryReceive();
    if (!received.has_value())
    {
        PLOGD << received.error();
    }

    auto sent = TrySend(received);
    if (!sent.has_value())
    {
        PLOGD << sent.error();
    }
}
