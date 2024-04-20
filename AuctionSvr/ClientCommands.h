#ifndef C2S_COMMAND
#define C2S_COMMAND(__NAME__, __COMMAND__)
#endif

#ifndef S2C_COMMAND 
#define S2C_COMMAND(__NAME__, __COMMAND__)
#endif

#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__) \
__NAMESPACE__ ## _COMMAND(__NAME__, __COMMAND__)
#include "ClientProtocolDefinition.h"

#undef S2C_COMMAND
#undef C2S_COMMAND
