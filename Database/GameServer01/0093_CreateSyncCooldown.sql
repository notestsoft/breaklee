CREATE PROCEDURE SyncCooldown (
    IN InCharacterID INT,
    IN InCooldownSlotCount SMALLINT UNSIGNED,
    IN InSpiritRaiseCooldown INT UNSIGNED,
    IN InCooldownSlotData BLOB
)
BEGIN
    UPDATE Cooldown
    SET
        SkillCooldownCount = InSkillCooldownCount,
        CooldownSlotCount = InCooldownSlotCount,
        SpiritRaiseCooldown = InSpiritRaiseCooldown,
        CooldownSlotData = InCooldownSlotData
    WHERE CharacterID = InCharacterID;
END;
