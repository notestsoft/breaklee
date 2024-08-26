CREATE PROCEDURE SyncHonorMedal (
    IN InCharacterID INT,
    IN InGrade INT,
    IN InScore INT,
    IN InSlotCount TINYINT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in HonorMedal table
    UPDATE HonorMedal
    SET
        Grade = InGrade,
        Score = InScore,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
