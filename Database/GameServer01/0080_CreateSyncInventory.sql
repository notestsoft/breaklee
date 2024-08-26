CREATE PROCEDURE SyncInventory (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Inventory
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
