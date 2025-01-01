CREATE PROCEDURE SyncMythMastery (
    IN InCharacterID INT,
    IN InRebirth INT,
    IN InHolyPower INT,
    IN InLevel INT,
    IN InExp BIGINT UNSIGNED,
    IN InPoints INT,
    IN InUnlockedPageCount INT,
    IN InPropertySlotCount TINYINT UNSIGNED,
    IN InStigmaGrade INT,
    IN InStigmaExp INT,
    IN InPropertySlotData BLOB
)
BEGIN
    -- Update the existing record in MythMastery table
    UPDATE MythMastery
    SET
        Rebirth = InRebirth,
        HolyPower = InHolyPower,
        Level = InLevel,
        Exp = InExp,
        Points = InPoints,
        UnlockedPageCount = InUnlockedPageCount,
        PropertySlotCount = InPropertySlotCount,
        StigmaGrade = InStigmaGrade,
        StigmaExp = InStigmaExp,
        PropertySlotData = InPropertySlotData
    WHERE CharacterID = InCharacterID;
END;
