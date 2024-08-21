CREATE TABLE Quest (
    CharacterID INT NOT NULL,
    QuestSlotCount SMALLINT UNSIGNED NOT NULL DEFAULT 0,
    FinishedQuestData BLOB NOT NULL,
    DeletedQuestData BLOB NOT NULL,
    FinishedDungeonData BLOB NOT NULL,
    QuestSlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Quest FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
