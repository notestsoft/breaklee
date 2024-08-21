CREATE TABLE DamageBooster (
    CharacterID INT NOT NULL,
    ItemData BLOB NOT NULL,
    SlotCount TINYINT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_DamageBooster FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
