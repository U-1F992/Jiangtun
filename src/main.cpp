#include <Arduino.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#include "pico/mutex.h"
#include "hardware/timer.h"

#include "Bluewhale.h"

#include "nthaka.h"
#include "nthaka/dol.h"
#include "nthaka/nxmc2.h"
#include "nthaka/orca.h"
#include "nthaka/pokecon.h"

#ifdef JIANGTUN_CONFIG_BOARD_XIAO_RP2040

#define PIN_RESET D10
#define PIN_SERVO D6
#define PIN_GAMECUBE D5

static Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
#define initLED()                           \
    do                                      \
    {                                       \
        pinMode(PIN_LED_R, OUTPUT);         \
        pinMode(PIN_LED_G, OUTPUT);         \
        pinMode(PIN_LED_B, OUTPUT);         \
        digitalWrite(PIN_LED_R, HIGH);      \
        digitalWrite(PIN_LED_G, HIGH);      \
        digitalWrite(PIN_LED_B, HIGH);      \
        pixels.begin();                     \
        pinMode(NEOPIXEL_POWER, OUTPUT);    \
        digitalWrite(NEOPIXEL_POWER, HIGH); \
    } while (0)
#define turnOnLED()                     \
    do                                  \
    {                                   \
        pixels.setPixelColor(0, color); \
        pixels.show();                  \
    } while (0)
#define turnOffLED()    \
    do                  \
    {                   \
        pixels.clear(); \
        pixels.show();  \
    } while (0)

#else // JIANGTUN_CONFIG_BOARD_PICO

#define PIN_RESET 3
#define PIN_SERVO 6
#define PIN_GAMECUBE 5

#define initLED()                       \
    do                                  \
    {                                   \
        pinMode(LED_BUILTIN, OUTPUT);   \
        digitalWrite(LED_BUILTIN, LOW); \
    } while (0)
#define turnOnLED() (digitalWrite(LED_BUILTIN, HIGH))
#define turnOffLED() (digitalWrite(LED_BUILTIN, LOW))

#endif

static Servo servo;

static uint32_t color;
static uint16_t hue;

typedef enum jiangtun_reset_action_t
{
    JIANGTUN_RESET_PRESS,
    JIANGTUN_RESET_RELEASE,
    JIANGTUN_RESET_PRESS_RELEASE,
    JIANGTUN_RESET_NOTHING,
} jiangtun_reset_action_t;

typedef struct jiangtun_gamecube_data_t
{
    Gamecube_Data_t gamecube;
    nthaka_button_state_t current_reset_state;
    jiangtun_reset_action_t next_action;
} jiangtun_gamecube_data_t;

static mutex_t mtx;
static CGamecubeConsole gamecube(PIN_GAMECUBE);
static jiangtun_gamecube_data_t data = {.gamecube = defaultGamecubeData,
                                        .current_reset_state = NTHAKA_BUTTON_RELEASED,
                                        .next_action = JIANGTUN_RESET_NOTHING};

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
static size_t auto_reset_release_idx[] = {1,
#ifdef JIANGTUN_CONFIG_ENABLE_DOL
                                          2
#endif // JIANGTUN_CONFIG_ENABLE_DOL
};
static size_t auto_reset_release_idx_size = sizeof(auto_reset_release_idx) / sizeof(auto_reset_release_idx[0]);
static nthaka_multi_format_handler_t fmt;
static nthaka_buffer_t buf;
static nthaka_gamepad_state_t out;

static int64_t _turnOffLED(alarm_id_t _0, void *_1)
{
    turnOffLED();
    return 0;
}

static inline void blinkLEDAsync(uint32_t ms)
{
    turnOnLED();
    add_alarm_in_ms(ms, _turnOffLED, NULL, false);
}

static inline void pressReset()
{
    servo.write(65);
    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, LOW);
}

static inline int64_t releaseReset(alarm_id_t _0, void *_1)
{
    servo.write(90);
    pinMode(PIN_RESET, INPUT);

    if (data.current_reset_state != NTHAKA_BUTTON_RELEASED)
    {
        mutex_enter_blocking(&mtx);
        {
            data.current_reset_state = NTHAKA_BUTTON_RELEASED;
        }
        mutex_exit(&mtx);
        blinkLEDAsync(100);
    }

    return 0;
}

