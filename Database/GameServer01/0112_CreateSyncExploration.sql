CREATE PROCEDURE SyncExploration (
    IN InCharacterID INT,
    IN InEndDate BIGINT,
    IN InPoints INT,
    IN InLevel INT,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in the Exploration table
    UPDATE Exploration
    SET
        EndDate = InEndDate,
        Points = InPoints,
        Level = InLevel,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
