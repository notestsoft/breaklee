CREATE TABLE Costume (
    CharacterID INT NOT NULL,
    PageCount INT NOT NULL DEFAULT 0,
    AppliedSlotCount INT NOT NULL DEFAULT 0,
    ActivePageIndex INT NOT NULL DEFAULT 0,
    PageData BLOB NOT NULL,
    AppliedSlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Costume FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
