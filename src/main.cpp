#include <Arduino.h>
#include <Servo.h>

#include <Adafruit_NeoPixel.h>

#include <Bluewhale.h>
#include <jiangtun.h>

static const pin_size_t PIN_GAMECUBE = 7; // D5 (SCL)
static const pin_size_t PIN_SERVO = 0;    // D6 (TX)
static const pin_size_t PIN_RESET = 3;    // D10 (MOSI)

static const pin_size_t PIN_NEOPIXEL = 12;
static const pin_size_t PIN_NEOPIXEL_POWER = 11;
static const pin_size_t PIN_LED_R = 17;
static const pin_size_t PIN_LED_G = 16;
static const pin_size_t PIN_LED_B_BUILTIN = 25;

static CGamecubeConsole gamecube(PIN_GAMECUBE);
static Servo servo;
static Gamecube_Data_t gamecube_data = defaultGamecubeData;
static bool gamecube_data_reset = false;
static mutex_t gamecube_data_mtx;
static bool current_reset_state = true; // to ensure initial releasing

static Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
static jiangtun_board_t board;
static jiangtun_t j;

static jiangtun_bool_t
jiangtun_board_rp2040_serial_getc(jiangtun_board_t *board, unsigned char *c) {
    int c_ = 0;
    assert(board != NULL);
    assert(c != NULL);

    if (Serial.available() <= 0) {
        return JIANGTUN_FALSE;
    }
    c_ = Serial.read();
    if (c_ < 0 || 255 < c_) {
        return JIANGTUN_FALSE;
    }
    *c = c_ & 0xFF;
    return JIANGTUN_TRUE;
}

static void jiangtun_board_rp2040_serial_puts(jiangtun_board_t *board,
                                              const char *s) {
    assert(board != NULL);
    assert(s != NULL);

    Serial.print(s);
}

static jiangtun_bool_t
jiangtun_board_rp2040_gamecube_send(jiangtun_board_t *board,
                                    jiangtun_report_mode3_t *report) {
    assert(board != NULL);
    assert(report != NULL);

    mutex_enter_blocking(&gamecube_data_mtx);
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
    mutex_exit(&gamecube_data_mtx);

    return JIANGTUN_TRUE;
}

static void jiangtun_board_rp2040_led_set(jiangtun_board_t *board,
                                          jiangtun_bool_t state) {
    assert(board != NULL);

    digitalWrite(PIN_LED_B_BUILTIN, state ? HIGH : LOW);
    if (state) {
        uint16_t hue = (millis() % 2000) * (65535 / 2000);
        pixels.setPixelColor(0, Adafruit_NeoPixel::ColorHSV(hue));
    } else {
        pixels.clear();
    }
    pixels.show();

#ifndef NDEBUG
    // Calibrate `JIANGTUN_MICROSECONDS_PER_LOOP`
    // to make this duration approximately 100 ms.
    static unsigned long led_on = 0;
    if (state) {
        led_on = micros();
    } else {
        char buffer[128];
        sprintf(buffer, "led_on duration: %lu\n", micros() - led_on);
        Serial.print(buffer);
    }
#endif
}

void setup() {
    Serial.begin(115200);

    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    // pinMode(PIN_LED_B_BUILTIN, OUTPUT);
    digitalWrite(PIN_LED_R, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    // digitalWrite(PIN_LED_B_BUILTIN, HIGH);

    pinMode(PIN_NEOPIXEL_POWER, OUTPUT);
    digitalWrite(PIN_NEOPIXEL_POWER, HIGH);
    pixels.begin();

    pinMode(PIN_LED_B_BUILTIN, OUTPUT);
    digitalWrite(PIN_LED_B_BUILTIN, LOW);

    jiangtun_board_init(&board, jiangtun_board_rp2040_serial_getc,
                        jiangtun_board_rp2040_serial_puts,
                        jiangtun_board_rp2040_gamecube_send,
                        jiangtun_board_rp2040_led_set);
    mutex_init(&gamecube_data_mtx);
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

void setup1() {
    servo.attach(PIN_SERVO, 500, 2400);
    while (!mutex_is_initialized(&gamecube_data_mtx))
        ;
}

void loop1() {
    mutex_enter_blocking(&gamecube_data_mtx);
    bool ret = gamecube.write(gamecube_data);
    bool reset = gamecube_data_reset;
    mutex_exit(&gamecube_data_mtx);
    if (!ret) {
        Serial.println("[core2]\tfailed to send report");
    }
    if (!(current_reset_state) && reset) {
        servo.write(65);
        pinMode(PIN_RESET, OUTPUT);
        digitalWrite(PIN_RESET, LOW);
        current_reset_state = true;

    } else if (current_reset_state && !reset) {
        servo.write(90);
        pinMode(PIN_RESET, INPUT);
        current_reset_state = true;
    }
}