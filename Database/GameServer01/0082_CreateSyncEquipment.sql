CREATE PROCEDURE SyncEquipment (
    IN InCharacterID INT,
    IN InEquipmentSlotCount TINYINT UNSIGNED,
    IN InInventorySlotCount TINYINT UNSIGNED,
    IN InLinkSlotCount TINYINT UNSIGNED,
    IN InLockSlotCount TINYINT UNSIGNED,
    IN InEquipmentSlotData BLOB,
    IN InInventorySlotData BLOB,
    IN InLinkSlotData BLOB,
    IN InLockSlotData BLOB
)
BEGIN
    UPDATE Equipment
    SET
        EquipmentSlotCount = InEquipmentSlotCount,
        InventorySlotCount = InInventorySlotCount,
        LinkSlotCount = InLinkSlotCount,
        LockSlotCount = InLockSlotCount,
        EquipmentSlotData = InEquipmentSlotData,
        InventorySlotData = InInventorySlotData,
        LinkSlotData = InLinkSlotData,
        LockSlotData = InLockSlotData
    WHERE CharacterID = InCharacterID;
END;
