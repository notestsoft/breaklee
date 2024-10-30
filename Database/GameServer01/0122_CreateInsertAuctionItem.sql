CREATE PROCEDURE InsertAuctionItem(
    IN InAccountID INT,
    IN InCharacterID INT,
    IN InSlotIndex TINYINT,
    IN InItemID BIGINT UNSIGNED,
    IN InItemOptions BIGINT UNSIGNED,
    IN InItemDuration INT UNSIGNED,
    IN InItemPrice BIGINT UNSIGNED,
    IN InItemCount INT,
    IN InCategory1 SMALLINT,
    IN InCategory2 SMALLINT,
    IN InCategory3 SMALLINT,
    IN InCategory4 SMALLINT,
    IN InCategory5 SMALLINT,
    IN InExpirationTime BIGINT UNSIGNED,
    IN InInventorySlotCount SMALLINT UNSIGNED,
    IN InInventorySlotData BLOB,
    OUT OutResult TINYINT UNSIGNED,
    OUT OutExpiresAt BIGINT UNSIGNED
)
BEGIN
    DECLARE TempCharacterName VARCHAR(50);

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET OutResult = 1;
    END;

    START TRANSACTION;

    SELECT Name INTO TempCharacterName
    FROM Characters
    WHERE CharacterID = InCharacterID
    LIMIT 1;

    UPDATE Inventory
    SET
        SlotCount = InInventorySlotCount,
        SlotData = InInventorySlotData
    WHERE CharacterID = InCharacterID;

    INSERT INTO AuctionItems (
        AccountID, 
        SlotIndex, 
        ItemID, 
        ItemOptions, 
        ItemDuration, 
        ItemPrice,
        ItemCount, 
        Category1, 
        Category2, 
        Category3, 
        Category4, 
        Category5, 
        CharacterID,
        CharacterName,
        ExpiresAt
    )
    VALUES (
        InAccountID, 
        InSlotIndex, 
        InItemID, 
        InItemOptions, 
        InItemDuration, 
        InItemPrice, 
        InItemCount, 
        InCategory1, 
        InCategory2, 
        InCategory3, 
        InCategory4, 
        InCategory5, 
        InCharacterID,
        TempCharacterName,
        UNIX_TIMESTAMP() + InExpirationTime
    );

    SET OutResult = 0;
    SET OutExpiresAt = UNIX_TIMESTAMP() + InExpirationTime;

    COMMIT;
END