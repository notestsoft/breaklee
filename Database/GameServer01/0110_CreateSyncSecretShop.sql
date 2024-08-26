CREATE PROCEDURE SyncSecretShop (
    IN InCharacterID INT,
    IN InRefreshCost TINYINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in the SecretShop table
    UPDATE SecretShop
    SET
        RefreshCost = InRefreshCost,
        ResetTimestamp = UNIX_TIMESTAMP(),
        RefreshCount = RefreshCount + 1,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
