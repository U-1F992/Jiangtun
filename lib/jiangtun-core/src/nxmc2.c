#include <jiangtun/parse.h>

jiangtun_error_t jiangtun_nxmc2_parse(const jiangtun_uint8_t *buffer,
                                      size_t buffer_length,
                                      jiangtun_report_t *out_report,
                                      jiangtun_bool_t *out_reset) {
    jiangtun_uint8_t btns_lsb = 0;
    jiangtun_uint8_t btns_msb = 0;
    jiangtun_uint8_t hat = 8;

    if (buffer == NULL) {
        return JIANGTUN_ERROR_INVALID_ARGUMENT;
    } else if (buffer_length != 11 || buffer[0] != 0xAB) {
        return JIANGTUN_ERROR_PARSE_FAILURE;
    }

    btns_lsb = buffer[1];
    btns_msb = buffer[2];
    hat = buffer[3] % 9;

    if (out_report != NULL) {
        if (jiangtun_report_init(out_report) != JIANGTUN_SUCCESS) {
            return JIANGTUN_ERROR_INTERNAL;
        }

        out_report->mode3.y = (btns_lsb >> 0) & 1;
        out_report->mode3.b = (btns_lsb >> 1) & 1;
        out_report->mode3.a = (btns_lsb >> 2) & 1;
        out_report->mode3.x = (btns_lsb >> 3) & 1;
        out_report->mode3.l = (btns_lsb >> 4) & 1;
        out_report->mode3.r = (btns_lsb >> 5) & 1;
        /* zl = (btns_lsb >> 6) & 1; */
        out_report->mode3.z = (btns_msb >> 7) & 1;
        /* minus = (btns_msb >> 0) & 1; */
        out_report->mode3.start = (btns_msb >> 1) & 1;
        /* l_click = (btns_msb >> 2) & 1; */
        /* r_click = (btns_msb >> 3) & 1; */
        /* home = (btns_msb >> 4) & 1; --> reset */
        /* capture = (btns_msb >> 5) & 1; */

        switch (hat) {
        case 0:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 1:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 2:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        case 3:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 1;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 0;
            break;
        case 4:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 0;
            break;
        case 5:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 1;
            out_report->mode3.dleft = 1;
            break;
        case 6:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 1;
            break;
        case 7:
            out_report->mode3.dup = 1;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 1;
            break;
        case 8:
        default:
            out_report->mode3.dup = 0;
            out_report->mode3.dright = 0;
            out_report->mode3.ddown = 0;
            out_report->mode3.dleft = 0;
            break;
        }

        out_report->mode3.xAxis = buffer[4];
        out_report->mode3.yAxis = 0xFF - buffer[5];

        out_report->mode3.cxAxis = buffer[6];
        out_report->mode3.cyAxis = 0xFF - buffer[7];

        (void)buffer[8];
        (void)buffer[9];
        (void)buffer[10];

        if (jiangtun_report_emend_axis(out_report) != JIANGTUN_SUCCESS) {
            return JIANGTUN_ERROR_INTERNAL;
        }
    }
    if (out_reset != NULL) {
        *out_reset = (jiangtun_bool_t)((btns_msb >> 4) & 1);
    }

    return JIANGTUN_SUCCESS;
}
