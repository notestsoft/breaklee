CREATE PROCEDURE SyncCraft (
    IN InCharacterID INT,
    IN InSlotCount INT,
    IN InEnergy INT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Craft
    SET
        SlotCount = InSlotCount,
        Energy = InEnergy,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
