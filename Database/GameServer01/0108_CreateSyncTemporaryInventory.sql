CREATE PROCEDURE SyncTemporaryInventory (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in TemporaryInventory table
    UPDATE TemporaryInventory
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
