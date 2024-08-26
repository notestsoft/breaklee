CREATE PROCEDURE SyncNewbieSupport (
    IN InCharacterID INT,
    IN InTimestamp BIGINT UNSIGNED,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in NewbieSupport table
    UPDATE NewbieSupport
    SET
        Timestamp = InTimestamp,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
