CREATE PROCEDURE SyncOverlordMastery (
    IN InCharacterID INT,
    IN InLevel SMALLINT,
    IN InExp BIGINT,
    IN InPoints SMALLINT,
    IN InSlotCount TINYINT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in OverlordMastery table
    UPDATE OverlordMastery
    SET
        Level = InLevel,
        Exp = InExp,
        Points = InPoints,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
