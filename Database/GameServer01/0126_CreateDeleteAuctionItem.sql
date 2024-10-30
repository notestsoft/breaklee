CREATE PROCEDURE DeleteAuctionItem(
    IN InAccountID INT,
    IN InSlotIndex TINYINT,
    IN InSlotCount SMALLINT,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutItemID BIGINT UNSIGNED,
    OUT OutItemOptions BIGINT UNSIGNED,
    OUT OutItemDuration INT UNSIGNED,
    OUT OutItemCount SMALLINT
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
        ItemDuration,
        ItemCount
    INTO 
        OutItemID,
        OutItemOptions,
        OutItemDuration,
        OutItemCount
    FROM AuctionItems
    WHERE AccountID = InAccountID 
      AND SlotIndex = InSlotIndex
      AND SoldItemCount = 0
      AND ItemCount <= InSlotCount;

    IF ROW_COUNT() > 0 THEN
        DELETE FROM AuctionItems
        WHERE AccountID = InAccountID 
          AND SlotIndex = InSlotIndex;

        COMMIT;
        SET OutResult = 0;
    ELSE
        ROLLBACK;
        SET OutResult = 1;
    END IF;
END;
