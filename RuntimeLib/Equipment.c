#include "Constants.h"
#include "Character.h"
#include "Equipment.h"
#include "Runtime.h"
#include "NotificationProtocol.h"
#include "NotificationManager.h"

Bool RTEquipmentSlotIndexMatchItemType(
    UInt32 BattleStyleIndex,
    UInt32 SlotIndex,
    UInt32 ItemType
) {
    switch (SlotIndex) {
    case RUNTIME_EQUIPMENT_SLOT_INDEX_HELMET:
        return (ItemType == RUNTIME_ITEM_TYPE_HELMED1 || ItemType == RUNTIME_ITEM_TYPE_HELMED2 || ItemType == RUNTIME_ITEM_TYPE_HEADGEAR3);

    case RUNTIME_EQUIPMENT_SLOT_INDEX_SUIT:         return ItemType == RUNTIME_ITEM_TYPE_SUIT;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_GLOVES:       return ItemType == RUNTIME_ITEM_TYPE_GLOVES;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BOOTS:        return ItemType == RUNTIME_ITEM_TYPE_BOOTS;

    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_RIGHT: switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return ItemType == RUNTIME_ITEM_TYPE_CHAKRAM;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    default:
        return false;
    }

    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LEFT: switch (BattleStyleIndex) {
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WA: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_TWO_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_BL: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_WI: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FA: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FS: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FB: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_ONE_HAND;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_GL: return ItemType == RUNTIME_ITEM_TYPE_CHAKRAM;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_FG: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    case RUNTIME_DATA_CHARACTER_BATTLE_STYLE_INDEX_DM: return ItemType == RUNTIME_ITEM_TYPE_WEAPON_FORCE_CONTROLLER;
    default:
        return false;
    }

    case RUNTIME_EQUIPMENT_SLOT_INDEX_EPAULET:
        return (ItemType == RUNTIME_ITEM_TYPE_EPAULET || ItemType == RUNTIME_ITEM_TYPE_EPAULET2);

    case RUNTIME_EQUIPMENT_SLOT_INDEX_AMULET:               return ItemType == RUNTIME_ITEM_TYPE_AMULET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_LEFT:        return ItemType == RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_TOP_RIGHT:       return ItemType == RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_VEHICLE:
        return (ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BOARD || ItemType == RUNTIME_ITEM_TYPE_VEHICLE_BIKE);

    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_APPLIED:          return ItemType == RUNTIME_ITEM_TYPE_SUMMONING_PET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_1:            return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_LEFT:         return ItemType == RUNTIME_ITEM_TYPE_EARRING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EARRING_RIGHT:        return ItemType == RUNTIME_ITEM_TYPE_EARRING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_LEFT:        return ItemType == RUNTIME_ITEM_TYPE_BRACELET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BRACELET_RIGHT:       return ItemType == RUNTIME_ITEM_TYPE_BRACELET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_LEFT:     return ItemType == RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_RING_BOTTOM_RIGHT:    return ItemType == RUNTIME_ITEM_TYPE_RING;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BELT:                 return ItemType == RUNTIME_ITEM_TYPE_BELT;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_EXTENDER:         return ItemType == RUNTIME_ITEM_TYPE_SUMMONING_PET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_CHARM:                return ItemType == RUNTIME_ITEM_TYPE_CHARM;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_LEFT:        return ItemType == RUNTIME_ITEM_TYPE_EFFECTOR;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_EFFECTOR_RIGHT:       return ItemType == RUNTIME_ITEM_TYPE_EFFECTOR;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_WEAPON_LINKED:        return ItemType == RUNTIME_ITEM_TYPE_LINK_WEAPON;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_RIGHT: return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_WEAPON_LEFT:  return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_HELMET:       return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED:      return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_DIALOG_BUBBLE:        return ItemType == RUNTIME_ITEM_TYPE_DIALOGUE_BUBBLE;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_TALISMAN:             return ItemType == RUNTIME_ITEM_TYPE_TALISMAN;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY:      return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_CARNELIAN:            return ItemType == RUNTIME_ITEM_TYPE_CARNELIAN;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_LEFT:          return ItemType == RUNTIME_ITEM_TYPE_ARCANA;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_ARCANA_RIGHT:         return ItemType == RUNTIME_ITEM_TYPE_ARCANA;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_UNKNOWN_2:            return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_VEHICLE:      return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_APPLIED_FORCE_WING:   return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_COSTUME_DISPLAY_FORCE_WING:   return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_PET_DISPLAY:          return ItemType == RUNTIME_ITEM_TYPE_SUMMONING_PET;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_BADGE:                return false;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_1:         return ItemType == RUNTIME_ITEM_TYPE_LINK_WEAPON;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_2:         return ItemType == RUNTIME_ITEM_TYPE_LINK_WEAPON;
    case RUNTIME_EQUIPMENT_SLOT_INDEX_JEWEL_CUBE_3:         return ItemType == RUNTIME_ITEM_TYPE_LINK_WEAPON;
    default: 
        return false;
    }
}

Bool RTEquipmentIsSlotEmpty(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentDataRef Equipment,
    Int32 SlotIndex
) {
    return RTEquipmentGetSlot(Runtime, Equipment, SlotIndex) == NULL;
}

RTItemSlotRef RTEquipmentGetSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentDataRef Equipment,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return NULL;

    for (Int32 Index = 0; Index < Equipment->Info.EquipmentSlotCount; Index += 1) {
        RTItemSlotRef Slot = &Equipment->EquipmentSlots[Index];
        if (Slot->SlotIndex == SlotIndex) return Slot;
    }

    return NULL;
}

