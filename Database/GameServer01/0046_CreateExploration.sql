CREATE TABLE Exploration (
    CharacterID INT NOT NULL,
    EndDate BIGINT NOT NULL DEFAULT 0,
    Points INT NOT NULL DEFAULT 0,
    Level INT NOT NULL DEFAULT 0,
    SlotCount INT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Exploration FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
