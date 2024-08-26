CREATE PROCEDURE SyncBlessingBead (
    IN InCharacterID INT,
    IN InSlotCount TINYINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE BlessingBead
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
