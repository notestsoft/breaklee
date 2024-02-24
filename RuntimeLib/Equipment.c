#include "Constants.h"
#include "Character.h"
#include "Equipment.h"
#include "Runtime.h"

UInt32 RTEquipmentSlotIndexGetItemType(
    UInt32 BattleStyleIndex,
    UInt32 SlotIndex
) {
    switch (SlotIndex) {
    case RUNTIME_EQUIPMENT_SLOT_INDEX_HELMET:       return RUNTIME_ITEM_TYPE_HELMED1;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT:         return RUNTIME_ITEM_TYPE_SUIT;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES:       return RUNTIME_ITEM_TYPE_GLOVES;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS:        return RUNTIME_ITEM_TYPE_BOOTS;

    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT: switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return RUNTIME_ITEM_TYPE_CHAKRAM;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    default:
        FatalError("Implementation missing!");
    }

    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT: switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return RUNTIME_ITEM_TYPE_CHAKRAM;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    default:
        FatalError("Implementation missing!");
    }

    case RUNTIME_EQUIPMENT_SLOT_INDEX_EPAULET:              return RUNTIME_ITEM_TYPE_EPAULET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_AMULET:               return RUNTIME_ITEM_TYPE_AMULET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_LEFT:        return RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_RIGHT:       return RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_VEHICLE:              FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_APPLIED:          FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_1:            FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_LEFT:         return RUNTIME_ITEM_TYPE_EARRING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_RIGHT:        return RUNTIME_ITEM_TYPE_EARRING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_LEFT:        return RUNTIME_ITEM_TYPE_BRACELET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_RIGHT:       return RUNTIME_ITEM_TYPE_BRACELET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_LEFT:     return RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_RIGHT:    return RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BELT:                 return RUNTIME_ITEM_TYPE_BELT;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_EXTENDER:         FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_CHARM:                return RUNTIME_ITEM_TYPE_CHARM;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_LEFT:        return RUNTIME_ITEM_TYPE_EFFECTOR;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_RIGHT:       return RUNTIME_ITEM_TYPE_EFFECTOR;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LINKED:        FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_RIGHT: FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_LEFT:  FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_HELMET:       FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED:      FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_DIALOG_BUBBLE:        return RUNTIME_ITEM_TYPE_DIALOGUE_BUBBLE;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_TALISMAN:             return RUNTIME_ITEM_TYPE_TALISMAN;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY:      FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_CARNELIAN:            return RUNTIME_ITEM_TYPE_CARNELIAN;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_LEFT:          FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_RIGHT:         FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_2:            FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_VEHICLE:  FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED_FORCE_WING:   FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_FORCE_WING:   FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_DISPLAY:          FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BADGE:                FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_1:         FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_2:         FatalError("Implementation missing!");
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_3:         FatalError("Implementation missing!");
    default: 
        FatalError("Implementation missing!");
    }
}

RTItemSlotRef RTEquipmentGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return NULL;

    for (Int32 Index = 0; Index < Equipment->Count; Index++) {
        RTItemSlotRef Slot = &Equipment->Slots[Index];
        if (Slot->SlotIndex == SlotIndex) return Slot;
    }

    return NULL;
}

Bool RTEquipmentSetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    RTItemSlotRef Slot
) {
    if (Slot->SlotIndex < 0 || Slot->SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;
    if (Equipment->Count >= RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    RTItemSlotRef CurrentSlot = RTEquipmentGetSlot(Runtime, Equipment, Slot->SlotIndex);
    if (CurrentSlot) return false;

    CurrentSlot = &Equipment->Slots[Equipment->Count];
    memcpy(CurrentSlot, Slot, sizeof(struct _RTItemSlot));
    Equipment->Count += 1;
}

Bool RTEquipmentClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    for (Int32 Index = 0; Index < Equipment->Count; Index++) {
        RTItemSlotRef Slot = &Equipment->Slots[Index];
        if (Slot->SlotIndex == SlotIndex) {
            Int32 TailLength = Equipment->Count - Index - 1;
            if (TailLength > 0) {
                memmove(
                    &Equipment->Slots[Index],
                    &Equipment->Slots[Index + 1],
                    sizeof(struct _RTItemSlot) * TailLength
                );
            }

            Equipment->Count -= 1;
            return true;
        }
    }

    return false;
}

Bool RTEquipmentRemoveSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex,
    RTItemSlotRef Result
) {
    RTItemSlotRef Slot = RTEquipmentGetSlot(Runtime, Equipment, SlotIndex);
    if (!Slot) return false;

    memcpy(Result, Slot, sizeof(struct _RTItemSlot));
    
    return RTEquipmentClearSlot(Runtime, Equipment, SlotIndex);
}

Int32 RTCharacterFindNextEquipmentSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NextSlotIndex,
    UInt32 ItemType
) {
    Int32 BattleStyleIndex = Character->Info.Style.BattleStyle | (Character->Info.Style.ExtendedBattleStyle << 3);
    for (Int32 SlotIndex = NextSlotIndex; SlotIndex < RUNTIME_MAX_EQUIPMENT_SLOT_INDEX_COUNT; SlotIndex += 1) {
        if (RTEquipmentSlotIndexGetItemType(BattleStyleIndex, SlotIndex) == ItemType) {
            return SlotIndex;
        }
    }

    return -1;
}

Void RTCharacterApplyEquipmentAttributes(
    RTRuntimeRef Runtime,
    RTCharacterRef Character
) {
    for (Int32 Index = 0; Index < Character->EquipmentInfo.Count; Index += 1) {
        RTItemSlotRef ItemSlot = &Character->EquipmentInfo.Slots[Index];
        RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, ItemSlot->Item.ID);
        if (!ItemData) {
            LogMessageFormat(LOG_LEVEL_WARNING, "No item data found for item id: %d", ItemSlot->Item.ID);
            continue;
        }

        RTItemUseInternal(Runtime, Character, ItemSlot, ItemData, NULL);
    }
}
