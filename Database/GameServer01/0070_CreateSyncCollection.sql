CREATE PROCEDURE SyncCollection (
    IN InAccountID INT,
    IN InSlotCount SMALLINT,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Collection
    SET
        SlotCount = InSlotCount,
        SlotData = InSlotData
    WHERE AccountID = InAccountID;
END;
