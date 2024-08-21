CREATE TABLE Appearance (
    CharacterID INT NOT NULL,
    EquipmentSlotCount TINYINT NOT NULL DEFAULT 0,
    InventorySlotCount SMALLINT NOT NULL DEFAULT 0,
    EquipmentSlotData BLOB NOT NULL,
    InventorySlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    CONSTRAINT FK_Character_Appearance FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
