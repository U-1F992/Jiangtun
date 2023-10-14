#include "orca.h"

#include <stdlib.h>

// - 0x40（'@'）のみ
// - {0x80, A, B}、ただし0xFFでない

static bool orca_protocol_is_acceptable(NxamfBytesProtocolInterface *self, const uint8_t packet, const uint8_t buffer[], const size_t length)
{
    return (length == 0 && (packet == (uint8_t)'@' || packet == 0x80)) ||
           ((0 < length && length <= 2) && packet != 0xFF);
}

static bool orca_protocol_is_ready(NxamfBytesProtocolInterface *self, const uint8_t buffer[], const size_t length)
{
    return (length == 1 && buffer[0] == (uint8_t)'@') ||
           (length == 3 && buffer[0] == 0x80 && buffer[1] != 0xFF && buffer[2] != 0xFF);
}

static void orca_protocol_convert(NxamfBytesProtocolInterface *self, const uint8_t buffer[], const size_t length, NxamfGamepadState *state)
{
    if (length == 1)
    {
        state->home = NXAMF_BUTTON_STATE_PRESSED;
        return;
    }

    if (buffer[1] == 0x80)
    {
        state->a = NXAMF_BUTTON_STATE_RELEASED;
        state->b = NXAMF_BUTTON_STATE_RELEASED;
        state->x = NXAMF_BUTTON_STATE_RELEASED;
        state->y = NXAMF_BUTTON_STATE_RELEASED;
        state->l = NXAMF_BUTTON_STATE_RELEASED;
        state->r = NXAMF_BUTTON_STATE_RELEASED;
    }
    else
    {
        state->a = (NxamfButtonState)(buffer[1] & 1);
        state->b = (NxamfButtonState)((buffer[1] >> 1) & 1);
        state->x = (NxamfButtonState)((buffer[1] >> 2) & 1);
        state->y = (NxamfButtonState)((buffer[1] >> 3) & 1);
        state->l = (NxamfButtonState)((buffer[1] >> 4) & 1);
        state->r = (NxamfButtonState)((buffer[1] >> 5) & 1);
    }

    uint8_t dleft = 0;
    uint8_t dright = 0;
    uint8_t dup = 0;
    uint8_t ddown = 0;

    if (buffer[2] == 0x80)
    {
        state->zr = NXAMF_BUTTON_STATE_RELEASED;
        state->plus = NXAMF_BUTTON_STATE_RELEASED;
        dleft = 0;
        dright = 0;
        dup = 0;
        ddown = 0;
    }
    else
    {
        state->zr = (NxamfButtonState)(buffer[2] & 1);
        state->plus = (NxamfButtonState)((buffer[2] >> 1) & 1);
        dleft = (buffer[2] >> 2) & 1;
        dright = (buffer[2] >> 3) & 1;
        dup = (buffer[2] >> 4) & 1;
        ddown = (buffer[2] >> 5) & 1;
    }

    switch (dup << 3 | dright << 2 | ddown << 1 | dleft)
    {
    case 0b1000:
        state->hat = NXAMF_HAT_STATE_UP;
        break;
    case 0b1100:
        state->hat = NXAMF_HAT_STATE_UPRIGHT;
        break;
    case 0b0100:
        state->hat = NXAMF_HAT_STATE_RIGHT;
        break;
    case 0b0110:
        state->hat = NXAMF_HAT_STATE_DOWNRIGHT;
        break;
    case 0b0010:
        state->hat = NXAMF_HAT_STATE_DOWN;
        break;
    case 0b0011:
        state->hat = NXAMF_HAT_STATE_DOWNLEFT;
        break;
    case 0b0001:
        state->hat = NXAMF_HAT_STATE_LEFT;
        break;
    case 0b1001:
        state->hat = NXAMF_HAT_STATE_UPLEFT;
        break;
    default:
        state->hat = NXAMF_HAT_STATE_NEUTRAL;
        break;
    }
}

OrcaProtocol *orca_protocol_new(void)
{
    OrcaProtocol *self = (OrcaProtocol *)malloc(sizeof(OrcaProtocol));
    if (self == NULL)
    {
        return NULL;
    }

    self->parent.is_acceptable = orca_protocol_is_acceptable;
    self->parent.is_ready = orca_protocol_is_ready;
    self->parent.convert = orca_protocol_convert;

    return self;
}

void orca_protocol_delete(OrcaProtocol *self)
{
    if (self == NULL)
    {
        return;
    }

    free(self);
}