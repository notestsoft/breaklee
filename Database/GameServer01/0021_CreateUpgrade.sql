CREATE TABLE Upgrade (
    CharacterID INT NOT NULL,
    Point INT NOT NULL DEFAULT 0,
    Timestamp BIGINT NOT NULL DEFAULT 0,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Upgrade FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
