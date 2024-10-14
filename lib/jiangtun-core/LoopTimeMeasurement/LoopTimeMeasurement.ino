#include <Arduino.h>

#include <stdio.h>

static unsigned long start_time = 0;
static unsigned int loop_count = 0;

void setup(void) {
  Serial.begin();
  start_time = micros();
}

void loop_(void) {
}

void loop(void) {
  loop_();

  if (loop_count++ >= 1000) {
    unsigned long total_time = micros() - start_time;
    double average_time = (double)total_time / 1000.0;
    char buffer[256];

    sprintf(buffer, "Average time per loop: %lf\n", average_time);
    Serial.print(buffer);

    start_time = micros();
    loop_count = 0;
  }
}
