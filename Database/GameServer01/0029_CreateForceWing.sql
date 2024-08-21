CREATE TABLE ForceWing (
    CharacterID INT NOT NULL,
    Grade TINYINT UNSIGNED NOT NULL DEFAULT 0,
    Level TINYINT UNSIGNED NOT NULL DEFAULT 0,
    Exp BIGINT NOT NULL DEFAULT 0,
    ActivePresetIndex TINYINT UNSIGNED NOT NULL DEFAULT 0,
    PresetEnabled BLOB NOT NULL,               -- Serialized array of TINYINT
    PresetTrainingPointCount BLOB NOT NULL,    -- Serialized array of INT
    PresetSlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    TrainingSlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    TrainingUnlockFlagData BLOB NOT NULL,
    ArrivalSkillSlotData BLOB NOT NULL,
    ArrivalSkillRestoreSlotData BLOB NOT NULL, 
    PresetSlotData BLOB NOT NULL, 
    TrainingSlotData BLOB NOT NULL, 

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_ForceWing FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
