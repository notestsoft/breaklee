#include "Drop.h"
#include "OptionPool.h"
#include "Runtime.h"

struct _RTOptionPoolValue {
    Int32 Value;
    Int32 Rate;
};
typedef struct _RTOptionPoolValue* RTOptionPoolValueRef;

struct _RTOptionPool {
    ArrayRef ItemLevels;
    ArrayRef EpicLevels;
    ArrayRef ForceSlots;
    ArrayRef ForceOptionSlots;
    DictionaryRef EpicOptions;
    DictionaryRef ForceOptions;
};
typedef struct _RTOptionPool* RTOptionPoolRef;

struct _RTOptionPoolManager {
    AllocatorRef Allocator;
    DictionaryRef OptionPool;
};

RTOptionPoolManagerRef RTOptionPoolManagerCreate(
    AllocatorRef Allocator
) {
    RTOptionPoolManagerRef OptionPoolManager = (RTOptionPoolManagerRef)AllocatorAllocate(Allocator, sizeof(struct _RTOptionPoolManager));
    if (!OptionPoolManager) Fatal("Memory allocation failed!");
    OptionPoolManager->Allocator = Allocator;
    OptionPoolManager->OptionPool = IndexDictionaryCreate(Allocator, 64);
    return OptionPoolManager;
}

Void RTOptionPoolManagerDestroy(
    RTOptionPoolManagerRef OptionPoolManager
) {
    DictionaryKeyIterator Iterator = DictionaryGetKeyIterator(OptionPoolManager->OptionPool);
    while (Iterator.Key) {
        RTOptionPoolRef OptionPool = (RTOptionPoolRef)DictionaryLookup(OptionPoolManager->OptionPool, Iterator.Key);
        ArrayDestroy(OptionPool->ItemLevels);
        ArrayDestroy(OptionPool->EpicLevels);
        ArrayDestroy(OptionPool->ForceSlots);
        ArrayDestroy(OptionPool->ForceOptionSlots);

        DictionaryKeyIterator GroupIterator = DictionaryGetKeyIterator(OptionPool->EpicOptions);
        while (GroupIterator.Key) {
            ArrayRef Group = DictionaryLookup(OptionPool->EpicOptions, GroupIterator.Key);
            ArrayDealloc(Group);
            GroupIterator = DictionaryKeyIteratorNext(GroupIterator);
        }

        GroupIterator = DictionaryGetKeyIterator(OptionPool->ForceOptions);
        while (GroupIterator.Key) {
            ArrayRef Group = DictionaryLookup(OptionPool->ForceOptions, GroupIterator.Key);
            ArrayDealloc(Group);
            GroupIterator = DictionaryKeyIteratorNext(GroupIterator);
        }

        Iterator = DictionaryKeyIteratorNext(Iterator);
    }

    DictionaryDestroy(OptionPoolManager->OptionPool);
    AllocatorDeallocate(OptionPoolManager->Allocator, OptionPoolManager);
}

