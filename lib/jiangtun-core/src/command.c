#include <jiangtun/command.h>

#include <assert.h>

static void transition_default(jiangtun_command_t *command,
                               jiangtun_uint8_t c) {
    assert(command != NULL);

    (void)c;
    jiangtun_command_init(command, NULL, NULL);
}

static jiangtun_bool_t action_default(const jiangtun_uint8_t *buffer,
                                      size_t length, void *context) {
    (void)buffer;
    (void)length;
    (void)context;

    return JIANGTUN_FALSE;
}

static jiangtun_bool_t jiangtun_terminal(jiangtun_command_t *command) {
    assert(command != NULL);

    return command->transition == transition_default ||
           /* for safety */ command->transition == NULL;
}

jiangtun_bool_t jiangtun_command_pending(jiangtun_command_t *command) {
    assert(command != NULL);

    return !jiangtun_terminal(command) &&
           (command->action == action_default ||
            /* for safety */ command->action == NULL);
}

jiangtun_bool_t jiangtun_command_accepted(jiangtun_command_t *command) {
    assert(command != NULL);

    return command->action != action_default &&
           /* for safety */ command->action != NULL;
}

jiangtun_bool_t jiangtun_command_rejected(jiangtun_command_t *command) {
    assert(command != NULL);

    return jiangtun_terminal(command) &&
           (command->action == action_default ||
            /* for safety */ command->action == NULL);
}

void jiangtun_command_init(jiangtun_command_t *command,
                           jiangtun_transition_t transition,
                           jiangtun_action_t action) {
    assert(command != NULL);

    command->transition = transition != NULL ? transition : transition_default;
    command->action = action != NULL ? action : action_default;
}

void append_if_not_rejected(jiangtun_command_t *command, jiangtun_uint8_t c,
                            jiangtun_transition_t transition,
                            jiangtun_action_t action) {
    assert(command != NULL);

    jiangtun_command_init(command, transition, action);
    if (!jiangtun_command_rejected(command)) {
        command->buffer[command->length++] = c;
    }
}

void jiangtun_command_push(jiangtun_command_t *command, jiangtun_uint8_t c) {
    assert(command != NULL);

    if (jiangtun_terminal(command)) {
        jiangtun_command_init(command, NULL, NULL);
    }
    command->transition(command, c);
}

jiangtun_bool_t jiangtun_command_run(jiangtun_command_t *command,
                                     void *context) {
    assert(command != NULL);

    return jiangtun_command_accepted(command)
               ? command->action(command->buffer, command->length, context)
               : JIANGTUN_FALSE;
}