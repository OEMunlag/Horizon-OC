#pragma once

#include <switch.h>  // for Service, Result, hosversionBefore(), smGetService(), serviceClose(), etc.
#include "rgltr.h"   // for RgltrSession, PowerDomainId, etc.

extern Service g_rgltrSrv;

Result rgltrInitialize(void);
void   rgltrExit(void);

Result rgltrOpenSession(RgltrSession* session_out, PowerDomainId module_id);

Result rgltrGetVoltage(RgltrSession* session, u32* out_volt);

void   rgltrCloseSession(RgltrSession* session);
