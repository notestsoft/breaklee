CREATE PROCEDURE SyncCostume (
    IN InCharacterID INT,
    IN InPageCount INT,
    IN InAppliedSlotCount INT,
    IN InActivePageIndex INT,
    IN InPageData BLOB,
    IN InAppliedSlotData BLOB
)
BEGIN
    -- Update the existing record in Costume table
    UPDATE Costume
    SET
        PageCount = InPageCount,
        AppliedSlotCount = InAppliedSlotCount,
        ActivePageIndex = InActivePageIndex,
        PageData = InPageData,
        AppliedSlotData = InAppliedSlotData
    WHERE CharacterID = InCharacterID;
END;
