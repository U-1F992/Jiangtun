#include "Nintendo.h"

// #define DEBUG

CGamecubeConsole GamecubeConsole1(5);
Gamecube_Data_t d = defaultGamecubeData;

const uint8_t BUTTONS_UPPER_ALL = 0b00111111;
const uint8_t HAT_NEUTRAL = 0x08;

uint8_t data[11];
int index = 0;
int timeout = 0;

void initialize_data()
{
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
}

void setup()
{
    initialize_data();

    // Omajinai
    d.report.start = 1;
    GamecubeConsole1.write(d);
    d.report.start = 0;
    GamecubeConsole1.write(d);

    Serial.begin(9600);
}

void clear()
{
    timeout = 0;
    // memset(data, 0, sizeof(data));
    index = 0;
}

bool validate_header()
{
    if (index != 0)
    {
        return true;
    }
    uint8_t header = data[index];
    bool ret = header == 0xAB;
    if (!ret)
    {
        Serial.print("Parse error: invalid header, ");
        Serial.print(header);
        Serial.print("\n");
    }
    return ret;
}

bool validate_buttons_upper()
{
    if (index != 2)
    {
        return true;
    }
    uint16_t buttons_upper = data[index];
    bool ret = buttons_upper <= BUTTONS_UPPER_ALL;
    if (!ret)
    {
        Serial.print("Parse error: invalid buttons_upper, ");
        Serial.print(buttons_upper);
        Serial.print("\n");
    }
    return ret;
}

bool validate_hat()
{
    if (index != 3)
    {
        return true;
    }
    uint8_t hat = data[index];
    bool ret = hat <= HAT_NEUTRAL;
    if (!ret)
    {
        Serial.print("Parse error: invalid hat, ");
        Serial.print(hat);
        Serial.print("\n");
    }
    return ret;
}

bool validate_data()
{
    return validate_header() && validate_buttons_upper() && validate_hat();
}

void convert_buttons(uint16_t buttons)
{
    d.report.y = (buttons & 0b0000000000000001);
    d.report.b = (buttons & 0b0000000000000010) >> 1;
    d.report.a = (buttons & 0b0000000000000100) >> 2;
    d.report.x = (buttons & 0b0000000000001000) >> 3;
    d.report.l = (buttons & 0b0000000000010000) >> 4;
    d.report.r = (buttons & 0b0000000000100000) >> 5;
    d.report.z = (buttons & 0b0000000010000000) >> 7;
    d.report.start = (buttons & 0b0000001000000000) >> 9;
}

void convert_hat(uint8_t hat)
{
    switch (hat)
    {
    case 0x00:
        d.report.dup = 1;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;

    case 0x01:
        d.report.dup = 1;
        d.report.dright = 1;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;

    case 0x02:
        d.report.dup = 0;
        d.report.dright = 1;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;

    case 0x03:
        d.report.dup = 0;
        d.report.dright = 1;
        d.report.ddown = 1;
        d.report.dleft = 0;
        break;

    case 0x04:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 1;
        d.report.dleft = 0;
        break;

    case 0x05:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 1;
        d.report.dleft = 1;
        break;

    case 0x06:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 1;
        break;

    case 0x07:
        d.report.dup = 1;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 1;
        break;

    case 0x08:
        d.report.dup = 0;
        d.report.dright = 0;
        d.report.ddown = 0;
        d.report.dleft = 0;
        break;
    }
}

void convert_axis(uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry)
{
    d.report.xAxis = lx;
    d.report.yAxis = 0xFF - ly;
    d.report.cxAxis = rx;
    d.report.cyAxis = 0xFF - ry;
}

