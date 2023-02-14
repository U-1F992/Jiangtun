#include "Nintendo.h"

CGamecubeConsole GamecubeConsole1(5);
Gamecube_Data_t d = defaultGamecubeData;

void setup()
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

  // Omajinai
  d.report.start = 1;
  GamecubeConsole1.write(d);
  d.report.start = 0;
  GamecubeConsole1.write(d);

  Serial.begin(9600);
}

void convert_btns(uint16_t btns)
{
  // NX GC
  // Y  y
  // B  b
  // A  a
  // X  x
  // L  l
  // R  r
  // ZL (none)
  // ZR z
  // -  (none)
  // +  start
  // L Click (none)
  // R Click (none)
  // Home    (none)
  // Capture (none)
  d.report.y     = btns & 0b0000000000000001;
  d.report.b     = btns & 0b0000000000000010;
  d.report.a     = btns & 0b0000000000000100;
  d.report.x     = btns & 0b0000000000001000;
  d.report.l     = btns & 0b0000000000010000;
  d.report.r     = btns & 0b0000000000100000;
  d.report.z     = btns & 0b0000000010000000;
  d.report.start = btns & 0b0000001000000000;
}

void convert_hat(uint8_t hat)
{
  // 0x00  ↑
  // 0x01  ↗
  // 0x02  →
  // 0x03  ↘
  // 0x04  ↓
  // 0x05  ↙
  // 0x06  ←
  // 0x07  ↖
  // 0x08  -
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
  d.report.yAxis = ly;
  d.report.cxAxis = rx;
  d.report.cyAxis = ry;
}

void update_data()
{
  if (!Serial.available())
  {
    return;
  }
  
  uint8_t head = Serial.read();
  if (head != 0xAB)
  {
    return;
  }

  uint8_t lower = Serial.read();
  uint8_t upper = Serial.read();
  uint16_t btns = lower | (upper << 8);
  if (0b0011111111111111 < btns)
  {
    return;
  }

  uint8_t hat = Serial.read();
  if (0x08 < hat)
  {
    return;
  }

  uint8_t lx = Serial.read();
  uint8_t ly = Serial.read();
  uint8_t rx = Serial.read();
  uint8_t ry = Serial.read();

  // not in use
  Serial.read();
  Serial.read();
  Serial.read();
  
  convert_btns(btns);
  convert_hat(hat);
  convert_axis(lx, ly, rx, ry);
}

void loop()
{
  update_data();
  if(!GamecubeConsole1.write(d))
  {
    Serial.println("not connected...");
  }
}
