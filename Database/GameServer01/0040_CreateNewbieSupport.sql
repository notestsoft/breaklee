CREATE TABLE NewbieSupport (
    CharacterID INT NOT NULL,
    Timestamp BIGINT UNSIGNED NOT NULL DEFAULT 946652400,
    SlotCount INT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_NewbieSupport FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
