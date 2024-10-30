CREATE PROCEDURE UpdateAuctionItemDecrease(
    IN InAccountID INT,
    IN InCharacterID INT,
    IN InSlotIndex TINYINT,
    IN InItemPriceType TINYINT,
    IN InItemPrice BIGINT UNSIGNED,
    IN InItemCount SMALLINT UNSIGNED,
    IN InInventorySlotCount SMALLINT UNSIGNED,
    IN InExpirationTime BIGINT UNSIGNED,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutExpiresAt BIGINT UNSIGNED,
    OUT OutItemID BIGINT UNSIGNED,
    OUT OutItemOptions BIGINT UNSIGNED,
    OUT OutItemDuration INT UNSIGNED
)
BEGIN
    DECLARE TempItemCount SMALLINT UNSIGNED;
    
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    ProcLabelBody: BEGIN

        START TRANSACTION;

        SET OutResult = 0;
        SET OutExpiresAt = 0;
        
        SELECT 
            ItemID,
            ItemOptions,
            ItemDuration,
            ItemCount
        INTO 
            OutItemID,
            OutItemOptions,
            OutItemDuration,
            TempItemCount
        FROM AuctionItems
        WHERE AccountID = InAccountID 
        AND SlotIndex = InSlotIndex
        AND SoldItemCount = 0;

        IF ROW_COUNT() = 0 THEN
            ROLLBACK;
            SET OutResult = 1;
            LEAVE ProcLabelBody;
        END IF;

        IF TempItemCount <> InItemCount + InInventorySlotCount THEN
            ROLLBACK;
            SET OutResult = 1;
            LEAVE ProcLabelBody;
        END IF;

        SET OutExpiresAt = UNIX_TIMESTAMP() + InExpirationTime;

        UPDATE AuctionItems
        SET ItemCount = InItemCount, 
            ItemPrice = InItemPrice,
            ExpiresAt = OutExpiresAt
        WHERE AccountID = InAccountID 
        AND SlotIndex = InSlotIndex;

        IF ROW_COUNT() = 0 THEN
            ROLLBACK;
            SET OutResult = 1;
            LEAVE ProcLabelBody;
        END IF;

        COMMIT;
        SET OutResult = 0;

    END ProcLabelBody;
END;