RTOptionPoolRef RTOptionPoolManagerGetOptionPool(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex
) {
    RTOptionPoolRef OptionPool = (RTOptionPoolRef)DictionaryLookup(OptionPoolManager->OptionPool, &PoolIndex);
    if (OptionPool) return OptionPool;

    struct _RTOptionPool OptionPoolMemory = { 0 };
    OptionPoolMemory.ItemLevels = ArrayCreateEmpty(OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
    OptionPoolMemory.EpicLevels = ArrayCreateEmpty(OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
    OptionPoolMemory.ForceSlots = ArrayCreateEmpty(OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
    OptionPoolMemory.ForceOptionSlots = ArrayCreateEmpty(OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
    OptionPoolMemory.EpicOptions = IndexDictionaryCreate(OptionPoolManager->Allocator, 8);
    OptionPoolMemory.ForceOptions = IndexDictionaryCreate(OptionPoolManager->Allocator, 8);
    DictionaryInsert(OptionPoolManager->OptionPool, &PoolIndex, &OptionPoolMemory, sizeof(struct _RTOptionPool));
       
    return (RTOptionPoolRef)DictionaryLookup(OptionPoolManager->OptionPool, &PoolIndex);
}

Void RTOptionPoolManagerAddItemLevel(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 Level,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(OptionPool->ItemLevels);
    Value->Value = Level;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

Void RTOptionPoolManagerAddEpicLevel(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 Level,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(OptionPool->EpicLevels);
    Value->Value = Level;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

Void RTOptionPoolManagerAddEpicOption(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 ItemType,
    Int32 Level,
    Int32 ForceIndex,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    Int OptionIndex = (Int)Level << 16 | (Int)ItemType;
    ArrayRef EpicOptions = (ArrayRef)DictionaryLookup(OptionPool->EpicOptions, &OptionIndex);
    if (!EpicOptions) {
        struct _Array EpicOptionsMemory = { 0 };
        ArrayInitializeEmpty(&EpicOptionsMemory, OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
        DictionaryInsert(OptionPool->EpicOptions, &OptionIndex, &EpicOptionsMemory, sizeof(struct _Array));
        EpicOptions = DictionaryLookup(OptionPool->EpicOptions, &OptionIndex);
        assert(EpicOptions);
    }

    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(EpicOptions);
    Value->Value = ForceIndex;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

Void RTOptionPoolManagerAddForceSlot(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 Count,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(OptionPool->ForceSlots);
    Value->Value = Count;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

Void RTOptionPoolManagerAddForceOptionSlot(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 Count,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(OptionPool->ForceSlots);
    Value->Value = Count;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

Void RTOptionPoolManagerAddForceOption(
    RTOptionPoolManagerRef OptionPoolManager,
    Int PoolIndex,
    Int32 ItemType,
    Int32 ForceIndex,
    Float64 Rate
) {
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, PoolIndex);
    Int OptionIndex = ItemType;
    ArrayRef ForceOptions = DictionaryLookup(OptionPool->ForceOptions, &OptionIndex);
    if (!ForceOptions) {
        struct _Array ForceOptionsMemory = { 0 };
        ArrayInitializeEmpty(&ForceOptionsMemory, OptionPoolManager->Allocator, sizeof(struct _RTOptionPoolValue), 8);
        DictionaryInsert(OptionPool->ForceOptions, &OptionIndex, &ForceOptionsMemory, sizeof(struct _Array));
        ForceOptions = DictionaryLookup(OptionPool->ForceOptions, &OptionIndex);
        assert(ForceOptions);
    }

    RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayAppendUninitializedElement(ForceOptions);
    Value->Value = ForceIndex;
    Value->Rate = (Int32)(Rate / 100.0 * INT32_MAX);
}

RTOptionPoolValueRef RTCalculateOptionPoolValue(
    ArrayRef ValuePool,
    Int32* Seed
) {
    Int32 RateValue = RandomRange(Seed, 0, INT32_MAX);
    Int32 RateOffset = 0;

    for (Int Index = 0; Index < ArrayGetElementCount(ValuePool); Index += 1) {
        RTOptionPoolValueRef Value = (RTOptionPoolValueRef)ArrayGetElementAtIndex(ValuePool, Index);
        if (RateValue <= RateOffset + Value->Rate) {
            return Value;
        }

        RateOffset += Value->Rate;
    }

    return NULL;
}

Void RTOptionPoolManagerCalculateOptions(
    RTRuntimeRef Runtime,
    RTOptionPoolManagerRef OptionPoolManager,
    Int OptionPoolIndex,
    RTDropResultRef DropResult
) {
    if (OptionPoolIndex < 1) return;

    RTItemDataRef ItemData = RTRuntimeGetItemDataByIndex(Runtime, DropResult->ItemID.ID);
    RTOptionPoolRef OptionPool = RTOptionPoolManagerGetOptionPool(OptionPoolManager, OptionPoolIndex);
    if (!OptionPool) return;
    
    Int32 Seed = (Int32)GetTimestampMs();
    RTOptionPoolValueRef ItemLevelValue = RTCalculateOptionPoolValue(OptionPool->ItemLevels, &Seed);
    if (ItemLevelValue) {
        DropResult->ItemID.UpgradeLevel = ItemLevelValue->Value;
    }

    RTItemOptions ItemOptions = { .Serial = DropResult->ItemOptions };

    RTOptionPoolValueRef EpicLevelValue = RTCalculateOptionPoolValue(OptionPool->EpicLevels, &Seed);
    if (EpicLevelValue && EpicLevelValue->Value > 0) {
        Int OptionIndex = EpicLevelValue->Value << 16 | ItemData->ItemType;
        ArrayRef EpicOptions = (ArrayRef)DictionaryLookup(OptionPool->EpicOptions, &OptionIndex);
        RTOptionPoolValueRef EpicOptionValue = RTCalculateOptionPoolValue(EpicOptions, &Seed);
        if (EpicOptionValue && EpicOptionValue->Value > 0) {
            RTItemOptionSlot Slot = {
                .ForceIndex = EpicOptionValue->Value,
                .ForceLevel = EpicLevelValue->Value,
                .IsEpic = true
            };
            RTItemOptionAppendSlot(&ItemOptions, Slot, DropResult->ItemID.IsAccountBinding);
        }
    }

    RTOptionPoolValueRef ForceSlotValue = RTCalculateOptionPoolValue(OptionPool->ForceSlots, &Seed);
    if (ForceSlotValue) {
        ItemOptions.Equipment.SlotCount = ForceSlotValue->Value;
    }

    RTOptionPoolValueRef ForceOptionSlotValue = RTCalculateOptionPoolValue(OptionPool->ForceOptionSlots, &Seed);
    if (ForceOptionSlotValue) {
        for (Int SlotIndex = 0; SlotIndex < ForceOptionSlotValue->Value; SlotIndex += 1) {
            Int OptionIndex = ItemData->ItemType;
            ArrayRef ForceOptions = (ArrayRef)DictionaryLookup(OptionPool->ForceOptions, &OptionIndex);
            RTOptionPoolValueRef ForceOptionValue = RTCalculateOptionPoolValue(ForceOptions, &Seed);
            if (ForceOptionValue && ForceOptionValue->Value > 0) {
                RTItemOptionSlot Slot = {
                    .ForceIndex = ForceOptionValue->Value,
                    .ForceLevel = 1,
                    .IsEpic = false
                };
                RTItemOptionAppendSlot(&ItemOptions, Slot, DropResult->ItemID.IsAccountBinding);
            }
        }
    }

    DropResult->ItemOptions = ItemOptions.Serial;
}
