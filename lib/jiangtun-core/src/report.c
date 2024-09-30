#include <jiangtun/report.h>

jiangtun_error_t jiangtun_report_init(jiangtun_report_t *report) {
    if (report == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    }

    report->mode3.a = 0;
    report->mode3.b = 0;
    report->mode3.x = 0;
    report->mode3.y = 0;
    report->mode3.start = 0;
    report->mode3.dleft = 0;
    report->mode3.dright = 0;
    report->mode3.ddown = 0;
    report->mode3.dup = 0;
    report->mode3.z = 0;
    report->mode3.r = 0;
    report->mode3.l = 0;
    report->mode3.xAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->mode3.yAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->mode3.cxAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->mode3.cyAxis = JIANGTUN_REPORT_STICK_NEUTRAL;
    report->mode3.left = 0;
    report->mode3.right = 0;

    return JIANGTUN_SUCCESS;
}

#define EMEND_AXIS(axis)                                                       \
    ((axis) = (axis) == 0 ? 1 : (axis) == 255 ? 254 : (axis))

jiangtun_error_t jiangtun_report_emend_axis(jiangtun_report_t *report) {
    if (report == NULL) {
        return JIANGTUN_ERROR_NULL_POINTER;
    }

    EMEND_AXIS(report->mode3.xAxis);
    EMEND_AXIS(report->mode3.yAxis);
    EMEND_AXIS(report->mode3.cxAxis);
    EMEND_AXIS(report->mode3.cyAxis);

    return JIANGTUN_SUCCESS;
}