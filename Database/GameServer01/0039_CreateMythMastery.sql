CREATE TABLE MythMastery (
    CharacterID INT NOT NULL,
    Rebirth INT NOT NULL DEFAULT 0,
    HolyPower INT NOT NULL DEFAULT 0,
    Level INT NOT NULL DEFAULT 0,
    Exp BIGINT UNSIGNED NOT NULL DEFAULT 0,
    Points INT NOT NULL DEFAULT 0,
    UnlockedPageCount INT NOT NULL DEFAULT 0,
    PropertySlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    StigmaGrade INT NOT NULL DEFAULT 0,
    StigmaExp INT NOT NULL DEFAULT 0,
    PropertySlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_MythMastery FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
