CREATE TABLE Craft (
    CharacterID INT NOT NULL,
    SlotCount INT NOT NULL DEFAULT 0,
    Energy INT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Craft FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
