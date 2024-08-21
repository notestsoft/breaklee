CREATE TABLE AuraMastery (
    CharacterID INT NOT NULL,
    Points INT NOT NULL DEFAULT 0,
    AccumulatedTimeInMinutes INT NOT NULL DEFAULT 0,
    SlotCount TINYINT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_AuraMastery FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
