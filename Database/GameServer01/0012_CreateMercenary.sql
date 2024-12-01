CREATE TABLE Mercenary (
    CharacterID INT NOT NULL,
    SlotCount INT UNSIGNED NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Mercenary FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
