CREATE PROCEDURE SyncPremiumService (
    IN InCharacterID INT,
    IN InSlotCount TINYINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE PremiumService
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
