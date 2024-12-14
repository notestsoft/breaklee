CREATE PROCEDURE UpdateAuctionItemIncrease(
    IN InAccountID INT,
    IN InCharacterID INT,
    IN InSlotIndex TINYINT,
    IN InItemID BIGINT UNSIGNED,
    IN InItemOptions BIGINT UNSIGNED,
    IN InItemDuration INT UNSIGNED,
    IN InItemPriceType TINYINT,
    IN InItemPrice BIGINT UNSIGNED,
    IN InItemCount SMALLINT UNSIGNED,
    IN InExpirationTime BIGINT UNSIGNED,
    IN InInventorySlotCount SMALLINT UNSIGNED,
    IN InInventorySlotData BLOB,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutExpiresAt BIGINT UNSIGNED
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    ProcLabelBody: BEGIN
        START TRANSACTION;

        SET OutResult = 0;
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

        UPDATE Inventory
        SET
            SlotCount = InInventorySlotCount,
            SlotData = InInventorySlotData
        WHERE CharacterID = InCharacterID;

        IF ROW_COUNT() = 0 THEN
            ROLLBACK;
            SET OutResult = 1;
            LEAVE ProcLabelBody;
        END IF;

        COMMIT;
        SET OutResult = 0;

    END ProcLabelBody;
END;
