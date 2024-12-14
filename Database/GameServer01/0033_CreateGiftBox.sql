CREATE TABLE Giftbox (
    CharacterID INT NOT NULL,
    SpecialBoxPoints SMALLINT NOT NULL DEFAULT 0,
    SlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    RewardSlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),
    
    CONSTRAINT FK_Character_Giftbox FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
