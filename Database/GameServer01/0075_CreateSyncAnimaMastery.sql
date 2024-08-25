CREATE PROCEDURE SyncAnimaMastery (
    IN InAccountID INT,
    IN InPresetCount INT,
    IN InStorageCount INT,
    IN InUnlockedCategoryFlags INT UNSIGNED,
    IN InPresetData BLOB,
    IN InCategoryData BLOB
)
BEGIN
    UPDATE AnimaMastery
    SET
        PresetCount = InPresetCount,
        StorageCount = InStorageCount,
        UnlockedCategoryFlags = InUnlockedCategoryFlags,
        PresetData = InPresetData,
        CategoryData = InCategoryData
    WHERE AccountID = InAccountID;
END;
