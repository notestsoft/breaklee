CREATE PROCEDURE SyncQuickSlot (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE QuickSlot
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
