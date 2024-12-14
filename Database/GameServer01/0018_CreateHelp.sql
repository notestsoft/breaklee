CREATE TABLE Help (
    CharacterID INT NOT NULL,
    HelpWindow INT UNSIGNED NOT NULL DEFAULT 0,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Help FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
