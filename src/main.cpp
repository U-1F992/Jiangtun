#include <Arduino.h>

#include "nxmc.h"
#include "nxmc/arduino.h"
#include "nxmc/gamecube.h"
#include "nxmc/helper.h"

constexpr uint8_t kPin = 5;
constexpr nxmc::Severity kLowestToWrite = nxmc::Severity::None;
constexpr unsigned long kTimeout = 100;
constexpr unsigned long kBaudRate = 9600;

const auto _ = nxmc::DefineLoop(
    nxmc::helper::DefineRetrieve(
        nxmc::arduino::TryReadBytes),
    nxmc::gamecube::DefineSendData(kPin),
    nxmc::helper::DefineLog(
        nxmc::arduino::Println, kLowestToWrite));

void setup()
{
    Serial.setTimeout(kTimeout);
    Serial.begin(kBaudRate);
}

void loop()
{
    _();
}
