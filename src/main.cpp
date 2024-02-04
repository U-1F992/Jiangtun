#include <Arduino.h>
#include <Servo.h>

#include "pico/mutex.h"
#include "hardware/timer.h"

#include "Bluewhale.h"

#include "nthaka.h"
#include "nthaka/dol.h"
#include "nthaka/nxmc2.h"
#include "nthaka/orca.h"
#include "nthaka/pokecon.h"

#include "jiangtun.h"
#include "internal.h"

static jiangtun_t j;

#ifdef JIANGTUN_CONFIG_BOARD_XIAO_RP2040
#define PIN_RESET 3
#define PIN_GC 7
#define PIN_SERVO 0
#else // JIANGTUN_CONFIG_BOARD_PICO
#define PIN_RESET 3
#define PIN_GC 5
#define PIN_SERVO 6
#endif

static Servo servo;

static mutex_t mtx;
static CGamecubeConsole console(PIN_GC);
static Gamecube_Data_t gc = defaultGamecubeData;

static dol_format_handler_t dol;
static nxmc2_format_handler_t nxmc2;
static orca_format_handler_t orca;
static pokecon_format_handler_t pokecon;
static nthaka_format_handler_t *fmts[] = {(nthaka_format_handler_t *)&nxmc2,
                                          (nthaka_format_handler_t *)&orca,
#ifdef JIANGTUN_CONFIG_ENABLE_DOL
                                          (nthaka_format_handler_t *)&dol
#else
                                          (nthaka_format_handler_t *)&pokecon
#endif // JIANGTUN_CONFIG_ENABLE_DOL
};
static size_t idx_servo_blocking[] = {1,
#ifdef JIANGTUN_CONFIG_ENABLE_DOL
                                      2
#endif // JIANGTUN_CONFIG_ENABLE_DOL
};
static nthaka_multi_format_handler_t fmt;
static nthaka_buffer_t buf;
static nthaka_gamepad_state_t out;

static int64_t _led_off(alarm_id_t id, void *user_data)
{
    digitalWriteFast(LED_BUILTIN, LOW);
    return 0;
}

static inline void async_led_on(uint32_t dur_ms)
{
    digitalWriteFast(LED_BUILTIN, HIGH);
    add_alarm_in_ms(dur_ms, _led_off, NULL, false);
}

void setup()
{
    Serial.setTimeout(100);
    Serial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    pinMode(PIN_RESET, INPUT); // Hi-Z

    // Setup for SG90
    servo.attach(PIN_SERVO, 500, 2400);
    servo.write(90);

    mutex_init(&mtx);

    gc_console_data_init(&console, &gc, &mtx);

    if (!(dol_format_handler_init(&dol) &&
          nxmc2_format_handler_init(&nxmc2) &&
          orca_format_handler_init(&orca) &&
          pokecon_format_handler_init(&pokecon) &&
          nthaka_multi_format_handler_init(&fmt, fmts, 3) &&
          nthaka_buffer_init(&buf, (nthaka_format_handler_t *)&fmt) &&

          jiangtun_init(&j, PIN_RESET, &servo, idx_servo_blocking, sizeof(idx_servo_blocking) / sizeof(size_t), &mtx)))
    {
        while (true)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
            delay(100);
        }
    }
}

void loop()
{
    uint8_t d;
    nthaka_buffer_state_t s;
    if (Serial.readBytes(&d, 1) != 1 ||
        (s = nthaka_buffer_append(&buf, d, &out)) == NTHAKA_BUFFER_REJECTED)
    {
        nthaka_buffer_clear(&buf);
        return;
    }
    else if (s == NTHAKA_BUFFER_PENDING)
    {
        return;
    }

    size_t *idx = nthaka_multi_format_handler_get_last_deserialized_index(&fmt);
    if (idx == NULL)
    {
        *idx = 0;
    }

    async_led_on(100);
    jiangtun_update(&j, &out, *idx, &gc);
    nthaka_buffer_clear(&buf);
}

void setup1()
{
    delay(10);
}

void loop1()
{
    mutex_enter_blocking(&mtx);
    bool ret = console.write(gc);
    mutex_exit(&mtx);

    if (!ret)
    {
        Serial.println("GC is not powered on or not connected.");
    }
}