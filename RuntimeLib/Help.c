#include "Character.h"
#include "Help.h"

Void RTCharacterSetHelpWindow(
    RTCharacterRef Character,
    UInt32 HelpWindow
) {
    Character->Data.HelpInfo.HelpWindow = HelpWindow;
    Character->SyncMask.HelpInfo = true;
}
