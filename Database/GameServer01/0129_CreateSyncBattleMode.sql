CREATE PROCEDURE SyncBattleMode (
    IN InCharacterID INT,
    IN InBattleModeDuration INT UNSIGNED,
    IN InBattleModeIndex TINYINT UNSIGNED,
    IN InBattleModeUnknown1 TINYINT UNSIGNED,
    IN InBattleModeStyleRank TINYINT UNSIGNED,
    IN InAuraModeIndex TINYINT UNSIGNED,
    IN InAuraModeUnknown1 TINYINT UNSIGNED,
    IN InAuraModeStyleRank TINYINT UNSIGNED,
    IN InAuraModeDuration INT UNSIGNED,
    IN InVehicleState INT
)
BEGIN
    UPDATE BattleMode
    SET
        BattleModeDuration = InBattleModeDuration,
        BattleModeIndex = InBattleModeIndex,
        BattleModeUnknown1 = InBattleModeUnknown1,
        BattleModeStyleRank = InBattleModeStyleRank,
        AuraModeIndex = InAuraModeIndex,
        AuraModeUnknown1 = InAuraModeUnknown1,
        AuraModeStyleRank = InAuraModeStyleRank,
        AuraModeDuration = InAuraModeDuration,
        VehicleState = InVehicleState
    WHERE CharacterID = InCharacterID;
END;
