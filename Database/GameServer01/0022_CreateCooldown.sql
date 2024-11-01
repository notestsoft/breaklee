CREATE TABLE Cooldown (
    CharacterID INT NOT NULL,
    CooldownSlotCount SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    SpiritRaiseCooldown INT UNSIGNED NOT NULL DEFAULT 0,
    CooldownSlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Cooldown FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
