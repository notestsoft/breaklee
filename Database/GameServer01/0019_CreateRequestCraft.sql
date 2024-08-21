CREATE TABLE RequestCraft (
    CharacterID INT NOT NULL,
    SlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    Exp SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    RegisteredFlagData BLOB NOT NULL,
    FavoriteFlagData BLOB NOT NULL,
    SortingOrder SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_RequestCraft FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
