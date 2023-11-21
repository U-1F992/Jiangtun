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
static mutex_t serial1_mutex;

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

static const char *_button_to_string(NxamfButtonState state)
{
    switch (state)
    {
    case NXAMF_BUTTON_STATE_RELEASED:
        return "RELEASED";
    case NXAMF_BUTTON_STATE_PRESSED:
        return "PRESSED";
    default:
        return "[Unknown]";
    }
}

static const char *_hat_to_string(NxamfHatState state)
{
    switch (state)
    {
    case NXAMF_HAT_STATE_UP:
        return "UP";
    case NXAMF_HAT_STATE_UPRIGHT:
        return "UPRIGHT";
    case NXAMF_HAT_STATE_RIGHT:
        return "RIGHT";
    case NXAMF_HAT_STATE_DOWNRIGHT:
        return "DOWNRIGHT";
    case NXAMF_HAT_STATE_DOWN:
        return "DOWN";
    case NXAMF_HAT_STATE_DOWNLEFT:
        return "DOWNLEFT";
    case NXAMF_HAT_STATE_LEFT:
        return "LEFT";
    case NXAMF_HAT_STATE_UPLEFT:
        return "UPLEFT";
    case NXAMF_HAT_STATE_NEUTRAL:
        return "NEUTRAL";
    default:
        return "[Unknown]";
    }
}

static char _format_state_buffer[1024];
static char *format_state(const NxamfGamepadState *state)
{
    snprintf(_format_state_buffer, 1024,
             "Gamepad State:\n"
             "  Y: %s\t"
             "B: %s\t"
             "A: %s\t"
             "X: %s\n"
             "  L: %s\t"
             "R: %s\t"
             "ZL: %s\t"
             "ZR: %s\n"
             "  Minus: %s\t"
             "Plus: %s\t"
             "L Click: %s\t"
             "R Click: %s\n"
             "  Home: %s\t"
             "Capture: %s\n"
             "  Hat: %s\n"
             "  Left Stick: (x=%u, y=%u)\n"
             "  Right Stick: (x=%u, y=%u)\n",
             _button_to_string(state->y), _button_to_string(state->b),
             _button_to_string(state->a), _button_to_string(state->x),
             _button_to_string(state->l), _button_to_string(state->r),
             _button_to_string(state->zl), _button_to_string(state->zr),
             _button_to_string(state->minus), _button_to_string(state->plus),
             _button_to_string(state->l_click), _button_to_string(state->r_click),
             _button_to_string(state->home), _button_to_string(state->capture),
             _hat_to_string(state->hat),
             state->l_stick.x, state->l_stick.y, state->r_stick.x, state->r_stick.y);

    return _format_state_buffer;
}

static int64_t led_off(alarm_id_t id, void *user_data)
{
    digitalWrite(LED_BUILTIN, LOW);
    return false;
}

static void async_led_on_for_100ms()
{
    digitalWrite(LED_BUILTIN, HIGH);
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

#ifndef NDEBUG
    mutex_init(&serial1_mutex);
    mutex_enter_blocking(&serial1_mutex);
    Serial1.setTX(0);
    Serial1.setRX(1);
    Serial1.begin();
    mutex_exit(&serial1_mutex);
#define Serial1_println(msg)              \
    mutex_enter_blocking(&serial1_mutex); \
    Serial1.println(msg);                 \
    mutex_exit(&serial1_mutex);
#else
#define Serial1_println(msg) ((void)0)
#endif

    // Setup for SG90
    servo.attach(PIN_SERVO, 500, 2400);

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

    nxmc2 = nxmc2_protocol_new();
    pokecon = pokecon_protocol_new();
    orca = orca_protocol_new();
    protocols[0] = (NxamfBytesProtocolInterface *)nxmc2;
    protocols[1] = (NxamfBytesProtocolInterface *)pokecon;
    protocols[2] = (NxamfBytesProtocolInterface *)orca;
    mux = nxamf_protocol_multiplexer_new(protocols, 3);
    buffer = nxamf_bytes_buffer_new((NxamfBytesProtocolInterface *)mux);
    assert(
        nxmc2 != NULL &&
        pokecon != NULL &&
        orca != NULL &&
        mux != NULL &&
        buffer != NULL);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Blink if the setup routine succeeded.
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
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
    Serial1_println(packet);
    NxamfGamepadState *state = nxamf_bytes_buffer_append(buffer, packet);
    if (state == NULL)
    {
        return;
    }

    Serial1_println(format_state(state));
    reflect_state(state);

    nxamf_gamepad_state_delete(state);
    state = NULL;
}

void setup1()
{
}

void loop1()
{
    mutex_enter_blocking(&d_mutex);
    bool ret = console.write(d);
    mutex_exit(&d_mutex);

    if (!ret)
    {
        Serial1_println("GC is not powered on or not connected.");
    }
}