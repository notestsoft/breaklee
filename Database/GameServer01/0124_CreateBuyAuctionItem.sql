CREATE PROCEDURE BuyAuctionItem(
    IN InAccountID INT,
    IN InItemID BIGINT UNSIGNED,
    IN InItemOptions BIGINT UNSIGNED,
    IN InItemDuration INT UNSIGNED,
    IN InItemPrice BIGINT UNSIGNED,
    IN InItemCount SMALLINT,
    OUT OutResult TINYINT UNSIGNED
)
BEGIN
    DECLARE TempSlotIndex SMALLINT UNSIGNED;
    DECLARE TempItemCount SMALLINT;
    DECLARE TempSoldItemCount SMALLINT;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    START TRANSACTION;

    SET OutResult = 0;

    SELECT 
        SlotIndex,
        ItemCount,
        SoldItemCount
    INTO
        TempSlotIndex,
        TempItemCount,
        TempSoldItemCount
    FROM AuctionItems
    WHERE AccountID = InAccountID
      AND ItemID = InItemID
      AND ItemOptions = InItemOptions
      AND ItemDuration = InItemDuration
      AND ItemPrice = InItemPrice
      AND ItemCount > SoldItemCount
      AND ExpiresAt < UNIX_TIMESTAMP();

    IF ROW_COUNT() > 0 THEN
        SET TempSoldItemCount = TempSoldItemCount + InItemCount;
        IF TempSoldItemCount <= TempItemCount THEN
            UPDATE AuctionItems
            SET SoldItemCount = TempSoldItemCount
            WHERE AccountID = InAccountID
              AND SlotIndex = TempSlotIndex;
            
            COMMIT;
            SET OutResult = 0;
        ELSE
            ROLLBACK;
            SET OutResult = 1;
        END IF;
    ELSE
        ROLLBACK;
        SET OutResult = 1;
    END IF;
END;