CREATE PROCEDURE DeleteAuctionBookmark (
    IN InAccountID INT,
    IN InSlotIndex TINYINT
)
BEGIN
    DELETE FROM AuctionBookmarks
    WHERE AccountID = InAccountID
      AND SlotIndex = InSlotIndex;
END