CREATE PROCEDURE SyncSkillSlot (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE SkillSlot
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
