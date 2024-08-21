CREATE TABLE Transform (
    CharacterID INT NOT NULL,
    SlotCount SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL, 
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Transform FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
