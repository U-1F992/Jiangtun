#ifndef JIANGTUN_H_
#define JIANGTUN_H_

#include <Arduino.h>
#include <Servo.h>

#include "pico/mutex.h"

#include "Bluewhale.h"

#include "nthaka.h"

typedef struct jiangtun_t
{
    pin_size_t _reset;
    Servo *_servo;
    size_t *_idx_servo_blocking;
    size_t _idx_servo_blocking_size;
    mutex_t *_mtx;

    nthaka_button_state_t _reset_state;
} jiangtun_t;

inline bool jiangtun_init(jiangtun_t *j, pin_size_t reset, Servo *servo, size_t idx_servo_blocking[], size_t size, mutex_t *mtx)
{
    if (j == NULL ||
        servo == NULL ||
        idx_servo_blocking == NULL ||
        mtx == NULL)
    {
        return false;
    }

    j->_reset = reset;
    j->_servo = servo;
    j->_idx_servo_blocking = idx_servo_blocking;
    j->_idx_servo_blocking_size = size;
    j->_mtx = mtx;

    j->_reset_state = NTHAKA_BUTTON_RELEASED;

    return true;
}

void jiangtun_update(jiangtun_t *j, nthaka_gamepad_state_t *s, size_t idx_from, Gamecube_Data_t *gc);

#endif // JIANGTUN_H_