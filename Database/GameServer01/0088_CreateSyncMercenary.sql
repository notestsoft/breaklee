CREATE PROCEDURE SyncMercenary (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Mercenary
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
