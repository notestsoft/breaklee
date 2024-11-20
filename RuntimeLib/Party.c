#include "Party.h"

RTPartySlotRef RTPartyGetMember(
    RTPartyRef Party,
    Index CharacterIndex
) {
    for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
        RTPartySlotRef PartySlot = &Party->Members[Index];
        if (PartySlot->Info.CharacterIndex != CharacterIndex) continue;

        return PartySlot;
    }

    return NULL;
}

Bool RTPartyChangeLeader(
    RTPartyRef Party,
    Index CharacterIndex
) {
    RTPartySlotRef PartySlot = RTPartyGetMember(Party, CharacterIndex);
    if (!PartySlot) return false;

    Party->LeaderCharacterIndex = CharacterIndex;
    return true;
}
