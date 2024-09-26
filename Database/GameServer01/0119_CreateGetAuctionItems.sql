CREATE PROCEDURE GetAuctionItems (
    IN InAccountID INT
)
BEGIN
    SELECT 
        SlotIndex,
        ItemID,
        ItemOptions,
        ItemPrice,
        ItemCount,
        SoldItemCount,
        ExpiresAt,
        CreatedAt
    FROM AuctionItems
    WHERE AccountID = InAccountID;
END;
