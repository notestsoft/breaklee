CREATE PROCEDURE ProceedAuctionItem(
    IN InAccountID INT,
    IN InSlotIndex TINYINT,
    IN InCommissionRate INT,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutSoldItemCount INT,
    OUT OutReceivedAlz BIGINT
)
BEGIN
    DECLARE TempItemCount INT;
    DECLARE TempItemPrice BIGINT;
    DECLARE TempTotalPrice BIGINT;
    DECLARE TempCommission BIGINT;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
        SET OutSoldItemCount = 0;
    END;

    START TRANSACTION;

    SET OutResult = 0;
    SET OutSoldItemCount = 0;
    SET OutReceivedAlz = 0;

    SELECT 
        ItemCount, 
        SoldItemCount, 
        ItemPrice
    INTO 
        TempItemCount, 
        OutSoldItemCount, 
        TempItemPrice
    FROM AuctionItems
    WHERE AccountID = InAccountID 
      AND SlotIndex = InSlotIndex;

    IF ROW_COUNT() > 0 THEN
        SET TempTotalPrice = OutSoldItemCount * TempItemPrice;
        SET TempCommission = FLOOR((TempTotalPrice * InCommissionRate) / 10000.0);
        SET OutReceivedAlz = TempTotalPrice - TempCommission;

        SET TempItemCount = TempItemCount - OutSoldItemCount;

        IF TempItemCount > 0 THEN
            UPDATE AuctionItems
            SET ItemCount = TempItemCount, 
                SoldItemCount = 0
            WHERE AccountID = InAccountID 
              AND SlotIndex = InSlotIndex;
        ELSE
            DELETE FROM AuctionItems
            WHERE AccountID = InAccountID 
              AND SlotIndex = InSlotIndex;
        END IF;

        COMMIT;
        SET OutResult = 0;
    ELSE
        ROLLBACK;
        SET OutResult = 1;
        SET OutSoldItemCount = 0;
    END IF;
END;
