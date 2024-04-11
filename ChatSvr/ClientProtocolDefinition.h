#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>
#include "Constants.h"

#ifndef CLIENT_PROTOCOL_ENUM
#define CLIENT_PROTOCOL_ENUM(...)
#endif

#ifndef CLIENT_PROTOCOL_STRUCT
#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)
#endif

#ifndef CLIENT_PROTOCOL
#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)
#endif

#undef CLIENT_PROTOCOL_ENUM
#undef CLIENT_PROTOCOL_STRUCT
#undef CLIENT_PROTOCOL