Bool RTEquipmentSetSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTCharacterEquipmentDataRef Equipment,
    RTItemSlotRef Slot
) {
    Info("RTEquipmentSetSlot(%d)", Slot->SlotIndex);
    if (Slot->SlotIndex < 0 || Slot->SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;
    if (Equipment->Info.EquipmentSlotCount >= RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    RTItemSlotRef CurrentSlot = RTEquipmentGetSlot(Runtime, Equipment, Slot->SlotIndex);
    if (CurrentSlot) return false;

    CurrentSlot = &Equipment->EquipmentSlots[Equipment->Info.EquipmentSlotCount];
    memcpy(CurrentSlot, Slot, sizeof(struct _RTItemSlot));
    Equipment->Info.EquipmentSlotCount += 1;

    {
        NOTIFICATION_DATA_CHARACTER_ITEM_EQUIP* Notification = RTNotificationInit(CHARACTER_ITEM_EQUIP);
        Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
        Notification->Item = Slot->Item;
        Notification->ItemOptions = Slot->ItemOptions;
        Notification->EquipmentSlotIndex = Slot->SlotIndex;
        Notification->Unknown1 = 0;
        Notification->Unknown2 = 0;
        RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
    }

    return true;
}

Bool RTEquipmentClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTCharacterEquipmentDataRef Equipment,
    Int32 SlotIndex
) {
    Info("RTEquipmentClearSlot(%d)", SlotIndex);
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    for (Int32 Index = 0; Index < Equipment->Info.EquipmentSlotCount; Index += 1) {
        RTItemSlotRef Slot = &Equipment->EquipmentSlots[Index];
        if (Slot->SlotIndex == SlotIndex) {
            {
                NOTIFICATION_DATA_CHARACTER_ITEM_UNEQUIP* Notification = RTNotificationInit(CHARACTER_ITEM_UNEQUIP);
                Notification->CharacterIndex = (UInt32)Character->CharacterIndex;
                Notification->EquipmentSlotIndex = Slot->SlotIndex;
                RTNotificationDispatchToNearby(Notification, Character->Movement.WorldChunk);
            }

            Int32 TailLength = Equipment->Info.EquipmentSlotCount - Index - 1;
            if (TailLength > 0) {
                memmove(
                    &Equipment->EquipmentSlots[Index],
                    &Equipment->EquipmentSlots[Index + 1],
                    TailLength * sizeof(struct _RTItemSlot)
                );
            }

            Equipment->Info.EquipmentSlotCount -= 1;
            return true;
        }
    }

    return false;
}

Bool RTEquipmentRemoveSlot(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTCharacterEquipmentDataRef Equipment,
    Int32 SlotIndex,
    RTItemSlotRef Result
) {
    RTItemSlotRef Slot = RTEquipmentGetSlot(Runtime, Equipment, SlotIndex);
    if (!Slot) return false;

    memcpy(Result, Slot, sizeof(struct _RTItemSlot));
    
    return RTEquipmentClearSlot(Runtime, Character, Equipment, SlotIndex);
}

Int32 RTCharacterFindNextEquipmentSlotIndex(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 NextSlotIndex,
    UInt32 ItemType
) {
    Int32 BattleStyleIndex = Character->Data.StyleInfo.Style.BattleStyle | (Character->Data.StyleInfo.Style.ExtendedBattleStyle << 3);
    for (Int32 SlotIndex = NextSlotIndex; SlotIndex < RUNTIME_MAX_EQUIPMENT_SLOT_INDEX_COUNT; SlotIndex += 1) {
        if (RTEquipmentSlotIndexMatchItemType(BattleStyleIndex, SlotIndex, ItemType) && !RTCharacterEquipmentIsLocked(Runtime, Character, SlotIndex)) {
            return SlotIndex;
        }
    }

    return -1;
}

Bool RTCharacterEquipmentIsLocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    for (Int32 Index = 0; Index < Character->Data.EquipmentInfo.Info.LockSlotCount; Index += 1) {
        RTEquipmentLockSlotRef LockSlot = &Character->Data.EquipmentInfo.LockSlots[Index];
        if (LockSlot->SlotIndex != SlotIndex) continue;

        return true;
    }

    return false;
}

Bool RTCharacterEquipmentSetLocked(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    Int32 SlotIndex,
    Bool IsLocked
) {
    if (SlotIndex < 0 || SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    Int32 LockSlotIndex = -1;
    for (Int32 Index = 0; Index < Character->Data.EquipmentInfo.Info.LockSlotCount; Index += 1) {
        RTEquipmentLockSlotRef LockSlot = &Character->Data.EquipmentInfo.LockSlots[Index];
        if (LockSlot->SlotIndex != SlotIndex) continue;

        LockSlotIndex = Index;
        break;
    }

    if (IsLocked && LockSlotIndex < 0) {
        Character->Data.EquipmentInfo.LockSlots[Character->Data.EquipmentInfo.Info.LockSlotCount].SlotIndex = SlotIndex;
        Character->Data.EquipmentInfo.Info.LockSlotCount += 1;
        Character->SyncMask.EquipmentInfo = true;
    }

    if (!IsLocked && LockSlotIndex >= 0) {
        Int32 TailLength = sizeof(struct _RTEquipmentLockSlot) * (Character->Data.EquipmentInfo.Info.LockSlotCount - LockSlotIndex - 1);
        if (TailLength > 0) {
            memmove(
                &Character->Data.EquipmentInfo.LockSlots[LockSlotIndex],
                &Character->Data.EquipmentInfo.LockSlots[LockSlotIndex + 1],
                TailLength * sizeof(struct _RTEquipmentLockSlot)
            );
        }

        Character->Data.EquipmentInfo.Info.LockSlotCount -= 1;
        Character->SyncMask.EquipmentInfo = true;
    }

    return true;
}
