#include <Arduino.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#include <Bluewhale.h>
#include <jiangtun.h>

// #ifdef JIANGTUN_CONFIG_BOARD_XIAO_RP2040

// #define PIN_RESET D10
// #define PIN_SERVO D6
// #define PIN_GAMECUBE D5

// static Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
// #define initLED() \
//     do { \
//         pinMode(PIN_LED_R, OUTPUT); \
//         pinMode(PIN_LED_G, OUTPUT); \
//         pinMode(PIN_LED_B, OUTPUT); \
//         digitalWrite(PIN_LED_R, HIGH); \
//         digitalWrite(PIN_LED_G, HIGH); \
//         digitalWrite(PIN_LED_B, HIGH); \
//         pixels.begin(); \
//         pinMode(NEOPIXEL_POWER, OUTPUT); \
//         digitalWrite(NEOPIXEL_POWER, HIGH); \
//     } while (0)
// #define turnOnLED(color) \
//     do { \
//         pixels.setPixelColor(0, (color)); \
//         pixels.show(); \
//     } while (0)
// #define turnOffLED() \
//     do { \
//         pixels.clear(); \
//         pixels.show(); \
//     } while (0)

// #else // JIANGTUN_CONFIG_BOARD_PICO

// #define PIN_RESET 3
// #define PIN_SERVO 6
// #define PIN_GAMECUBE 5

typedef struct jiangtun_board_rp2040_t {
    jiangtun_board_t base;

    Stream *serial;
    CGamecubeConsole *gamecube;
    Gamecube_Data_t gamecube_data;
    Servo servo;
    pin_size_t reset;
    bool reset_data;
} jiangtun_board_rp2040_t;

static void jiangtun_board_rp2040_init(jiangtun_board_rp2040_t *board,
                                       Stream *serial,
                                       CGamecubeConsole *gamecube,
                                       pin_size_t servo, pin_size_t reset) {
    assert(board != NULL);
    assert(serial != NULL);
    assert(gamecube != NULL);

    board->serial = serial;
    board->gamecube = gamecube;
    board->gamecube_data = defaultGamecubeData;
    board->servo.attach(servo, 500, 2400);
    board->reset = reset;
    board->reset_data = true; // to ensure initial releasing

    jiangtun_board_init(
        &(board->base),
        [](jiangtun_board_t *base, unsigned char *c) -> jiangtun_bool_t {
            jiangtun_board_rp2040_t *board = (jiangtun_board_rp2040_t *)base;
            int c_ = 0;
            assert(board != NULL);
            assert(c != NULL);

            if (board->serial->available() <= 0) {
                return JIANGTUN_FALSE;
            }
            c_ = board->serial->read();
            if (c_ < 0 || 255 < c_) {
                return JIANGTUN_FALSE;
            }
            *c = c_ & 0xFF;
            return JIANGTUN_TRUE;
        },
        [](jiangtun_board_t *base, const char *s) {
            jiangtun_board_rp2040_t *board = (jiangtun_board_rp2040_t *)base;
            assert(board != NULL);
            assert(s != NULL);

            board->serial->print(s);
        },
        [](jiangtun_board_t *base,
           jiangtun_report_mode3_t *report) -> jiangtun_bool_t {
            jiangtun_board_rp2040_t *board = (jiangtun_board_rp2040_t *)base;
            assert(board != NULL);
            assert(report != NULL);

            board->gamecube_data.report.a = report->a ? 1 : 0;
            board->gamecube_data.report.b = report->b ? 1 : 0;
            board->gamecube_data.report.x = report->x ? 1 : 0;
            board->gamecube_data.report.y = report->y ? 1 : 0;
            board->gamecube_data.report.start = report->start ? 1 : 0;
            board->gamecube_data.report.dleft = report->dleft ? 1 : 0;
            board->gamecube_data.report.dright = report->dright ? 1 : 0;
            board->gamecube_data.report.ddown = report->ddown ? 1 : 0;
            board->gamecube_data.report.dup = report->dup ? 1 : 0;
            board->gamecube_data.report.z = report->z ? 1 : 0;
            board->gamecube_data.report.r = report->r ? 1 : 0;
            board->gamecube_data.report.l = report->l ? 1 : 0;
            board->gamecube_data.report.xAxis = (uint8_t)report->xAxis;
            board->gamecube_data.report.yAxis = (uint8_t)report->yAxis;
            board->gamecube_data.report.cxAxis = (uint8_t)report->cxAxis;
            board->gamecube_data.report.cyAxis = (uint8_t)report->cyAxis;
            board->gamecube_data.report.left = (uint8_t)report->left;
            board->gamecube_data.report.right = (uint8_t)report->right;

            if (!(board->reset_data) && report->reset) {
                board->servo.write(65);
                pinMode(board->reset, OUTPUT);
                digitalWrite(board->reset, LOW);
                board->reset_data = true;

            } else if (board->reset_data && !(report->reset)) {
                board->servo.write(90);
                pinMode(board->reset, INPUT);
                board->reset_data = false;
            }

            return board->gamecube->write(board->gamecube_data)
                       ? JIANGTUN_TRUE
                       : JIANGTUN_FALSE;
        },
        [](jiangtun_board_t *base, jiangtun_bool_t state) {
            jiangtun_board_rp2040_t *board = (jiangtun_board_rp2040_t *)base;
            assert(board != NULL);
        });
}

static const pin_size_t PIN_GAMECUBE = 7; // D5 (SCL)
static const pin_size_t PIN_SERVO = 0;    // D6 (TX)
static const pin_size_t PIN_RESET = 3;    // D10 (MOSI)

static const pin_size_t PIN_NEOPIXEL = 12;
static const pin_size_t PIN_NEOPIXEL_POWER = 11;
static const pin_size_t PIN_LED_R = 17;
static const pin_size_t PIN_LED_G = 16;
static const pin_size_t PIN_LED_B = 25;

static const pin_size_t PIN_LED_BUILTIN = 25;

static CGamecubeConsole gamecube(PIN_GAMECUBE);
static jiangtun_board_rp2040_t board;

static jiangtun_t j;

void setup() {
    Serial.begin(115200);
    jiangtun_board_rp2040_init(&board, &Serial, &gamecube, PIN_SERVO,
                               PIN_RESET);
    jiangtun_init(
        &j, &(board.base),
        JIANGTUN_FEATURE_ENABLE_LED_BLINK | JIANGTUN_FEATURE_ENABLE_NXMC2 |
            JIANGTUN_FEATURE_ENABLE_POKECON | JIANGTUN_FEATURE_ENABLE_ORCA,
        JIANGTUN_LOG_LEVEL_DEBUG);
}

void loop() { jiangtun_loop(&j); }
