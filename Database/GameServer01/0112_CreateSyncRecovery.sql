CREATE PROCEDURE SyncRecovery (
    IN InCharacterID INT,
    IN InCount INT,
    IN InPriceData BLOB,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in Recovery table
    UPDATE Recovery
    SET
        Count = InCount,
        PriceData = InPriceData,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
