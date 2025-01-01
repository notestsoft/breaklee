CREATE PROCEDURE SyncDiamondMeritMastery (
    IN InAccountID INT,
    IN InIsEnabled TINYINT UNSIGNED,
    IN InExp INT,
    IN InPoints INT,
    IN InActiveMemorizeIndex TINYINT UNSIGNED,
    IN InOpenSlotMasteryIndex INT,
    IN InOpenSlotUnlockTime BIGINT UNSIGNED,
    IN InTotalMemorizeSlotCount SMALLINT,
    IN InUnlockedSlotCount SMALLINT,
    IN InMasterySlotCount SMALLINT,
    IN InSpecialMasterySlotCount SMALLINT,
    IN InSpecialMasteryExp INT,
    IN InExtendedMemorizeSlotCount TINYINT,
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
        TotalMemorizeSlotCount = InTotalMemorizeSlotCount,
        UnlockedSlotCount = InUnlockedSlotCount,
        MasterySlotCount = InMasterySlotCount,
        SpecialMasterySlotCount = InSpecialMasterySlotCount,
        SpecialMasteryExp = InSpecialMasteryExp,
        ExtendedMemorizeSlotCount = InExtendedMemorizeSlotCount,
        ExtendedMemorizeSlotData = InExtendedMemorizeSlotData,
        UnlockedSlotData = InUnlockedSlotData,
        MasterySlotData = InMasterySlotData,
        SpecialMasterySlotData = InSpecialMasterySlotData
    WHERE AccountID = InAccountID;
END;
