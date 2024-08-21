CREATE TABLE Recovery (
    CharacterID INT NOT NULL,
    Count INT NOT NULL DEFAULT 0,
    PriceData BLOB NOT NULL,
    SlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Recovery FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
