CREATE PROCEDURE SyncCostumeWarehouse (
    IN InAccountID INT,
    IN InSlotCount INT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE CostumeWarehouse
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE AccountID = InAccountID;
END;
