#include <jiangtun/report.h>

#include <assert.h>

void jiangtun_report_init(jiangtun_report_mode3_t *report) {
    assert(report != NULL);

    report->a = JIANGTUN_FALSE;
    report->b = JIANGTUN_FALSE;
    report->x = JIANGTUN_FALSE;
    report->y = JIANGTUN_FALSE;
    report->start = JIANGTUN_FALSE;
    report->dleft = JIANGTUN_FALSE;
    report->dright = JIANGTUN_FALSE;
    report->ddown = JIANGTUN_FALSE;
    report->dup = JIANGTUN_FALSE;
    report->z = JIANGTUN_FALSE;
    report->r = JIANGTUN_FALSE;
    report->l = JIANGTUN_FALSE;
    report->xAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->yAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->cxAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->cyAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->left = 0;
    report->right = 0;

    report->reset = JIANGTUN_FALSE;
}

#define EMEND_AXIS(axis)                                                       \
    ((axis) = (axis) == 0 ? 1 : (axis) == 255 ? 254 : (axis))

void jiangtun_emend_axis(jiangtun_report_mode3_t *report) {
    assert(report != NULL);

    EMEND_AXIS(report->xAxis);
    EMEND_AXIS(report->yAxis);
    EMEND_AXIS(report->cxAxis);
    EMEND_AXIS(report->cyAxis);
}