CREATE PROCEDURE SyncWarpService (
    IN InCharacterID INT,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record
    UPDATE WarpService
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
