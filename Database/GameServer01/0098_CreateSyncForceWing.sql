CREATE PROCEDURE SyncForceWing (
    IN InCharacterID INT,
    IN InGrade TINYINT UNSIGNED,
    IN InLevel TINYINT UNSIGNED,
    IN InExp BIGINT,
    IN InActivePresetIndex TINYINT UNSIGNED,
    IN InPresetEnabled BLOB,
    IN InPresetTrainingPointCount BLOB,
    IN InPresetSlotCount TINYINT UNSIGNED,
    IN InTrainingSlotCount TINYINT UNSIGNED,
    IN InTrainingUnlockFlagData BLOB,
    IN InArrivalSkillSlotData BLOB,
    IN InArrivalSkillRestoreSlotData BLOB,
    IN InPresetSlotData BLOB,
    IN InTrainingSlotData BLOB
)
BEGIN
    -- Update the existing record in ForceWing table
    UPDATE ForceWing
    SET
        Grade = InGrade,
        Level = InLevel,
        Exp = InExp,
        ActivePresetIndex = InActivePresetIndex,
        PresetEnabled = InPresetEnabled,
        PresetTrainingPointCount = InPresetTrainingPointCount,
        PresetSlotCount = InPresetSlotCount,
        TrainingSlotCount = InTrainingSlotCount,
        TrainingUnlockFlagData = InTrainingUnlockFlagData,
        ArrivalSkillSlotData = InArrivalSkillSlotData,
        ArrivalSkillRestoreSlotData = InArrivalSkillRestoreSlotData,
        PresetSlotData = InPresetSlotData,
        TrainingSlotData = InTrainingSlotData
    WHERE CharacterID = InCharacterID;
END;
