CREATE PROCEDURE SyncSkillTranscendence (
    IN InCharacterID INT,
    IN InPoints INT,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in SkillTranscendence table
    UPDATE SkillTranscendence
    SET
        Points = InPoints,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
