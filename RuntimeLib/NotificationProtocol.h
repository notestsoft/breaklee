#pragma once

#include "NotificationManager.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

#define NOTIFICATION_PROTOCOL_ENUM(...) \
enum {                                  \
    __VA_ARGS__                         \
};
#include "NotificationProtocolDefinition.h"

#define NOTIFICATION_PROTOCOL_STRUCT(__NAME__, __BODY__) \
typedef struct {                                         \
    __BODY__                                             \
} NOTIFICATION_DATA_ ## __NAME__;
#include "NotificationProtocolDefinition.h"

#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__) \
typedef struct {                                               \
    struct _RTNotification Base;                               \
    __BODY__                                                   \
} NOTIFICATION_DATA_ ## __NAME__;
#include "NotificationProtocolDefinition.h"

enum {
#define NOTIFICATION_PROTOCOL(__NAME__, __COMMAND__, __BODY__) \
	NOTIFICATION_ ## __NAME__ = __COMMAND__,
#include "NotificationProtocolDefinition.h"
};

#pragma pack(pop)

EXTERN_C_END
