CREATE PROCEDURE SyncAbility (
    IN InCharacterID INT,
    IN InAPTotal SMALLINT UNSIGNED,
    IN InAP SMALLINT UNSIGNED,
    IN InAxp INT UNSIGNED,
    IN InEssenceAbilityCount TINYINT UNSIGNED,
    IN InExtendedEssenceAbilityCount TINYINT UNSIGNED,
    IN InBlendedAbilityCount TINYINT UNSIGNED,
    IN InExtendedBlendedAbilityCount TINYINT UNSIGNED,
    IN InKarmaAbilityCount TINYINT UNSIGNED,
    IN InExtendedKarmaAbilityCount TINYINT UNSIGNED,
    IN InEssenceAbilityData BLOB,
    IN InBlendedAbilityData BLOB,
    IN InKarmaAbilityData BLOB
)
BEGIN
    UPDATE Ability
    SET
        APTotal = InAPTotal,
        AP = InAP,
        Axp = InAxp,
        EssenceAbilityCount = InEssenceAbilityCount,
        ExtendedEssenceAbilityCount = InExtendedEssenceAbilityCount,
        BlendedAbilityCount = InBlendedAbilityCount,
        ExtendedBlendedAbilityCount = InExtendedBlendedAbilityCount,
        KarmaAbilityCount = InKarmaAbilityCount,
        ExtendedKarmaAbilityCount = InExtendedKarmaAbilityCount,
        EssenceAbilityData = InEssenceAbilityData,
        BlendedAbilityData = InBlendedAbilityData,
        KarmaAbilityData = InKarmaAbilityData
    WHERE CharacterID = InCharacterID;
END;
