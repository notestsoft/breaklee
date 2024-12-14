CREATE PROCEDURE SyncDiamondMeritMastery (
    IN InAccountID INT,
    IN InIsEnabled TINYINT UNSIGNED,
    IN InExp INT,
    IN InPoints INT,
    IN InActiveMemorizeIndex TINYINT UNSIGNED,
    IN InOpenSlotMasteryIndex INT,
    IN InOpenSlotUnlockTime BIGINT UNSIGNED,
    IN InExtendedMemorizeSlotCount SMALLINT,
    IN InUnlockedSlotCount SMALLINT,
    IN InMasterySlotCount SMALLINT,
    IN InSpecialMasterySlotCount SMALLINT,
    IN InExtendedMasterySlotCount INT,
    IN InExtendedMemorizeSlotData BLOB,
    IN InUnlockedSlotData BLOB,
    IN InMasterySlotData BLOB,
    IN InSpecialMasterySlotData BLOB
)
BEGIN
    UPDATE DiamondMeritMastery
    SET
        IsEnabled = InIsEnabled,
        Exp = InExp,
        Points = InPoints,
        ActiveMemorizeIndex = InActiveMemorizeIndex,
        OpenSlotMasteryIndex = InOpenSlotMasteryIndex,
        OpenSlotUnlockTime = InOpenSlotUnlockTime,
        ExtendedMemorizeSlotCount = InExtendedMemorizeSlotCount,
        UnlockedSlotCount = InUnlockedSlotCount,
        MasterySlotCount = InMasterySlotCount,
        SpecialMasterySlotCount = InSpecialMasterySlotCount,
        ExtendedMasterySlotCount = InExtendedMasterySlotCount,
        ExtendedMemorizeSlotData = InExtendedMemorizeSlotData,
        UnlockedSlotData = InUnlockedSlotData,
        MasterySlotData = InMasterySlotData,
        SpecialMasterySlotData = InSpecialMasterySlotData
    WHERE AccountID = InAccountID;
END;
