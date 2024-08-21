CREATE TABLE Buff (
    CharacterID INT NOT NULL,
    SkillCooldownCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    BuffSlotCount SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    SpiritRaiseBuffCooldown INT UNSIGNED NOT NULL DEFAULT 0,
    BuffSlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Buff FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
