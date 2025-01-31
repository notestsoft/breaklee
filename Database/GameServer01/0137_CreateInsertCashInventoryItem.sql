CREATE PROCEDURE InsertCashInventoryItem(
    IN InAccountID INT,
    IN InItemID BIGINT UNSIGNED,
    IN InItemOptions BIGINT UNSIGNED,
    IN InItemDurationIndex TINYINT UNSIGNED,
    OUT OutResult TINYINT UNSIGNED
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    START TRANSACTION;

    INSERT INTO CashInventory (
        AccountID, 
        ItemID, 
        ItemOptions, 
        ItemDurationIndex
    )
    VALUES (
        InAccountID, 
        InItemID, 
        InItemOptions, 
        InItemDurationIndex
    );

    SET OutResult = 0;

    COMMIT;
END
