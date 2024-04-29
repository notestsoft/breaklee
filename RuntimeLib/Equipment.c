#include "Constants.h"
#include "Character.h"
#include "Equipment.h"
#include "Runtime.h"

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
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    return RTEquipmentGetSlot(Runtime, Equipment, SlotIndex) == NULL;
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
    Info("RTEquipmentSetSlot(%d)", Slot->SlotIndex);
    if (Slot->SlotIndex < 0 || Slot->SlotIndex > RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;
    if (Equipment->Count >= RUNTIME_CHARACTER_MAX_EQUIPMENT_COUNT) return false;

    RTItemSlotRef CurrentSlot = RTEquipmentGetSlot(Runtime, Equipment, Slot->SlotIndex);
    if (CurrentSlot) return false;

    CurrentSlot = &Equipment->Slots[Equipment->Count];
    memcpy(CurrentSlot, Slot, sizeof(struct _RTItemSlot));
    Equipment->Count += 1;
    return true;
}

Bool RTEquipmentClearSlot(
    RTRuntimeRef Runtime,
    RTCharacterEquipmentInfoRef Equipment,
    Int32 SlotIndex
) {
    Info("RTEquipmentClearSlot(%d)", SlotIndex);
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
        if (RTEquipmentSlotIndexMatchItemType(BattleStyleIndex, SlotIndex, ItemType)) {
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
            Warn("No item data found for item id: %d", ItemSlot->Item.ID);
            continue;
        }

        RTItemUseInternal(Runtime, Character, ItemSlot, ItemData, NULL);
    }
}

Void RTCharacterBroadcastEquipmentUpdate(
    RTRuntimeRef Runtime,
    RTCharacterRef Character,
    RTItemSlotRef Slot,
    Bool IsEquip
) {
    RTEventData EventData = { 0 };
    if (IsEquip) {
        EventData.CharacterEquipItem.CharacterIndex = (UInt32)Character->CharacterIndex;
        EventData.CharacterEquipItem.Item = Slot->Item;
        EventData.CharacterEquipItem.ItemOptions = Slot->ItemOptions;
        EventData.CharacterEquipItem.EquipmentSlotIndex = Slot->SlotIndex;

        RTRuntimeBroadcastEventData(
            Runtime,
            RUNTIME_EVENT_CHARACTER_EQUIP_ITEM,
            RTRuntimeGetWorldByCharacter(Runtime, Character),
            kEntityIDNull,
            Character->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y,
            EventData
        );
    }
    else {
        EventData.CharacterUnequipItem.CharacterIndex = (UInt32)Character->CharacterIndex;
        EventData.CharacterUnequipItem.EquipmentSlotIndex = Slot->SlotIndex;

        RTRuntimeBroadcastEventData(
            Runtime,
            RUNTIME_EVENT_CHARACTER_UNEQUIP_ITEM,
            RTRuntimeGetWorldByCharacter(Runtime, Character),
            kEntityIDNull,
            Character->ID,
            Character->Movement.PositionCurrent.X,
            Character->Movement.PositionCurrent.Y,
            EventData
        );
    }
}
