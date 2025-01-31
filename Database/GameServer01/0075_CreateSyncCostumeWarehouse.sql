CREATE PROCEDURE SyncCostumeWarehouse (
    IN InAccountID INT,
    IN InSlotCount INT,
    IN InMillagePoints INT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE CostumeWarehouse
    SET
        SlotCount = InSlotCount,
        MillagePoints = InMillagePoints,
        SlotData = InSlotData
    WHERE AccountID = InAccountID;
END;
