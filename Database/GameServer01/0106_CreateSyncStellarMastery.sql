CREATE PROCEDURE SyncStellarMastery (
    IN InCharacterID INT,
    IN InSlotCount TINYINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in StellarMastery table
    UPDATE StellarMastery
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
