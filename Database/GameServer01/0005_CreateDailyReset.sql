CREATE TABLE DailyReset (
    CharacterID INT NOT NULL, 
    LastResetTimestamp BIGINT UNSIGNED NOT NULL DEFAULT 0,
    NextResetTimestamp BIGINT UNSIGNED NOT NULL DEFAULT 0,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_DailyReset FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
