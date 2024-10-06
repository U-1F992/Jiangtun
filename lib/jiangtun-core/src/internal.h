#ifndef JIANGTUN_INTERNAL_H
#define JIANGTUN_INTERNAL_H

#include <jiangtun/command.h>

#ifdef __cplusplus
extern "C" {
#endif

void append_if_not_rejected(jiangtun_command_t *, jiangtun_uint8_t,
                            jiangtun_transition_t, jiangtun_action_t);

#ifdef __cplusplus
}
#endif

#endif /* JIANGTUN_INTERNAL_H */