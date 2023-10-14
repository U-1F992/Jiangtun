#include <Arduino.h>
#include <Servo.h>

#include "pico/stdlib.h"
#include "pico/mutex.h"
#include "hardware/timer.h"

#include "Bluewhale.h"

#include "nxamf.h"
#include "nxamf/nxmc2.h"
#include "nxamf/pokecon.h"

#include "orca.h"

static const int SERIAL_INACTIVE_TIMEOUT = 100;
static int inactive_count = 0;

static CGamecubeConsole console(5);
static Servo servo;
static const int PIN_SERVO = 6;
static Gamecube_Data_t d = defaultGamecubeData;
static mutex_t d_mutex;

static Nxmc2Protocol *nxmc2;
static PokeConProtocol *pokecon;
static OrcaProtocol *orca;
static const size_t ORCA_INDEX = 2;
static NxamfBytesProtocolInterface *protocols[3];
static NxamfProtocolMultiplexer *mux;
static NxamfBytesBuffer *buffer;

static NxamfButtonState reset_state = NXAMF_BUTTON_STATE_RELEASED;

static int64_t led_off(alarm_id_t id, void *user_data)
{
    digitalWrite(LED_BUILTIN, LOW);
    return false;
}

static void async_led_on_for_100ms()
{
    digitalWriteFast(LED_BUILTIN, HIGH);
    alarm_id_t alarm_id = add_alarm_in_ms(100, led_off, NULL, false);
}

static void reflect_state(NxamfGamepadState *state)
{
    if (state == NULL)
    {
        return;
    }

    async_led_on_for_100ms();

    mutex_enter_blocking(&d_mutex);

    d.report.y = (uint8_t)state->y;
    d.report.b = (uint8_t)state->b;
    d.report.a = (uint8_t)state->a;
    d.report.x = (uint8_t)state->x;
    d.report.l = (uint8_t)state->l;
    d.report.r = (uint8_t)state->r;
    d.report.z = (uint8_t)state->zr;
    d.report.start = (uint8_t)state->plus;
    if (reset_state != state->home)
    {
        servo.write(state->home == NXAMF_BUTTON_STATE_PRESSED ? 65 : 90);
        reset_state = state->home;

        // ORCAのサーボモータ使用はブロッキング処理
        if (mux->ready_index == ORCA_INDEX)
        {
            delay(500);
            servo.write(90);
            delay(500);
            reset_state = NXAMF_BUTTON_STATE_RELEASED;
        }
    }

    switch (state->hat)
    {
    case NXAMF_HAT_STATE_UP:
        d.report.dup = 1;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;
    case NXAMF_HAT_STATE_UPRIGHT:
        d.report.dup = 1;
        d.report.dright = 1;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;
    case NXAMF_HAT_STATE_RIGHT:
        d.report.dup = 0;
        d.report.dright = 1;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;
    case NXAMF_HAT_STATE_DOWNRIGHT:
        d.report.dup = 0;
        d.report.dright = 1;
        d.report.ddown = 1;
        d.report.dleft = 0;
        break;
    case NXAMF_HAT_STATE_DOWN:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 1;
        d.report.dleft = 0;
        break;
    case NXAMF_HAT_STATE_DOWNLEFT:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 1;
        d.report.dleft = 1;
        break;
    case NXAMF_HAT_STATE_LEFT:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 1;
        break;
    case NXAMF_HAT_STATE_UPLEFT:
        d.report.dup = 1;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 1;
        break;
    case NXAMF_HAT_STATE_NEUTRAL:
    default:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;
    }

    d.report.xAxis = state->l_stick.x;
    // There are a few games that do not handle yAxis=0 correctly.
    uint8_t y_axis = 0xFF - state->l_stick.y;
    d.report.yAxis = y_axis == 0 ? 1 : y_axis;

    d.report.cxAxis = state->r_stick.x;
    // There are a few games that do not handle cyAxis=0 correctly.
    uint8_t cy_axis = 0xFF - state->r_stick.y;
    d.report.cyAxis = cy_axis == 0 ? 1 : cy_axis;

    mutex_exit(&d_mutex);
}

void setup()
{
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Setup for SG90
    servo.attach(PIN_SERVO, 500, 2400);

    nxmc2 = nxmc2_protocol_new();
    pokecon = pokecon_protocol_new();
    orca = orca_protocol_new();
    protocols[0] = (NxamfBytesProtocolInterface *)nxmc2;
    protocols[1] = (NxamfBytesProtocolInterface *)pokecon;
    protocols[2] = (NxamfBytesProtocolInterface *)orca;
    mux = nxamf_protocol_multiplexer_new(protocols, 3);
    if (nxmc2 == NULL || pokecon == NULL || orca == NULL || mux == NULL)
    {
        abort();
    }
    buffer = nxamf_bytes_buffer_new((NxamfBytesProtocolInterface *)mux);
    if (buffer == NULL)
    {
        abort();
    }
}

void loop()
{
    if (Serial.available() == 0)
    {
        inactive_count++;
        if (SERIAL_INACTIVE_TIMEOUT < inactive_count)
        {
            inactive_count = 0;
            nxamf_bytes_buffer_clear(buffer);
        }
        return;
    }
    inactive_count = 0;

    uint8_t packet = Serial.read();
    NxamfGamepadState *state = nxamf_bytes_buffer_append(buffer, packet);
    if (state == NULL)
    {
        return;
    }

    reflect_state(state);
}

void setup1()
{
    mutex_init(&d_mutex);

    mutex_enter_blocking(&d_mutex);

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

    mutex_exit(&d_mutex);
}

void loop1()
{
    mutex_enter_blocking(&d_mutex);
    bool ret = console.write(d);
    mutex_exit(&d_mutex);

    if (!ret)
    {
        Serial.println("GC is not powered on or not connected.");
    }
}