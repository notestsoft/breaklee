CREATE PROCEDURE SyncVehicleInventory (
    IN InCharacterID INT,
    IN InSlotCount SMALLINT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE VehicleInventory
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
