CREATE PROCEDURE SearchAuctionItems(
    IN InCategory2 TINYINT UNSIGNED,
    IN InCategory3 SMALLINT UNSIGNED,
    IN InCategory4 TINYINT UNSIGNED,
    IN InCategory5 SMALLINT UNSIGNED,
    IN InSortOrder SMALLINT UNSIGNED
)
BEGIN
    SELECT 
        ItemID,
        ItemOptions,
        ItemDuration,
        (ItemCount - SoldItemCount),
        ItemPrice,
        AccountID,
        CharacterName
    FROM AuctionItems
    WHERE (InCategory2 = 0 OR Category2 = 0 OR Category2 = InCategory2)
      AND (InCategory3 = 0 OR Category3 = 0 OR Category3 = InCategory3)
      AND (InCategory4 = 0 OR Category4 = 0 OR Category4 = InCategory4)
      AND (InCategory5 = 0 OR Category5 = 0 OR Category5 = InCategory5)
      AND ItemCount > SoldItemCount
      AND ExpiresAt < UNIX_TIMESTAMP()
    ORDER BY 
        CASE 
            WHEN InSortOrder = 0 THEN ItemPrice END DESC,
        CASE 
            WHEN InSortOrder = 1 THEN ItemPrice END ASC;
END;