#ifdef DEBUG
void pretty_print_nx(uint8_t header, uint16_t buttons, uint8_t hat, uint8_t lx, uint8_t ly, uint8_t rx, uint8_t ry, uint8_t ext0, uint8_t ext1, uint8_t ext2)
{
    uint8_t y = (buttons & 0b0000000000000001);
    uint8_t b = (buttons & 0b0000000000000010) >> 1;
    uint8_t a = (buttons & 0b0000000000000100) >> 2;
    uint8_t x = (buttons & 0b0000000000001000) >> 3;
    uint8_t l = (buttons & 0b0000000000010000) >> 4;
    uint8_t r = (buttons & 0b0000000000100000) >> 5;
    uint8_t zl = (buttons & 0b0000000001000000) >> 6;
    uint8_t zr = (buttons & 0b0000000010000000) >> 7;
    uint8_t plus = (buttons & 0b0000000100000000) >> 8;
    uint8_t minus = (buttons & 0b0000001000000000) >> 9;
    uint8_t lclick = (buttons & 0b0000010000000000) >> 10;
    uint8_t rclick = (buttons & 0b0000100000000000) >> 11;
    uint8_t home = (buttons & 0b0001000000000000) >> 12;
    uint8_t capture = (buttons & 0b0010000000000000) >> 13;

    char buffer[256];
    sprintf(
        buffer,
        "{\"header\":%d,\"y\":%d,\"b\":%d,\"a\":%d,\"x\":%d,\"l\":%d,\"r\":%d,\"zl\":%d,\"zr\":%d,\"plus\":%d,\"minus\":%d,\"lclick\":%d,\"rclick\":%d,\"home\":%d,\"capture\":%d,\"lx\":%d,\"ly\":%d,\"rx\":%d,\"ry\":%d,\"ext0\":%d,\"ext1\":%d,\"ext2\":%d}",
        header,
        y,
        b,
        a,
        x,
        l,
        r,
        zl,
        zr,
        plus,
        minus,
        lclick,
        rclick,
        home,
        capture,
        lx,
        ly,
        rx,
        ry,
        ext0,
        ext1,
        ext2);
    Serial.println(buffer);
}

uint8_t previous[8];

void pretty_print_report()
{
    if (memcmp(previous, d.report.raw8, sizeof(previous)) == 0)
    {
        return;
    }
    char buffer[256];
    sprintf(
        buffer,
        "{\"a\":%d,\"b\":%d,\"x\":%d,\"y\":%d,\"start\":%d,\"origin\":%d,\"errlatch\":%d,\"errstat\":%d,\"dleft\":%d,\"dright\":%d,\"ddown\":%d,\"dup\":%d,\"z\":%d,\"r\":%d,\"l\":%d,\"high1\":%d,\"xAxis\":%d,\"yAxis\":%d,\"cxAxis\":%d,\"cyAxis\":%d,\"left\":%d,\"right\":%d}",
        d.report.a,
        d.report.b,
        d.report.x,
        d.report.y,
        d.report.start,
        d.report.origin,
        d.report.errlatch,
        d.report.errstat,
        d.report.dleft,
        d.report.dright,
        d.report.ddown,
        d.report.dup,
        d.report.z,
        d.report.r,
        d.report.l,
        d.report.high1,
        d.report.xAxis,
        d.report.yAxis,
        d.report.cxAxis,
        d.report.cyAxis,
        d.report.left,
        d.report.right);
    Serial.println(buffer);
    memcpy(previous, d.report.raw8, sizeof(previous));
}
#endif

void convert_data()
{
    uint8_t header = data[0];
    uint16_t buttons = data[1] | (data[2] << 8);
    uint8_t hat = data[3];
    uint8_t lx = data[4];
    uint8_t ly = data[5];
    uint8_t rx = data[6];
    uint8_t ry = data[7];

    // not in use for gamecube
    uint8_t ext0 = data[8];
    uint8_t ext1 = data[9];
    uint8_t ext2 = data[10];

    convert_buttons(buttons);
    convert_hat(hat);
    convert_axis(lx, ly, rx, ry);

#ifdef DEBUG
    pretty_print_nx(header, buttons, hat, lx, ly, rx, ry, ext0, ext1, ext2);
    pretty_print_report();
#endif
}

void send_data()
{
    bool ret = GamecubeConsole1.write(d);
    if (!ret)
    {
        Serial.println("Send error: not connected");
    }
}

void loop()
{
    if (Serial.available())
    {
        timeout = 0;
        data[index] = Serial.read();
        if (!validate_data())
        {
            clear();
            return;
        }
        index++;

        if (index == 11)
        {
            convert_data();
            send_data();
        }
    }
    else if (1000 < timeout)
    {
        Serial.print("Parse error: timeout");
        clear();
    }
    else
    {
        timeout++;
    }
}
