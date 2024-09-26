CREATE PROCEDURE GetAuctionBookmarks (
    IN InAccountID INT
)
BEGIN
    SELECT 
        SlotIndex,
        Category1,
        Category2,
        Category3,
        Category4,
        Category5,
        SubCategory1,
        SubCategory2,
        Description
    FROM AuctionBookmarks
    WHERE AccountID = InAccountID;
END;
