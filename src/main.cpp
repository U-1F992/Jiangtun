#include <Arduino.h>
#include <Servo.h>

#include "Bluewhale.h"
#include "nxmc2_contrib.h"

static CGamecubeConsole console(5);
static Servo servo;
static const int PIN_SERVO = 6;
static Gamecube_Data_t d = defaultGamecubeData;

static const int SERIAL_INACTIVE_TIMEOUT = 100;
static int inactive_count = 0;

static NXMC2CommandBuilder builder;
static NXMC2Command command;
static NXMC2CommandHandler handler;

static NXMC2CommandButtonState reset_state_ = NXMC2_COMMAND_BUTTON_STATE_RELEASED;

void setup()
{
    Serial.begin(9600);

    // Setup for SG90
    servo.attach(PIN_SERVO, 500, 2400);

    d.report.a = 0;
    d.report.b = 0;
    d.report.x = 0;
    d.report.y = 0;
    d.report.start = 0;
    d.report.dleft = 0;
    d.report.dright = 0;
    d.report.ddown = 0;
    d.report.dup = 0;
    d.report.z = 0;
    d.report.r = 0;
    d.report.l = 0;
    d.report.xAxis = 128;
    d.report.yAxis = 128;
    d.report.cxAxis = 128;
    d.report.cyAxis = 128;
    d.report.left = 0;
    d.report.right = 0;

    // Omajinai to recognize the controller
    d.report.start = 1;
    console.write(d);
    d.report.start = 0;
    console.write(d);

    nxmc2_command_builder_initialize(&builder);
    nxmc2_command_handler_initialize(&handler);
    handler.y = [](NXMC2CommandButtonState state)
    { d.report.y = (uint8_t)state; };
    handler.b = [](NXMC2CommandButtonState state)
    { d.report.b = (uint8_t)state; };
    handler.a = [](NXMC2CommandButtonState state)
    { d.report.a = (uint8_t)state; };
    handler.x = [](NXMC2CommandButtonState state)
    { d.report.x = (uint8_t)state; };
    handler.l = [](NXMC2CommandButtonState state)
    { d.report.l = (uint8_t)state; };
    handler.r = [](NXMC2CommandButtonState state)
    { d.report.r = (uint8_t)state; };
    handler.zr = [](NXMC2CommandButtonState state)
    { d.report.z = (uint8_t)state; };
    handler.plus = [](NXMC2CommandButtonState state)
    { d.report.start = (uint8_t)state; };
    handler.home = [](NXMC2CommandButtonState state)
    {
        if (reset_state_ == state)
        {
            return;
        }

        servo.write(state == NXMC2_COMMAND_BUTTON_STATE_PRESSED ? 65 : 90);
        reset_state_ = state;
    };
    handler.hat = [](NXMC2CommandHatState state)
    {
        switch (state)
        {
        case NXMC2_COMMAND_HAT_STATE_UP:
            d.report.dup = 1;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;
        case NXMC2_COMMAND_HAT_STATE_UPRIGHT:
            d.report.dup = 1;
            d.report.dright = 1;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;
        case NXMC2_COMMAND_HAT_STATE_RIGHT:
            d.report.dup = 0;
            d.report.dright = 1;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;
        case NXMC2_COMMAND_HAT_STATE_DOWNRIGHT:
            d.report.dup = 0;
            d.report.dright = 1;
            d.report.ddown = 1;
            d.report.dleft = 0;
            break;
        case NXMC2_COMMAND_HAT_STATE_DOWN:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 1;
            d.report.dleft = 0;
            break;
        case NXMC2_COMMAND_HAT_STATE_DOWNLEFT:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 1;
            d.report.dleft = 1;
            break;
        case NXMC2_COMMAND_HAT_STATE_LEFT:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 1;
            break;
        case NXMC2_COMMAND_HAT_STATE_UPLEFT:
            d.report.dup = 1;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 1;
            break;
        case NXMC2_COMMAND_HAT_STATE_NEUTRAL:
        default:
            d.report.dup = 0;
            d.report.dright = 0;
            d.report.ddown = 0;
            d.report.dleft = 0;
            break;
        }
    };
    handler.l_stick = [](uint8_t x, uint8_t y)
    {
        d.report.xAxis = x;

        // There are a few games that do not handle yAxis=0 correctly.
        uint8_t y_axis = 0xFF - y;
        d.report.yAxis = y_axis == 0 ? 1 : y_axis;
    };
    handler.r_stick = [](uint8_t x, uint8_t y)
    {
        d.report.cxAxis = x;

        // There are a few games that do not handle cyAxis=0 correctly.
        uint8_t cy_axis = 0xFF - y;
        d.report.cyAxis = cy_axis == 0 ? 1 : cy_axis;
    };
}

void static update_data()
{
    if (Serial.available() == 0)
    {
        inactive_count++;
        if (SERIAL_INACTIVE_TIMEOUT < inactive_count)
        {
            inactive_count = 0;
            nxmc2_command_builder_flush(&builder);
        }
        return;
    }
    inactive_count = 0;

    NXMC2Result ret = nxmc2_command_builder_append(&builder, Serial.read());
    if (ret != NXMC2_RESULT_OK)
    {
        nxmc2_command_builder_flush(&builder);
        return;
    }

    ret = nxmc2_command_builder_build(&builder, &command);
    if (ret != NXMC2_RESULT_OK)
    {
        return;
    }
    nxmc2_command_execute(&command, &handler);

    nxmc2_command_builder_flush(&builder);
}

void loop()
{
    update_data();
    if (!console.write(d))
    {
        Serial.println("GCが起動していないか、接続されていません。");
    }
}