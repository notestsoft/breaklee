CREATE PROCEDURE SyncGiftbox (
    IN InCharacterID INT,
    IN InSpecialBoxPoints SMALLINT,
    IN InSlotCount TINYINT UNSIGNED,
    IN InSlotData BLOB,
    IN InRewardSlotData BLOB
)
BEGIN
    -- Update the existing record in Giftbox table
    UPDATE Giftbox
    SET
        SpecialBoxPoints = InSpecialBoxPoints,
        SlotCount = InSlotCount,
        SlotData = InSlotData,
        RewardSlotData = InRewardSlotData
    WHERE CharacterID = InCharacterID;
END;
