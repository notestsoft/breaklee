CREATE PROCEDURE SyncGoldMeritMastery (
    IN InAccountID INT,
    IN InMasterySlotCount INT,
    IN InExp INT,
    IN InPoints INT,
    IN InMasterySlotData BLOB
)
BEGIN 
    -- Attempt to update the record
    UPDATE GoldMeritMastery
    SET
        MasterySlotCount = InMasterySlotCount,
        Exp = InExp,
        Points = InPoints,
        MasterySlotData = InMasterySlotData
    WHERE AccountID = InAccountID;
END;
