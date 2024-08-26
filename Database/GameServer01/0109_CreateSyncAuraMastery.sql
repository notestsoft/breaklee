CREATE PROCEDURE SyncAuraMastery (
    IN InCharacterID INT,
    IN InPoints INT,
    IN InAccumulatedTimeInMinutes INT,
    IN InSlotCount TINYINT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in the AuraMastery table
    UPDATE AuraMastery
    SET
        Points = InPoints,
        AccumulatedTimeInMinutes = InAccumulatedTimeInMinutes,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
