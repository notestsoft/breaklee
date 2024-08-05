#pragma once

#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"
#include "Context.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define CLIENT_PROTOCOL_ENUM(...) \
enum {                            \
    __VA_ARGS__                   \
};
#include "ClientProtocolDefinition.h"

#define CLIENT_PROTOCOL_STRUCT(__NAME__, __BODY__)      \
typedef struct {                                        \
    __BODY__                                            \
} __NAME__;
#include "ClientProtocolDefinition.h"

#define CLIENT_PROTOCOL(__NAMESPACE__, __NAME__, __SIGNATURE__, __COMMAND__, __BODY__)  \
typedef struct {                                                                        \
    __NAMESPACE__  ## _DATA_SIGNATURE_ ## __SIGNATURE__;                                \
    __BODY__                                                                            \
} __NAMESPACE__ ## _DATA_ ## __NAME__;
#include "ClientProtocolDefinition.h"

enum {
#define C2S_COMMAND(__NAME__, __COMMAND__) \
	C2S_ ## __NAME__ = __COMMAND__,
#include "ClientCommands.h"
    C2S_COMMAND_END,
};

enum {
#define S2C_COMMAND(__NAME__, __COMMAND__) \
	S2C_ ## __NAME__ = __COMMAND__,
#include "ClientCommands.h"
    S2C_COMMAND_END,
};

#pragma pack(pop)

EXTERN_C_END
