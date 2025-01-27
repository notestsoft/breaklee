CREATE PROCEDURE DeleteCashInventoryItem(
    IN InAccountID INT,
    IN InTransactionID INT,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutItemID BIGINT UNSIGNED,
    OUT OutItemOptions BIGINT UNSIGNED,
    OUT OutItemDurationIndex TINYINT UNSIGNED
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    START TRANSACTION;

    SET OutResult = 0;

    SELECT 
        ItemID,
        ItemOptions,
        ItemDurationIndex
    INTO 
        OutItemID,
        OutItemOptions,
        OutItemDurationIndex
    FROM CashInventory
    WHERE AccountID = InAccountID 
      AND TransactionID = InTransactionID
      AND DeletedAt IS NULL;

    IF ROW_COUNT() > 0 THEN
        UPDATE CashInventory
        SET DeletedAt = UNIX_TIMESTAMP()
        WHERE AccountID = InAccountID 
          AND TransactionID = InTransactionID
          AND DeletedAt IS NULL;

        COMMIT;
        SET OutResult = 0;
    ELSE
        ROLLBACK;
        SET OutResult = 1;
    END IF;
END;
