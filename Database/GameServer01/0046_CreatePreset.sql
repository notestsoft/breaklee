CREATE TABLE Preset (
    CharacterID INT NOT NULL,
    ConfigurationData BLOB NOT NULL,
    ActiveEquipmentPresetIndex INT NOT NULL DEFAULT 0,
    ActiveAnimaMasteryPresetIndex INT NOT NULL DEFAULT 0,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Preset FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
