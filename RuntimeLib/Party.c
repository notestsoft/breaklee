#include "Party.h"

RTPartyMemberInfoRef RTPartyGetMember(
    RTPartyRef Party,
    Index CharacterIndex
) {
    for (Index Index = 0; Index < Party->MemberCount; Index += 1) {
        RTPartyMemberInfoRef Member = &Party->Members[Index];
        if (Member->CharacterIndex != CharacterIndex) continue;

        return Member;
    }

    return NULL;
}

Bool RTPartyChangeLeader(
    RTPartyRef Party,
    Index CharacterIndex
) {
    RTPartyMemberInfoRef Member = RTPartyGetMember(Party, CharacterIndex);
    if (!Member) return false;

    Party->LeaderCharacterIndex = CharacterIndex;
    return true;
}
