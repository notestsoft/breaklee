#pragma once

#include "Base.h"
#include "Constants.h"

#include "Item.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

// TODO: Equipment slots should support being equipped / unequipped without having to be in the inventory.

enum {
    RUNTIME_EQUIPMENT_SLOT_INDEX_HELMET = 0,
    RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES,
    RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS,
    RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_EPAULET,
    RUNTIME_EQUIPMENT_SLOT_INDEX_AMULET,
    RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_VEHICLE,
    RUNTIME_EQUIPMENT_SLOT_INDEX_PET_APPLIED,
    RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_1,
    RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_BELT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_PET_EXTENDER,
    RUNTIME_EQUIPMENT_SLOT_INDEX_CHARM,
    RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LINKED,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_HELMET,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED,
    RUNTIME_EQUIPMENT_SLOT_INDEX_DIALOG_BUBBLE,
    RUNTIME_EQUIPMENT_SLOT_INDEX_TALISMAN,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY,
    RUNTIME_EQUIPMENT_SLOT_INDEX_CARNELIAN,
    RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_LEFT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_RIGHT,
    RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_2,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_VEHICLE,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED_FORCE_WING,
    RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_FORCE_WING,
    RUNTIME_EQUIPMENT_SLOT_INDEX_PET_DISPLAY,
    RUNTIME_EQUIPMENT_SLOT_INDEX_BADGE,
    RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_1,
    RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_2,
    RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_3,

    RUNTIME_MAX_EQUIPMENT_SLOT_INDEX_COUNT,
};

struct _RTCharacterEquipmentInfo {
    UInt32 Count;
    struct _RTItemSlot Slots[RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT];
};

#pragma pack(pop)

UInt32 RTEquipmentSlotIndexGetItemType(
    UInt32 BattleStyleIndex,
    UInt32 SlotIndex
);

RTItemSlotRef RTEquipmentGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
);

Bool RTEquipmentSetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    RTItemSlotRef Slot
);

Bool RTEquipmentClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
);

Bool RTEquipmentRemoveSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex,
    RTItemSlotRef Result
);

Int32 RTCharacterFindNextEquipmentSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NextSlotIndex,
    UInt32 ItemType
);

Void RTCharacterApplyEquipmentAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
);

EXTERN_C_END