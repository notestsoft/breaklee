#include "World.h"
#include "WorldAPI.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Void RTScriptBindWorldAPI(
    RTScriptRef Script
) {
    RTScriptCreateClass(
        Script,
        "World",
        NULL
    );
}
