CREATE PROCEDURE SyncDamageBooster (
    IN InCharacterID INT,
    IN InItemData BLOB,
    IN InSlotCount TINYINT,
    IN InSlotData BLOB
)
BEGIN
    -- Update the existing record in the DamageBooster table
    UPDATE DamageBooster
    SET
        ItemData = InItemData,
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END