static void updateData(nthaka_gamepad_state_t *state, size_t idx, size_t auto_reset_release_idx[], size_t auto_reset_release_idx_size, jiangtun_gamecube_data_t *data)
{
    // Update the reset state and determine the next reset action.
    nthaka_button_state_t next_reset_state = state->home;
    if (data->current_reset_state != next_reset_state)
    {
        if (next_reset_state == NTHAKA_BUTTON_PRESSED)
        {
            data->next_action = JIANGTUN_RESET_PRESS;

            for (size_t i = 0; i < auto_reset_release_idx_size; i++)
            {
                if (auto_reset_release_idx[i] == idx)
                {
                    data->next_action = JIANGTUN_RESET_PRESS_RELEASE;
                    break;
                }
            }
        }
        else
        {
            data->next_action = JIANGTUN_RESET_RELEASE;
        }
        data->current_reset_state = next_reset_state;
    }
    else
    {
        data->next_action = JIANGTUN_RESET_NOTHING;
    }

    data->gamecube.report.y = state->y == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.b = state->b == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.a = state->a == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.x = state->x == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.l = state->l == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.r = state->r == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.z = state->zr == NTHAKA_BUTTON_PRESSED ? 1U : 0U;
    data->gamecube.report.start = state->plus == NTHAKA_BUTTON_PRESSED ? 1U : 0U;

    switch (state->hat)
    {
    case NTHAKA_HAT_UP:
        data->gamecube.report.dup = 1U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 0U;
        break;

    case NTHAKA_HAT_UPRIGHT:
        data->gamecube.report.dup = 1U;
        data->gamecube.report.dright = 1U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 0U;
        break;

    case NTHAKA_HAT_RIGHT:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 1U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 0U;
        break;

    case NTHAKA_HAT_DOWNRIGHT:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 1U;
        data->gamecube.report.ddown = 1U;
        data->gamecube.report.dleft = 0U;
        break;

    case NTHAKA_HAT_DOWN:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 1U;
        data->gamecube.report.dleft = 0U;
        break;

    case NTHAKA_HAT_DOWNLEFT:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 1U;
        data->gamecube.report.dleft = 1U;
        break;

    case NTHAKA_HAT_LEFT:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 1U;
        break;

    case NTHAKA_HAT_UPLEFT:
        data->gamecube.report.dup = 1U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 1U;
        break;

    case NTHAKA_HAT_NEUTRAL:
    default:
        data->gamecube.report.dup = 0U;
        data->gamecube.report.dright = 0U;
        data->gamecube.report.ddown = 0U;
        data->gamecube.report.dleft = 0U;
        break;
    }

    // There are a few games that do not handle yAxis=0 and cyAxis=0 correctly.
    data->gamecube.report.xAxis = state->l_stick.x;
    uint8_t y_axis = 0xFF - state->l_stick.y;
    data->gamecube.report.yAxis = y_axis == 0U ? 1U : y_axis;

    data->gamecube.report.cxAxis = state->r_stick.x;
    uint8_t cy_axis = 0xFF - state->r_stick.y;
    data->gamecube.report.cyAxis = cy_axis == 0U ? 1U : cy_axis;
}

static void initGamecube(CGamecubeConsole *console, jiangtun_gamecube_data_t *data)
{
    data->gamecube.report.a = 0;
    data->gamecube.report.b = 0;
    data->gamecube.report.x = 0;
    data->gamecube.report.y = 0;
    data->gamecube.report.start = 0;
    data->gamecube.report.dleft = 0;
    data->gamecube.report.dright = 0;
    data->gamecube.report.ddown = 0;
    data->gamecube.report.dup = 0;
    data->gamecube.report.z = 0;
    data->gamecube.report.r = 0;
    data->gamecube.report.l = 0;
    data->gamecube.report.xAxis = 128;
    data->gamecube.report.yAxis = 128;
    data->gamecube.report.cxAxis = 128;
    data->gamecube.report.cyAxis = 128;
    data->gamecube.report.left = 0;
    data->gamecube.report.right = 0;

    // Magic spell to make the controller be recognized by the Gamecube
    data->gamecube.report.start = 1;
    console->write(data->gamecube);
    data->gamecube.report.start = 0;
    console->write(data->gamecube);

    data->current_reset_state = NTHAKA_BUTTON_RELEASED;

    data->next_action = JIANGTUN_RESET_NOTHING;
}

void setup()
{
    mutex_init(&mtx);
    mutex_enter_blocking(&mtx);
    {
        initGamecube(&gamecube, &data);
    }
    mutex_exit(&mtx);

    Serial.setTimeout(100);
    Serial.begin(9600);

    servo.attach(PIN_SERVO, 500, 2400);
    servo.write(90);

    pinMode(PIN_RESET, INPUT);

    initLED();

    dol_format_handler_init(&dol);
    nxmc2_format_handler_init(&nxmc2);
    orca_format_handler_init(&orca);
    pokecon_format_handler_init(&pokecon);
    nthaka_multi_format_handler_init(&fmt, fmts, 3);
    nthaka_buffer_init(&buf, (nthaka_format_handler_t *)&fmt);
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
        // Unreachable code
        *idx = 0;
    }

    blinkLEDAsync(100);

    mutex_enter_blocking(&mtx);
    {
        updateData(&out, *idx, auto_reset_release_idx, auto_reset_release_idx_size, &data);
    }
    mutex_exit(&mtx);

    nthaka_buffer_clear(&buf);
}

void setup1()
{
    delay(10);
}

void loop1()
{
    color = Adafruit_NeoPixel::ColorHSV(hue);
    hue += 16;

    bool ret;

    mutex_enter_blocking(&mtx);
    {
        ret = gamecube.write(data.gamecube);

        switch (data.next_action)
        {
        case JIANGTUN_RESET_PRESS:
            pressReset();
            break;

        case JIANGTUN_RESET_RELEASE:
            releaseReset(0, NULL);
            break;

        case JIANGTUN_RESET_PRESS_RELEASE:
            pressReset();
            add_alarm_in_ms(500, releaseReset, NULL, false);
            break;

        case JIANGTUN_RESET_NOTHING:
        default:
            break;
        }
        data.next_action = JIANGTUN_RESET_NOTHING;
    }
    mutex_exit(&mtx);

    if (!ret)
    {
        Serial.println("GC is not powered on or not connected.");
    }
}