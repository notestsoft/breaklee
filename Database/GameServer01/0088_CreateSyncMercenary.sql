CREATE PROCEDURE SyncMercenary (
    IN InCharacterID INT,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Mercenary
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
