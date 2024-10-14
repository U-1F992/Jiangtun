#include <Arduino.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#include <Bluewhale.h>
#include <jiangtun.h>

static const pin_size_t PIN_GAMECUBE = 7; // D5 (SCL)
static const pin_size_t PIN_SERVO = 0;    // D6 (TX)
static const pin_size_t PIN_RESET = 3;    // D10 (MOSI)

static const pin_size_t PIN_XIAO_NEOPIXEL = 12;
static const pin_size_t PIN_XIAO_NEOPIXEL_POWER = 11;
static const pin_size_t PIN_XIAO_LED_R = 17;
static const pin_size_t PIN_XIAO_LED_G = 16;
/**
 * GPIO25 is connected to the anode of the built-in LED on the Pico (lit
 * when HIGH), and to the cathode of the blue channel of the RGB LED on the
 * XIAO RP2040 (lit when LOW).
 * This firmware prioritizes blinking the LED on the Pico, so on the XIAO
 * RP2040, the blue LED will remain on and turn off when input is accepted.
 * As a replacement on the XIAO RP2040, the NeoPixel, which is brighter than
 * the RGB LED, will light up.
 */
static const pin_size_t PIN_XIAO_LED_B_PICO_LED_BUILTIN = 25;

static CGamecubeConsole gamecube(PIN_GAMECUBE);
static Servo servo;
static Gamecube_Data_t gamecube_data = defaultGamecubeData;
static bool gamecube_data_reset = false;
static bool current_reset_state = true; // to ensure initial releasing

static Adafruit_NeoPixel pixels(1, PIN_XIAO_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static jiangtun_board_t board;
static jiangtun_t j;

static jiangtun_bool_t serial_getc(jiangtun_board_t *board, unsigned char *c) {
    assert(board != NULL);
    assert(c != NULL);

    if (Serial.available() <= 0) {
        return JIANGTUN_FALSE;
    }
    int c_ = Serial.read();
    if (c_ < 0 || 255 < c_) {
        return JIANGTUN_FALSE;
    }
    *c = c_ & 0xFF;
    return JIANGTUN_TRUE;
}

static void serial_puts(jiangtun_board_t *board, const char *s) {
    assert(board != NULL);
    assert(s != NULL);

    Serial.print(s);
}

static jiangtun_bool_t gamecube_send(jiangtun_board_t *board,
                                     jiangtun_bool_t changed,
                                     jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    if (changed) {
        gamecube_data.report.a = report->a ? 1 : 0;
        gamecube_data.report.b = report->b ? 1 : 0;
        gamecube_data.report.x = report->x ? 1 : 0;
        gamecube_data.report.y = report->y ? 1 : 0;
        gamecube_data.report.start = report->start ? 1 : 0;
        gamecube_data.report.dleft = report->dleft ? 1 : 0;
        gamecube_data.report.dright = report->dright ? 1 : 0;
        gamecube_data.report.ddown = report->ddown ? 1 : 0;
        gamecube_data.report.dup = report->dup ? 1 : 0;
        gamecube_data.report.z = report->z ? 1 : 0;
        gamecube_data.report.r = report->r ? 1 : 0;
        gamecube_data.report.l = report->l ? 1 : 0;
        gamecube_data.report.xAxis = (uint8_t)report->xAxis;
        gamecube_data.report.yAxis = (uint8_t)report->yAxis;
        gamecube_data.report.cxAxis = (uint8_t)report->cxAxis;
        gamecube_data.report.cyAxis = (uint8_t)report->cyAxis;
        gamecube_data.report.left = (uint8_t)report->left;
        gamecube_data.report.right = (uint8_t)report->right;

        gamecube_data_reset = report->reset ? true : false;
    }

    bool ret = gamecube.write(gamecube_data);

    if (!(current_reset_state) && gamecube_data_reset) {
        servo.write(65);
        pinMode(PIN_RESET, OUTPUT);
        digitalWrite(PIN_RESET, LOW);
    } else if (current_reset_state && !gamecube_data_reset) {
        servo.write(90);
        pinMode(PIN_RESET, INPUT);
    }
    current_reset_state = gamecube_data_reset;

    return JIANGTUN_TRUE;
}

static void led_set(jiangtun_board_t *board, jiangtun_bool_t state) {
    assert(board != NULL);

    digitalWrite(PIN_XIAO_LED_B_PICO_LED_BUILTIN, state ? HIGH : LOW);
    if (state) {
        uint16_t hue = (millis() % 2000) * (65535 / 2000);
        pixels.setPixelColor(0, Adafruit_NeoPixel::ColorHSV(hue));
    } else {
        pixels.clear();
    }
    pixels.show();
}

static jiangtun_uint32_t get_millis(jiangtun_board_t *board) {
    assert(board != NULL);

    return (jiangtun_uint32_t)(millis() % JIANGTUN_UINT32_MAX);
}

void setup() {
    Serial.begin(115200);

    servo.attach(PIN_SERVO, 500, 2400);

    pinMode(PIN_XIAO_LED_R, OUTPUT);
    pinMode(PIN_XIAO_LED_G, OUTPUT);
    digitalWrite(PIN_XIAO_LED_R, HIGH);
    digitalWrite(PIN_XIAO_LED_G, HIGH);

    pinMode(PIN_XIAO_LED_B_PICO_LED_BUILTIN, OUTPUT);
    digitalWrite(PIN_XIAO_LED_B_PICO_LED_BUILTIN, LOW);

    pinMode(PIN_XIAO_NEOPIXEL_POWER, OUTPUT);
    digitalWrite(PIN_XIAO_NEOPIXEL_POWER, HIGH);
    pixels.begin();
    pixels.clear();
    pixels.show();

    jiangtun_board_init(&board, serial_getc, serial_puts, gamecube_send,
                        led_set, get_millis);
    jiangtun_init(
        &j, &board,
        JIANGTUN_FEATURE_ENABLE_LED_BLINK | JIANGTUN_FEATURE_ENABLE_NXMC2 |
            JIANGTUN_FEATURE_ENABLE_POKECON | JIANGTUN_FEATURE_ENABLE_ORCA,
#ifndef NDEBUG
        JIANGTUN_LOG_LEVEL_DEBUG
#else
        JIANGTUN_LOG_LEVEL_INFO
#endif
    );
}

void loop() { jiangtun_loop(&j); }