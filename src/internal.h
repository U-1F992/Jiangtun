#ifndef JIANGTUN_INTERNAL_H_
#define JIANGTUN_INTERNAL_H_

#include "pico/mutex.h"

#include "Bluewhale.h"

void gc_console_data_init(CGamecubeConsole *console, Gamecube_Data_t *gc, mutex_t *mtx)
{
    if (console == NULL ||
        gc == NULL ||
        mtx == NULL)
    {
        return;
    }

    mutex_enter_blocking(mtx);
    gc->report.a = 0;
    gc->report.b = 0;
    gc->report.x = 0;
    gc->report.y = 0;
    gc->report.start = 0;
    gc->report.dleft = 0;
    gc->report.dright = 0;
    gc->report.ddown = 0;
    gc->report.dup = 0;
    gc->report.z = 0;
    gc->report.r = 0;
    gc->report.l = 0;
    gc->report.xAxis = 128;
    gc->report.yAxis = 128;
    gc->report.cxAxis = 128;
    gc->report.cyAxis = 128;
    gc->report.left = 0;
    gc->report.right = 0;

    // Magic spell to make the controller be recognized by the Gamecube
    gc->report.start = 1;
    console->write(*gc);
    gc->report.start = 0;
    console->write(*gc);
    mutex_exit(mtx);
}

#endif // JIANGTUN_INTERNAL_H_