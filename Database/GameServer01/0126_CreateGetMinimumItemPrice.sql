CREATE PROCEDURE GetMinimumItemPrice(
    IN InItemID BIGINT UNSIGNED,
    IN InItemOptions BIGINT UNSIGNED,
    OUT OutPrice BIGINT UNSIGNED
)
BEGIN
    SELECT COALESCE(MIN(ItemPrice), 0)
    INTO OutPrice
    FROM AuctionItems
    WHERE ItemID = InItemID 
      AND ItemOptions = InItemOptions
      AND ItemCount > SoldItemCount;
END