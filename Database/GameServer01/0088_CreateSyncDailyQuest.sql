CREATE PROCEDURE SyncDailyQuest (
    IN InCharacterID INT,
    IN InSlotCount INT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE DailyQuest
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
