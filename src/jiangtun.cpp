#include "jiangtun.h"

void jiangtun_update(jiangtun_t *j, nthaka_gamepad_state_t *s, size_t idx_from, Gamecube_Data_t *gc)
{
    if (j == NULL)
    {
        return;
    }

    mutex_enter_blocking(j->_mtx);

    gc->report.y = (uint8_t)s->y;
    gc->report.b = (uint8_t)s->b;
    gc->report.a = (uint8_t)s->a;
    gc->report.x = (uint8_t)s->x;
    gc->report.l = (uint8_t)s->l;
    gc->report.r = (uint8_t)s->r;
    gc->report.z = (uint8_t)s->zr;
    gc->report.start = (uint8_t)s->plus;

    bool servo_up = false;
    if (j->_reset_state != s->home)
    {
        if (s->home == NTHAKA_BUTTON_PRESSED)
        {
            j->_reset_state = NTHAKA_BUTTON_PRESSED;

            j->_servo->write(65);
            pinMode(j->_reset, OUTPUT);
            digitalWrite(j->_reset, LOW);

            // For listed idx, use servo with blocking
            for (size_t i = 0; i < j->_idx_servo_blocking_size; i++)
            {
                if (j->_idx_servo_blocking[i] == idx_from)
                {
                    servo_up = true;
                    break;
                }
            }
        }
        else
        {
            j->_reset_state = NTHAKA_BUTTON_RELEASED;

            j->_servo->write(90);
            pinMode(j->_reset, INPUT);
        }
    }

    switch (s->hat)
    {
    case NTHAKA_HAT_UP:
        gc->report.dup = 1;
        gc->report.dright = 0;
        gc->report.ddown = 0;
        gc->report.dleft = 0;
        break;
    case NTHAKA_HAT_UPRIGHT:
        gc->report.dup = 1;
        gc->report.dright = 1;
        gc->report.ddown = 0;
        gc->report.dleft = 0;
        break;
    case NTHAKA_HAT_RIGHT:
        gc->report.dup = 0;
        gc->report.dright = 1;
        gc->report.ddown = 0;
        gc->report.dleft = 0;
        break;
    case NTHAKA_HAT_DOWNRIGHT:
        gc->report.dup = 0;
        gc->report.dright = 1;
        gc->report.ddown = 1;
        gc->report.dleft = 0;
        break;
    case NTHAKA_HAT_DOWN:
        gc->report.dup = 0;
        gc->report.dright = 0;
        gc->report.ddown = 1;
        gc->report.dleft = 0;
        break;
    case NTHAKA_HAT_DOWNLEFT:
        gc->report.dup = 0;
        gc->report.dright = 0;
        gc->report.ddown = 1;
        gc->report.dleft = 1;
        break;
    case NTHAKA_HAT_LEFT:
        gc->report.dup = 0;
        gc->report.dright = 0;
        gc->report.ddown = 0;
        gc->report.dleft = 1;
        break;
    case NTHAKA_HAT_UPLEFT:
        gc->report.dup = 1;
        gc->report.dright = 0;
        gc->report.ddown = 0;
        gc->report.dleft = 1;
        break;
    case NTHAKA_HAT_NEUTRAL:
    default:
        gc->report.dup = 0;
        gc->report.dright = 0;
        gc->report.ddown = 0;
        gc->report.dleft = 0;
        break;
    }

    // There are a few games that do not handle yAxis=0 and cyAxis=0 correctly.
    gc->report.xAxis = s->l_stick.x;
    uint8_t y_axis = 0xFF - s->l_stick.y;
    gc->report.yAxis = y_axis == 0 ? 1 : y_axis;

    gc->report.cxAxis = s->r_stick.x;
    uint8_t cy_axis = 0xFF - s->r_stick.y;
    gc->report.cyAxis = cy_axis == 0 ? 1 : cy_axis;

    mutex_exit(j->_mtx);

    if (servo_up)
    {
        j->_reset_state = NTHAKA_BUTTON_RELEASED;

        delay(500);
        j->_servo->write(90);
        pinMode(j->_reset, INPUT);
        delay(500);
    }
}