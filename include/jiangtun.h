#ifndef JIANGTUN_H_
#define JIANGTUN_H_

#include <Servo.h>

#include "pico/mutex.h"

#include "Bluewhale.h"

#include "nthaka.h"

typedef struct jiangtun_t
{
    Servo *_servo;
    size_t *_idx_using_servo;
    size_t _idx_using_servo_size;
    mutex_t *_mtx;

    nthaka_button_state_t _reset_state;
} jiangtun_t;

inline bool jiangtun_init(jiangtun_t *j, Servo *servo, size_t idx_using_servo[], size_t size, mutex_t *mtx)
{
    if (j == NULL ||
        servo == NULL ||
        idx_using_servo == NULL ||
        mtx == NULL)
    {
        return false;
    }

    j->_servo = servo;
    j->_idx_using_servo = idx_using_servo;
    j->_idx_using_servo_size = size;
    j->_mtx = mtx;

    j->_reset_state = NTHAKA_BUTTON_RELEASED;

    return true;
}

void jiangtun_update(jiangtun_t *j, nthaka_gamepad_state_t *s, size_t idx_from, Gamecube_Data_t *gc);

#endif // JIANGTUN_H_