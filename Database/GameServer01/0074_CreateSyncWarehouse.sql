CREATE PROCEDURE SyncWarehouse (
    IN InAccountID INT,
    IN InCount SMALLINT UNSIGNED,
    IN InCurrency BIGINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Warehouse
    SET
        Count = InCount,
        Currency = InCurrency,
        SlotData = InSlotData
    WHERE AccountID = InAccountID;
END;
