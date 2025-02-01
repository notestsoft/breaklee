CREATE PROCEDURE GetCashInventory (
    IN InAccountID INT
)
BEGIN
    SELECT 
        TransactionID,
        ItemID,
        ItemOptions,
        ItemDurationIndex
    FROM CashInventory
    WHERE AccountID = InAccountID
      AND DeletedAt IS NULL;
END;
