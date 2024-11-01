CREATE TABLE OverlordMastery (
    CharacterID INT NOT NULL,
    Level SMALLINT NOT NULL DEFAULT 0,
    Exp BIGINT NOT NULL DEFAULT 0,
    Points SMALLINT NOT NULL DEFAULT 0,
    SlotCount TINYINT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_OverlordMastery FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
