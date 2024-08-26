CREATE PROCEDURE SyncBuff (
    IN InCharacterID INT,
    IN InSkillCooldownCount TINYINT UNSIGNED,
    IN InBuffSlotCount SMALLINT UNSIGNED,
    IN InSpiritRaiseBuffCooldown INT UNSIGNED,
    IN InBuffSlotData BLOB
)
BEGIN
    UPDATE Buff
    SET
        SkillCooldownCount = InSkillCooldownCount,
        BuffSlotCount = InBuffSlotCount,
        SpiritRaiseBuffCooldown = InSpiritRaiseBuffCooldown,
        BuffSlotData = InBuffSlotData
    WHERE CharacterID = InCharacterID;
END;
