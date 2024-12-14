CREATE PROCEDURE SyncAppearance (
    IN InCharacterID INT,
    IN InEquipmentSlotCount TINYINT,
    IN InInventorySlotCount SMALLINT,
    IN InEquipmentSlotData BLOB,
    IN InInventorySlotData BLOB
)
BEGIN
    UPDATE Appearance
    SET
        EquipmentSlotCount = InEquipmentSlotCount,
        InventorySlotCount = InInventorySlotCount,
        EquipmentSlotData = InEquipmentSlotData,
        InventorySlotData = InInventorySlotData
    WHERE CharacterID = InCharacterID;
END;
