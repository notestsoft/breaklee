CREATE TABLE HonorMedal (
    CharacterID INT NOT NULL,
    Grade INT NOT NULL DEFAULT 0,
    Score INT NOT NULL DEFAULT 0,
    SlotCount TINYINT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_HonorMedal FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
