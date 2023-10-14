#ifndef ORCA_H_
#define ORCA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "nxamf.h"

typedef struct OrcaProtocol
{
    NxamfBytesProtocolInterface parent;
} OrcaProtocol;

OrcaProtocol *orca_protocol_new(void);
void orca_protocol_delete(OrcaProtocol *self);

#ifdef __cplusplus
}
#endif

#endif // ORCA_H_