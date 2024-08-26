CREATE PROCEDURE SyncRequestCraft (
    IN InCharacterID INT,
    IN InSlotCount TINYINT UNSIGNED,
    IN InExp SMALLINT UNSIGNED,
    IN InRegisteredFlagData BLOB,
    IN InFavoriteFlagData BLOB,
    IN InSortingOrder SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE RequestCraft
    SET
        SlotCount = InSlotCount,
        Exp = InExp,
        RegisteredFlagData = InRegisteredFlagData,
        FavoriteFlagData = InFavoriteFlagData,
        SortingOrder = InSortingOrder,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
