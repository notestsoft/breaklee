#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _RTCharacterHelpInfo {
    UInt32 HelpWindow;
};

#pragma pack(pop)

Void RTCharacterSetHelpWindow(
    RTCharacterRef Character,
    UInt32 HelpWindow
);

EXTERN_C_END