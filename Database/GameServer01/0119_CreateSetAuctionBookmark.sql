CREATE PROCEDURE SetAuctionBookmark (
    IN InAccountID INT,
    IN InSlotIndex TINYINT,
    IN InCategory1 SMALLINT,
    IN InCategory2 SMALLINT,
    IN InCategory3 SMALLINT,
    IN InCategory4 SMALLINT,
    IN InCategory5 SMALLINT,
    IN InSubCategory1 TINYINT,
    IN InSubCategory2 TINYINT,
    IN InDescription VARCHAR(49),
    OUT OutResult TINYINT
)
BEGIN
    IF InSlotIndex < 0 OR InSlotIndex > 19 THEN
        SET OutResult = 0;
    ELSE
        BEGIN
            DECLARE EXIT HANDLER FOR SQLEXCEPTION
            BEGIN
                -- Set failure in case of any SQL error
                SET OutResult = 0;
            END;

            INSERT INTO AuctionBookmarks (
                AccountID, 
                SlotIndex, 
                Category1, 
                Category2, 
                Category3, 
                Category4, 
                Category5, 
                SubCategory1, 
                SubCategory2, 
                Description, 
                CreatedAt, 
                UpdatedAt
            ) VALUES (
                InAccountID, 
                InSlotIndex, 
                InCategory1, 
                InCategory2, 
                InCategory3, 
                InCategory4, 
                InCategory5, 
                InSubCategory1, 
                InSubCategory2, 
                InDescription, 
                UNIX_TIMESTAMP(), 
                UNIX_TIMESTAMP()
            )
            ON DUPLICATE KEY UPDATE
                Category1 = InCategory1,
                Category2 = InCategory2,
                Category3 = InCategory3,
                Category4 = InCategory4,
                Category5 = InCategory5,
                SubCategory1 = InSubCategory1,
                SubCategory2 = InSubCategory2,
                Description = InDescription,
                UpdatedAt = UNIX_TIMESTAMP();

            SET OutResult = 1;
        END;
    END IF;
END;