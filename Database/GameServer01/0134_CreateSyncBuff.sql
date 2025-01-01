CREATE PROCEDURE SyncBuff (
    IN InCharacterID INT,
    IN InSkillBuffCount TINYINT UNSIGNED,
    IN InPotionBuffCount TINYINT UNSIGNED,
    IN InGmBuffCount TINYINT UNSIGNED,
    IN InUnknownBuffCount1 TINYINT UNSIGNED,
    IN InUnknownBuffCount2 TINYINT UNSIGNED,
    IN InForceWingBuffCount TINYINT UNSIGNED,
    IN InFirePlaceBuffCount TINYINT UNSIGNED,
    IN InSlotData BLOB
)
BEGIN
    UPDATE Buff
    SET
        SkillBuffCount = InSkillBuffCount,
        PotionBuffCount = InPotionBuffCount,
        GmBuffCount = InGmBuffCount,
        UnknownBuffCount1 = InUnknownBuffCount1,
        UnknownBuffCount2 = InUnknownBuffCount2,
        ForceWingBuffCount = InForceWingBuffCount,
        FirePlaceBuffCount = InFirePlaceBuffCount,
        SlotData = InSlotData
    WHERE CharacterID = InCharacterID;
END;
