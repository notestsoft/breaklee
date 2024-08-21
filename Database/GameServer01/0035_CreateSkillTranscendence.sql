CREATE TABLE SkillTranscendence (
    CharacterID INT NOT NULL,
    Points INT NOT NULL DEFAULT 0,
    SlotCount INT NOT NULL DEFAULT 0,
    SlotData BLOB NOT NULL,
    
    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_SkillTranscendence FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
