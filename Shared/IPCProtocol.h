#pragma once

#include <NetLib/NetLib.h>
#include <RuntimeLib/RuntimeLib.h>

#include "Constants.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define IPC_PROTOCOL_ENUM(...) \
enum {                         \
    __VA_ARGS__                \
};
#include "IPCProtocolDefinition.h"

#define IPC_PROTOCOL_STRUCT(__NAME__, __BODY__) \
typedef struct {                                \
    __BODY__                                    \
} __NAME__;
#include "IPCProtocolDefinition.h"

#define IPC_PROTOCOL(__NAMESPACE__, __NAME__, __BODY__) \
typedef struct {                                        \
    struct _IPCPacket Header;                           \
    __BODY__                                            \
} IPC_ ## __NAMESPACE__ ## _DATA_ ## __NAME__;
#include "IPCProtocolDefinition.h"

enum {
#define IPC_PROTOCOL(__NAMESPACE__, __NAME__, __BODY__) \
	IPC_ ## __NAMESPACE__ ## _ ## __NAME__,
#include "IPCProtocolDefinition.h"

    IPC_COMMAND_COUNT,
};

#pragma pack(pop)

EXTERN_C_END
