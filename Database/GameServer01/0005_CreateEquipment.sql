CREATE TABLE Equipment (
    CharacterID INT NOT NULL, 
    EquipmentSlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    InventorySlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    LinkSlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    LockSlotCount TINYINT UNSIGNED NOT NULL DEFAULT 0,
    EquipmentSlotData BLOB NOT NULL,
    InventorySlotData BLOB NOT NULL,
    LinkSlotData BLOB NOT NULL,
    LockSlotData BLOB NOT NULL,

    PRIMARY KEY (CharacterID),

    -- Foreign key constraint
    CONSTRAINT FK_Character_Equipment FOREIGN KEY (CharacterID)
        REFERENCES Characters(CharacterID)
        ON DELETE CASCADE
) ENGINE = INNODB DEFAULT CHARSET = utf8;
